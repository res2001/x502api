/* Данный пример представляет из себя консольную программу на языке C,
   демонстрирующую работу с модулями L502 и E502 на примере синхронного
   ввода данных с АЦП и цифровых линий.

   Перед сбором в примере идет поиск модулей, подключенных по интерфейсам PCI-Express и USB,
   и предоставляется список для выбора модуля, с которым нужно работать.
   Для подключения по Ethernet нужно указать IP-адреса интересующих модулей
   в качестве аргументов командной строки при вызове примера, тогда эти адреса
   будут добавлены в конец списка выбора модуля.
   Например, если интересуют модули с адресами 192.168.1.5 и 192.168.1.6,
   то пример можно вызвать:
   x502_stream_read 192.168.1.5 192.168.1.6
   и две дополнительные строки с этими адресами появятся в списке выбора.
   
   Настройки частот, количества принимаемых данных и т.д. задаются с помощью макросов в
   начале программы.
   Настройки логических каналов - с помощью таблиц f_channels/f_ch_modes/f_ch_ranges.

   Пример выполняет прием блоков данных заданного размера.
   Сбор идет до нажатия любой клавиши на Windows или  CTRL+C на Linux.

   Пример также показывает как выполнять обработку данных и определять начало кадра,
   в случае если в X502_ProcessData() передается не целое число кадров.

   Данный пример содержит проект для Visual Studio 2008, а также может быть собран
   gcc в Linux или mingw в Windows через makefile или с помощью cmake (подробнее
   в комментариях в соответствующих файлах).

   Для того чтобы собрать проект в Visual Studio, измените путь к заголовочным файлам
   (Проект (Project) -> Свойства (Properties) -> Свойства конфигурации (Configuration Properties)
   -> С/С++ -> Общие (General) -> Дополнительные каталоги включения (Additional Include Directories))
   на тот, где у вас лежат заголовочный файлы x502api.h, l502api.h и e502api.h и измените путь к библиотекам
   (Проект (Project) -> Свойства (Properties) -> Свойства конфигурации (Configuration Properties) ->
   Компановщик (Linker) -> Общие (General) -> Дополнительные катологи библиотек (Additional Library Directories)).

   Внимание!!: Если Вы собираете проект под Visual Studio и взяли проект с сайта (а не из SDK),
   то для корректного отображения русских букв в программе нужно изменить кодировку
   или указать сохранение с сигнатурой кодировки для UTF-8:
   выберите Файл (File) -> Дополнительные параметры сохранения (Advanced Save Options)...
   и в поле Кодировка (Encoding) выберите Юникод (UTF8, с сигнатурой)/Unicode (UTF-8 with signature)
   и сохраните изменения в файле.

   */

//#ifdef X502API_ENABLE_DEV_L502
//    #include "l502api.h"
//#endif
#ifdef X502API_ENABLE_DEV_E502
    #include "e502api.h"
#else
    #error E502 not supported!
#endif

#ifdef _WIN32
#include <locale.h>
#include <conio.h>
#else
#include <signal.h>
#include <unistd.h>
#endif

#include <stdio.h>
#include <stdlib.h>


#define SLEEP_TIME_MS 10

/* количество используемых логических каналов */
#define ADC_LCH_CNT  3

/* частота сбора АЦП в Гц*/
#define ADC_FREQ          500000
/* частота кадров (на логический канал). При ADC_FREQ/ADC_LCH_CNT - межкадровой задержки нет */
#define ADC_FRAME_FREQ    (ADC_FREQ/ADC_LCH_CNT)

#define TCP_CONNECTION_TOUT 5000


/* сколько отсчетов считываем за блок */
#define READ_BLOCK_SIZE   4096*200
/* таймаут на прием блока (мс) */
#define READ_TIMEOUT     2000


/* номера используемых физических каналов */
static uint32_t f_channels[ADC_LCH_CNT] = {0,4,6};
/* режимы измерения для каналов */
static uint32_t f_ch_modes[ADC_LCH_CNT] = {X502_LCH_MODE_DIFF, X502_LCH_MODE_DIFF, X502_LCH_MODE_DIFF};
/* диапазоны измерения для каналов */
static uint32_t f_ch_ranges[ADC_LCH_CNT] = {X502_ADC_RANGE_10, X502_ADC_RANGE_10, X502_ADC_RANGE_10};



/* признак необходимости завершить сбор данных */
static int f_out = 0;

#ifndef _WIN32
/* Обработчик сигнала завершения для Linux */
static void f_abort_handler(int sig) {
    f_out = 1;
}
#endif


/* настройка параметров модуля */
int32_t f_setup_params(t_x502_hnd hnd) {
    int32_t err = X502_ERR_OK, i;

    /* устанавливаем параметры логической таблицы АЦП */
    err = X502_SetLChannelCount(hnd, ADC_LCH_CNT);
    for (i=0; (i < ADC_LCH_CNT) && (err == X502_ERR_OK); i++)
        err = X502_SetLChannel(hnd, i, f_channels[i], f_ch_modes[i], f_ch_ranges[i], 0);

    /* устанавливаем частоты ввода для АЦП и цифровых входов */
    if (err == X502_ERR_OK) {
        double f_adc = ADC_FREQ, f_frame = ADC_FRAME_FREQ;
        err = X502_SetAdcFreq(hnd, &f_adc, &f_frame);

        if (err == X502_ERR_OK) {
            /* выводим реально установленные значения - те что вернули функции */
            printf("Установлены частоты:\n    Частота сбора АЦП = %0.0f\n"
                "    Частота на лог. канал = %0.0f\n", f_adc, f_frame);
        }
    }

    /* записываем настройки в модуль */
    if (err == X502_ERR_OK)
        err = X502_Configure(hnd, 0);

    /* разрешаем синхронные потоки */
    if (err == X502_ERR_OK) {
        err = X502_StreamsEnable(hnd, X502_STREAM_ADC | X502_STREAM_DIN);
    }

    if (err == X502_ERR_OK) {
        err = X502_StreamsEnable(hnd, X502_STREAM_DOUT);
    }


    return err;
}

#ifdef SEND_TEST
static int32_t f_send_data(t_x502_hnd hnd, int32_t size) {
    int32_t err = X502_ERR_OK;
    uint32_t *dout = malloc(size*sizeof(dout[0]));
    uint32_t *snd_vals = malloc(size * sizeof(snd_vals[0]));
    if ((dout != NULL) && (snd_vals != NULL)) {

        err = X502_PrepareData(hnd, NULL, NULL, dout, size, 0, snd_vals);
        if (err == X502_ERR_OK) {
            int32_t sent_cnt = X502_Send(hnd, snd_vals, size, 200);
            if (sent_cnt < 0) {
                err = sent_cnt;
                fprintf(stderr, "Ошибка передачи данных: %s!\n", X502_GetErrorString(err));
            } else if (sent_cnt != size) {
                fprintf(stderr, "Передано недостаточно данных! Запрашивалось %d, передали %d\n", size, sent_cnt);
                err = -1;
            }
        }
    } else {
        err = X502_ERR_MEMORY_ALLOC;
    }
    free(dout);
    free(snd_vals);
    return err;
}
#endif

static int32_t f_recv(t_x502_hnd hnd, int32_t size) {
    int32_t err = X502_ERR_OK;
    double *adc = malloc(size * sizeof(adc[0]));
    uint32_t *rcv_vals = malloc(size * sizeof(rcv_vals[0]));
    if ((rcv_vals != NULL) && (adc != NULL)) {
        /* принимаем данные (по таймауту) */
        int32_t rcv_size = X502_Recv(hnd, rcv_vals, size, 2);
        /* результат меньше нуля означает ошибку */
        if (rcv_size < 0) {
            err = rcv_size;
            fprintf(stderr, "Ошибка приема данных: %s\n", X502_GetErrorString(err));
        } else if (rcv_size != size) {
            fprintf(stderr, "Принято недостаточно данных! Запрашивалось %d, передали %d\n", size, rcv_size);
            err = -1;
        } else {
            /* обрабатываем принятые данные, распределяя их на данные АЦП и цифровых входов */
            err = X502_ProcessAdcData(hnd, rcv_vals, adc, (uint32_t*)&rcv_size, X502_PROC_FLAGS_VOLT);
            if (err != X502_ERR_OK) {
                fprintf(stderr, "Ошибка обработки данных: %s\n", X502_GetErrorString(err));
            } else {
               printf("recv %d words!\n", size);
            }
        }
    } else {
        err = X502_ERR_MEMORY_ALLOC;
        printf("Ошибка выделения памяти (size = %d)\n", size);
    }
    free(adc);
    free(rcv_vals);
    return err;
}

int main(int argc, char** argv) {
    int32_t err = X502_ERR_OK;
    uint32_t ver;
    t_x502_hnd hnd = NULL;
#ifndef _WIN32
    struct sigaction sa;
    /* В ОС Linux устанавливаем свой обработчик на сигнал закрытия,
       чтобы завершить сбор корректно */
    sa.sa_handler = f_abort_handler;
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGABRT, &sa, NULL);
#endif
#ifdef _WIN32
    /* для вывода русских букв в консоль для ОС Windows в CP1251 без перевода в OEM */
    setlocale(LC_CTYPE, "");
#endif
    /* получаем версию библиотеки */
    ver = X502_GetLibraryVersion();
    printf("Версия библиотеки: %d.%d.%d\n", (ver >> 24)&0xFF, (ver>>16)&0xFF, (ver>>8)&0xFF);

    /********** Получение списка устройств и выбор, с каким будем работать ******************/
#ifdef E502API_ENABLE_TCP
    if (argc < 2) {
        fprintf(stderr, "Не указан IP-адрес устройства!\n");
    } else {
        hnd = X502_Create();
        if (hnd != NULL) {
            int a[4];
            uint32_t ip_addr;
            if (sscanf(argv[1], "%d.%d.%d.%d", &a[0], &a[1], &a[2], &a[3])==4) {
                ip_addr = ((a[0] & 0xFF) << 24) |
                          ((a[1] & 0xFF) << 16) |
                          ((a[2] & 0xFF) <<  8) |
                          (a[3] & 0xFF);

                err = E502_OpenByIpAddr(hnd, ip_addr, 0, 5000);
                if (err != X502_ERR_OK) {
                    fprintf(stderr, "Ошибка подключения: %s!", X502_GetErrorString(err));
                }
            } else {
                err = -2;
                fprintf(stderr, "Неверно задан IP-адрес модуля\n");
            }
        } else {
            err = X502_ERR_MEMORY_ALLOC;
        }
    }
#endif  // E502API_ENABLE_TCP


    /********************************** Работа с модулем **************************/
    /* если успешно выбрали модуль и установили с ним связь - продолжаем работу */
    if (err == X502_ERR_OK) {
        /* получаем информацию */
        t_x502_info info;
        err = X502_GetDevInfo(hnd, &info);
        if (err != X502_ERR_OK) {
            fprintf(stderr, "Ошибка получения серийного информации о модуле: %s!", X502_GetErrorString(err));
        } else {
            /* выводим полученную информацию */
            printf("Установлена связь со следующим модулем:\n");
            printf(" Серийный номер          : %s\n", info.serial);
            printf(" Наличие ЦАП             : %s\n", info.devflags & X502_DEVFLAGS_DAC_PRESENT ? "Да" : "Нет");
            printf(" Наличие BlackFin        : %s\n", info.devflags & X502_DEVFLAGS_BF_PRESENT ? "Да" : "Нет");
            printf(" Наличие гальваноразвязки: %s\n", info.devflags & X502_DEVFLAGS_GAL_PRESENT ? "Да" : "Нет");
            printf(" Индустриальное исп.     : %s\n", info.devflags & X502_DEVFLAGS_INDUSTRIAL ? "Да" : "Нет");
            printf(" Наличие интерф. PCI/PCIe: %s\n", info.devflags & X502_DEVFLAGS_IFACE_SUPPORT_PCI ? "Да" : "Нет");
            printf(" Наличие интерф. USB     : %s\n", info.devflags & X502_DEVFLAGS_IFACE_SUPPORT_USB ? "Да" : "Нет");
            printf(" Наличие интерф. Ethernet: %s\n", info.devflags & X502_DEVFLAGS_IFACE_SUPPORT_ETH ? "Да" : "Нет");
            printf(" Версия ПЛИС             : %d.%d\n", (info.fpga_ver >> 8) & 0xFF, info.fpga_ver & 0xFF);
            printf(" Версия PLDA             : %d\n", info.plda_ver);
            if (info.mcu_firmware_ver != 0) {
                printf(" Версия прошивки ARM     : %d.%d.%d.%d\n",
                       (info.mcu_firmware_ver >> 24) & 0xFF,
                       (info.mcu_firmware_ver >> 16) & 0xFF,
                       (info.mcu_firmware_ver >>  8) & 0xFF,
                       info.mcu_firmware_ver & 0xFF);
            }
        }


        if (err == X502_ERR_OK) {
            /* настраиваем параметры модуля */
            err = f_setup_params(hnd);
            if (err != X502_ERR_OK)
                fprintf(stderr, "Ошибка настройки модуля: %s!\n", X502_GetErrorString(err));
        }

        if (err == X502_ERR_OK) {
            err = X502_PreloadStart(hnd);
            if (err != X502_ERR_OK)
                fprintf(stderr, "Ошибка предзагрузки данных: %s!\n", X502_GetErrorString(err));
        }

        /* запуск синхронного ввода-вывода */
        if (err == X502_ERR_OK) {
            err = X502_StreamsStart(hnd);
            if (err != X502_ERR_OK)
                fprintf(stderr, "Ошибка запуска сбора данных: %s!\n", X502_GetErrorString(err));
        }


        if (err == X502_ERR_OK) {
            int block;
            int32_t stop_err;

            printf("Сбор данных запущен. Для останова нажмите %s\n",
#ifdef _WIN32
                   "любую клавишу"
#else
                   "CTRL+C"
#endif
                   );
            fflush(stdout);



            for (block = 0; (err == X502_ERR_OK) && !f_out; block++) {                
                uint32_t rdy_cnt;

#ifdef SEND_TEST
                err = X502_GetSendReadyCount(hnd, &rdy_cnt);
                if (err == X502_ERR_OK)
                    err = f_send_data(hnd, rdy_cnt);
#endif
                err = X502_GetRecvReadyCount(hnd, &rdy_cnt);
                if (err != X502_ERR_OK)
                    fprintf(stderr, "Ошибка получения счетчика: %s\n", X502_GetErrorString(err));
                if ((err == X502_ERR_OK) && (rdy_cnt!=0))
                    err = f_recv(hnd, rdy_cnt);

#ifdef _WIN32
                Sleep(SLEEP_TIME_MS);
#else
                usleep(SLEEP_TIME_MS*1000);
#endif

#ifdef _WIN32
                /* проверка нажатия клавиши для выхода */
                if (err == X502_ERR_OK) {
                    if (_kbhit())
                        f_out = 1;
                }
#endif
            }

            /* останавливаем поток сбора данных (независимо от того, была ли ошибка) */
            stop_err = X502_StreamsStop(hnd);
            if (stop_err != X502_ERR_OK) {
                fprintf(stderr, "Ошибка останова сбора данных: %s\n", X502_GetErrorString(err));
                if (err == X502_ERR_OK)
                    err = stop_err;
            } else {
                printf("Сбор данных остановлен успешно\n");
            }
        }

        /* закрываем связь с модулем */
        X502_Close(hnd);
        /* освобождаем описатель */
        X502_Free(hnd);
    }
    return err;
}
