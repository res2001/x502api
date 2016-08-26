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

#include "l502api.h"
#include "e502api.h"

#ifdef _WIN32
#include <locale.h>
#include <conio.h>
#else
#include <signal.h>
#include <unistd.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* количество используемых логических каналов */
#define ADC_LCH_CNT  3

/* частота сбора АЦП в Гц*/
#define ADC_FREQ          2000000
/* частота кадров (на логический канал). При ADC_FREQ/ADC_LCH_CNT - межкадровой задержки нет */
#define ADC_FRAME_FREQ    (ADC_FREQ/ADC_LCH_CNT)
/* частота синхронного ввода в Гц*/
#define DIN_FREQ          2000000


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


/* Функция находит все подключенные модули по интерфейсам PCI-Express и USB и
 * сохраняет записи о этих устройствах в выделенный массив.
 * Также создаются записи по переданным IP-адресам модулей и добавляются в конец
 * массива.
 * Указатель на выделенный массив, который должен быть потом очищен, сохраняется
 * в pdevrec_list, а количество действительных элементов (память которых должна
 * быть в дальнейшем освобождена с помощью X502_FreeDevRecordList()) возвращается
 * как результат функции */
static uint32_t f_get_all_devrec(t_x502_devrec **pdevrec_list, uint32_t *ip_addr_list, unsigned ip_cnt) {
    int32_t fnd_devcnt = 0;
    uint32_t pci_devcnt = 0;
    uint32_t usb_devcnt = 0;

    t_x502_devrec *rec_list = NULL;

    /* получаем количество подключенных устройств по интерфейсам PCI и USB */
    L502_GetDevRecordsList(NULL, 0, 0, &pci_devcnt);
    E502_UsbGetDevRecordsList(NULL, 0, 0, &usb_devcnt);

    if ((pci_devcnt + usb_devcnt + ip_cnt) != 0) {
        /* выделяем память для массива для сохранения найденного количества записей */
        rec_list = malloc((pci_devcnt + usb_devcnt + ip_cnt) * sizeof(t_x502_devrec));

        if (rec_list != NULL) {
            unsigned i;
            /* получаем записи о модулях L502, но не больше pci_devcnt */
            if (pci_devcnt!=0) {
                int32_t res = L502_GetDevRecordsList(&rec_list[fnd_devcnt], pci_devcnt, 0, NULL);
                if (res >= 0) {
                    fnd_devcnt += res;
                }
            }
            /* добавляем записи о модулях E502, подключенных по USB, в конец массива */
            if (usb_devcnt!=0) {
                int32_t res = E502_UsbGetDevRecordsList(&rec_list[fnd_devcnt], usb_devcnt, 0, NULL);
                if (res >= 0) {
                    fnd_devcnt += res;
                }
            }

            /* создаем записи для переданного массива ip-адресов */
            for (i=0; i < ip_cnt; i++) {
                if (E502_MakeDevRecordByIpAddr(&rec_list[fnd_devcnt], ip_addr_list[i],0, TCP_CONNECTION_TOUT) == X502_ERR_OK) {
                    fnd_devcnt++;
                }
            }
        }
    }

    if (fnd_devcnt != 0) {
        /* если создана хотя бы одна запись, то сохраняем указатель на выделенный массив */
        *pdevrec_list = rec_list;
    } else {
        *pdevrec_list = NULL;
        free(rec_list);
    }

    return fnd_devcnt;
}


static t_x502_hnd f_dev_select_open(int argc, char** argv) {
    t_x502_hnd hnd = NULL;
    uint32_t fnd_devcnt,i, dev_ind;
    t_x502_devrec *devrec_list = NULL;
    uint32_t *ip_addr_list = NULL;
    uint32_t ip_cnt = 0;

    /* если есть аргументы командной строки, то предполагаем, что это могут быть
       ip-адреса интересующих устройств. */
    if (argc > 1) {
        ip_addr_list = malloc((argc-1) * sizeof(ip_addr_list[0]));
        if (ip_addr_list == NULL) {
            fprintf(stderr, "Ошибка выделения памяти!\n");
        } else {
            for (i=1; (int)i < argc; i++) {
                int a[4];
                if (sscanf(argv[i], "%d.%d.%d.%d", &a[0], &a[1], &a[2], &a[3])==4) {
                    ip_addr_list[ip_cnt++] = ((a[0] & 0xFF) << 24) |
                                             ((a[1] & 0xFF) << 16) |
                                             ((a[2] & 0xFF) <<  8) |
                                             (a[3] & 0xFF);
                }
            }
        }
    }

    /* получаем список модулей для выбора */
    fnd_devcnt = f_get_all_devrec(&devrec_list, ip_addr_list, ip_cnt);

    if (fnd_devcnt == 0) {
        printf("Не найдено ни одного модуля\n");
    } else {
        /* выводим информацию по списку модулей */
        printf("Доступны следующие модули:\n");
        for (i=0; i < fnd_devcnt; i++) {
            printf("Модуль № %d: %s, %-9s", i, devrec_list[i].devname,
                   devrec_list[i].iface == X502_IFACE_PCI ? "PCI/PCIe" :
                   devrec_list[i].iface == X502_IFACE_USB ? "USB" :
                   devrec_list[i].iface == X502_IFACE_ETH ? "Ethernet" : "?");

            /* при подключении по сети по IP-адресу серийный номер можно узнать
               только после открытия соединения. При этом поле location
               содержит строку с описанием адреса устройства */
            if (devrec_list[i].iface != X502_IFACE_ETH) {
                printf("Сер. номер: %s\n", devrec_list[i].serial);
            } else {
                printf("Адрес: %s\n", devrec_list[i].location);
            }
        }

        /* выбираем нужный по введенному номеру модуля по порядку с клавиатуры */
        printf("Введите номер модуля, с которым хотите работать (от 0 до %d)\n", fnd_devcnt-1);
        fflush(stdout);
        //scanf("%d", &dev_ind);
        dev_ind = 0;

        if (dev_ind >= fnd_devcnt) {
            printf("Неверно указан номер модуля...\n");
        } else {
            /* если ввели номер правильно - создаем описатель */
            hnd = X502_Create();
            if (hnd==NULL) {
                fprintf(stderr, "Ошибка создания описателя модуля!");
            } else {
                /* устанавливаем связь с модулем по записи */
                int32_t err = X502_OpenByDevRecord(hnd, &devrec_list[dev_ind]);
                if (err != X502_ERR_OK) {
                    fprintf(stderr, "Ошибка установления связи с модулем: %s!", X502_GetErrorString(err));
                    X502_Free(hnd);
                    hnd = NULL;
                }
            }
        }

        /* освобождение ресурсов действительных записей из списка */
        X502_FreeDevRecordList(devrec_list, fnd_devcnt);
        /* очистка памяти самого массива */
        free(devrec_list);
    }

    /* освобождаем выделенный массив под IP-адреса (если был выделен) */
    free(ip_addr_list);

    return hnd;
}


/* настройка параметров модуля */
int32_t f_setup_params(t_x502_hnd hnd) {
    int32_t err = X502_ERR_OK, i;

    /* устанавливаем параметры логической таблицы АЦП */
    err = X502_SetLChannelCount(hnd, ADC_LCH_CNT);
    for (i=0; (i < ADC_LCH_CNT) && (err == X502_ERR_OK); i++)
        err = X502_SetLChannel(hnd, i, f_channels[i], f_ch_modes[i], f_ch_ranges[i], 0);

    /* устанавливаем частоты ввода для АЦП и цифровых входов */
    if (err == X502_ERR_OK) {
        double f_adc = ADC_FREQ, f_frame = ADC_FRAME_FREQ, f_din = DIN_FREQ;
        err = X502_SetAdcFreq(hnd, &f_adc, &f_frame);
        if (err == X502_ERR_OK)
            err = X502_SetDinFreq(hnd, &f_din);
        if (err == X502_ERR_OK) {
            /* выводим реально установленные значения - те что вернули функции */
            printf("Установлены частоты:\n    Частота сбора АЦП = %0.0f\n"
                "    Частота на лог. канал = %0.0f\n    Частота цифрового ввода = %0.0f\n",
                f_adc, f_frame, f_din);
        }
    }

    if (err == X502_ERR_OK) {
        err = X502_BfLoadFirmware(hnd, "/home/user/PRJ/lpcie_sdk/firmware/l502-bf/build/release/bin/l502-bf.ldr");
    }

    /* записываем настройки в модуль */
    if (err == X502_ERR_OK)
        err = X502_Configure(hnd, 0);

    /* разрешаем синхронные потоки */
    if (err == X502_ERR_OK) {
        err = X502_StreamsEnable(hnd, X502_STREAM_DIN);
    }

    return err;
}

int main(int argc, char** argv) {
    int32_t err = X502_ERR_OK;
    uint32_t ver;
    t_x502_hnd hnd = NULL;
#ifndef _WIN32
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
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
    hnd = f_dev_select_open(argc, argv);

    /********************************** Работа с модулем **************************/
    /* если успешно выбрали модуль и установили с ним связь - продолжаем работу */
    if (hnd != NULL) {
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
                fprintf(stderr, "Ошибка настройки модуля: %s!", X502_GetErrorString(err));
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
                int32_t rcv_size;
                uint32_t adc_size, din_size;

                /* массив для приема необработанных данных */
                static uint32_t rcv_buf[READ_BLOCK_SIZE];
                static double   adc_data[READ_BLOCK_SIZE];
                static uint32_t din_data[READ_BLOCK_SIZE];

                /* принимаем данные (по таймауту) */
                rcv_size = X502_Recv(hnd, rcv_buf, READ_BLOCK_SIZE, READ_TIMEOUT);
                /* результат меньше нуля означает ошибку */
                if (rcv_size < 0) {
                    err = rcv_size;
                    fprintf(stderr, "Ошибка приема данных: %s\n", X502_GetErrorString(err));
                } else if (rcv_size > 0) {
                    uint32_t first_lch;
                    /* получаем номер логического канала, которому соответствует первый отсчет АЦП в массиве */
                    X502_GetNextExpectedLchNum(hnd, &first_lch);

                    adc_size = sizeof(adc_data)/sizeof(adc_data[0]);
                    din_size = sizeof(din_data)/sizeof(din_data[0]);

                    /* обрабатываем принятые данные, распределяя их на данные АЦП и цифровых входов */
                    err = X502_ProcessData(hnd, rcv_buf, rcv_size, X502_PROC_FLAGS_VOLT,
                                           adc_data, &adc_size, din_data, &din_size);
                    if (err != X502_ERR_OK) {
                        fprintf(stderr, "Ошибка обработки данных: %s\n", X502_GetErrorString(err));
                    } else {
                        uint32_t lch;

                        printf("Блок %3d. Обработано данных АЦП =%d, цифровых входов =%d\n",
                               block, adc_size, din_size);
                        /* если приняли цифровые данные - выводим первый отсчет */
                        if (din_size != 0)
                            printf("    din_data = 0x%05X\n", din_data[0]);

                        /* выводим по одному отсчету на канал. если обработанный блок
                           начинается не с начала кадра, то в данном примере для
                           вывода берем конец неполного кадра и начало следующего */
                        for (lch=0; lch < ADC_LCH_CNT; lch++) {
                            /* определяем позицию первого отсчета, соответствующего заданному логическому каналу:
                               либо с конца не полного кадра, либо из начала следующего */
                            uint32_t pos = lch >= first_lch ? lch - first_lch : ADC_LCH_CNT-first_lch + lch;
                            if (pos <= adc_size) {
                                printf("    lch[%d]=%6.4f\n", lch, adc_data[pos]);
                            } else {
                                printf("    lch[%d]= ---- \n", lch);
                            }
                        }
                        printf("\n");
                        fflush(stdout);
                    }
                }
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
