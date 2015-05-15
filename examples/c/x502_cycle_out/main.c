/* Данный пример демонстрирует возможность установки циклического сигнала на 
   вывод в модуль L502 (ввод не используется вообще).

   В примере определена таблица сигналов, которая задает несколько
   комбинаций сигналов для примера. Каждый сигнал задается количеством точек
   на период (общее значение для всех трех сигналов), амплитудой для ЦАП'ов и
   функцией для генерации очередного отсчета по его номеру и полному размеру.

   Пользовтель может ввести номер сигнала, которых он хочет выставить и нажать Enter - и этот
   сигнал будет выставлен на выходе. 
   
   Смена сигнала происходит по концу периода предыдущего.
   Надо также помнить, что хотя мы можем загружать новый сигнал на фоне вывода
   предыдущего, однако сразу после выполнения L502_OutCycleSetup() до того
   времени как реально произойдет смена старого сигнала на новый нельзя
   начинать загружать еще один новый сигнал.


   Данный пример содержит проект для Visual Studio 2008, а также может быть собран
   gcc в Linux или mingw в Windows через makefile или с помощью cmake (подробнее
   в коментариях в соответствующих файлах).

   Для того чтобы собрать проект в Visual Studio, измените путь к заголовочным файлам
   (Проект (Project) -> Свойства (Properties) -> Свойства конфигурации (Configuration Properties)
   -> С/С++ -> Общие (General) -> Дополнительные каталоги включения (Additional Include Directories))
   на тот, где у вас лежат заголовочный файл l502api.h и измените путь к библиотекам
   (Проект (Project) -> Свойства (Properties) -> Свойства конфигурации (Configuration Properties) ->
   Компановщик (Linker) -> Общие (General) -> Дополнительные катологи библиотек (Additional Library Directories)).

   Внимание!!: Если Вы собираете проект под Visual Studio и взяли проект с сайта (а не из SDK),
   то для корректного отображения русских букв в программе нужно изменить кодировку
   или указать сохранение с сигнатурой кодировки для UTF-8:
   выберите Файл (File) -> Доболнительные параметры сохранения (Advanced Save Options)...
   и в поле Кодировка (Encoding) выберите Юникод (UTF8, с сигнатурой)/Unicode (UTF-8 with signature)
   и сохраните изменения в файле.
   */

#include "l502api.h"
#include "e502api.h"
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define TCP_CONNECTION_TOUT 5000
#define OUT_SIGNAL_SIZE           2000
#define OUT_BLOCK_SIZE            256
#define SEND_TOUT                 500

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

typedef double (*f_gen_sig_word)(uint32_t cntr, uint32_t total_size, double amp);
typedef uint32_t (*f_gen_dout_word)(uint32_t cntr, uint32_t total_size);
/* структура, задающая сигналы на 2-х каналах ЦАП и/или1
1на DOUT */
typedef struct {
    uint32_t size; /* количество точек в сигнале */
    double amp_dac1; /* амплитуда сигнала для ЦАП1 */
    f_gen_sig_word gen_func_dac1; /* функция для генерации отсчета для ЦАП1 */
    double amp_dac2; /* амплитуда сигнала для ЦАП2 */
    f_gen_sig_word gen_func_dac2; /* функция для генерации отсчета для ЦАП2 */
    f_gen_dout_word gen_dout; /* функция для генерации слова на цифровой вывод */
} t_sig_struct;


/* генерация пилы на весь период */
static double f_gen_saw(uint32_t cntr, uint32_t total_size, double amp) {
    return amp*( (int32_t)(cntr%total_size)-(int32_t)total_size/2)/(total_size/2);
}

/* генерация синуса на весь период */
static double f_gen_sin(uint32_t cntr, uint32_t total_size, double amp) {
    return amp*sin(2*M_PI*cntr/total_size);
}

/* генерация синуса с периодом в два раза меньше периоду определяемому размером буфера */
static double f_gen_sin2(uint32_t cntr, uint32_t total_size, double amp) {
    return amp*sin(2*2*M_PI*cntr/total_size);
}

/* генерация меандра на всех выходах путем выдачи в первой половине на выходы 0xAA, а на второй 0x55 */
static uint32_t f_gen_dout_meander(uint32_t cntr, uint32_t total_size) {
    return cntr < total_size/2 ? 0xAA : 0x55;
}

/* генерация счетчика на цифроых выхоах*/
static uint32_t f_gen_dout_cntr(uint32_t cntr, uint32_t total_size) {
    return cntr;
}


/* таблица, задающая сигналы для выдачи на вход */
static t_sig_struct f_sig_tbl[] = {
    {100000, X502_DAC_RANGE, f_gen_saw, 0, NULL, NULL}, /* пила на одном канале с частотой 10 Гц */
    {2000, X502_DAC_RANGE, f_gen_sin, 0, NULL, NULL}, /* синусоидальный сигнал на одном канале с частотой 500 Гц */
    {100, X502_DAC_RANGE/2, f_gen_sin, X502_DAC_RANGE, f_gen_saw, NULL}, /* синус ампл. 2.5 и пила по 10 КГц */
    {50, 0, NULL, X502_DAC_RANGE, f_gen_sin, NULL}, /* синус только на втором канале с частотой 20 КГц */
    {2550, 1.5, f_gen_sin, 2.5, f_gen_sin2, f_gen_dout_meander}, /* на втором канале синус с частотой в 2 раза больше. меандр на цифровых линиях */
};


/* Запись в буфер драйвера блока данных от сигнала 
   cntr - номер отчета, соответствующего первому отсчету блока
   size - количество отсчетов на канал (т.е. записывается ch_cnt*size отсчетов)
   sig  - номер сигнала
   ch_cnt - кол-во используемых каналов (это кол-во можно определить по f_sig_tbl[sig],
            но так как мы его уже определили, то передаем сюда, чтобы опять не определять */
static int32_t f_load_block(t_x502_hnd hnd, uint32_t cntr, uint32_t size, uint32_t sig, uint32_t ch_cnt) {
    static double dac_data1[OUT_BLOCK_SIZE], dac_data2[OUT_BLOCK_SIZE];
    static uint32_t dout_data[OUT_BLOCK_SIZE];
    /* массив слов на запись в модуль - содержит смешенные подготовленные данные
       для всех каналов (максимум для 3-х - 2 ЦАП + DOUT) */
    static uint32_t sbuf[3*OUT_BLOCK_SIZE];
    uint32_t i;
    int32_t err = 0;

    /* заполняем массив на вывод */
    for (i=0; i < size; i++) {
        if (f_sig_tbl[sig].gen_func_dac1 != NULL) {
            dac_data1[i] = f_sig_tbl[sig].gen_func_dac1(cntr+i, f_sig_tbl[sig].size, f_sig_tbl[sig].amp_dac1);
        }
        if (f_sig_tbl[sig].gen_func_dac2 != NULL) {
            dac_data2[i] = f_sig_tbl[sig].gen_func_dac2(cntr+i, f_sig_tbl[sig].size, f_sig_tbl[sig].amp_dac2);
        }
        if (f_sig_tbl[sig].gen_dout != NULL) {
            dout_data[i] = f_sig_tbl[sig].gen_dout(cntr+i, f_sig_tbl[sig].size);
        }
    }

    /* Если нужная функция определена, значит мы испоьлзуем этот канал, и
     * подаем на вход сформированный массив. Иначе - канал не используется
     * и передаем на вход NULL */
    err = X502_PrepareData(hnd,
                           f_sig_tbl[sig].gen_func_dac1 ? dac_data1 : NULL,
                           f_sig_tbl[sig].gen_func_dac2 ? dac_data2 : NULL,
                           f_sig_tbl[sig].gen_dout ? dout_data : NULL,
                           size, X502_DAC_FLAGS_VOLT | X502_DAC_FLAGS_CALIBR,
                           sbuf);
    if (err != X502_ERR_OK) {
        fprintf(stderr, "Ошибка подкотовки данных на передачу: %s\n",
                X502_GetErrorString(err));
    } else {
        /* посылаем данные */
        int32_t snd_cnt = X502_Send(hnd, sbuf, size*ch_cnt, SEND_TOUT);
        if (snd_cnt < 0) {
            err = snd_cnt;
            fprintf(stderr, "Ошибка передачи данных\n", X502_GetErrorString(err));
        } else if ((uint32_t)snd_cnt != size*ch_cnt) {
            /* так как мы шлем всегда не больше чем готово, то должны
               всегда передать все */
            fprintf(stderr, "Переданно недостаточно данных: передавали = %d, передано = %d\n",
                    size*ch_cnt, snd_cnt);
            err = X502_ERR_SEND_INSUFFICIENT_WORDS;
        }
    }
    return err;
}

static int32_t f_load_cycle_signal(t_x502_hnd hnd, int sig) {
    int32_t err = 0;
    uint32_t cntr = 0;
    uint32_t ch_cnt=0;

    /* определяем, сколько каналов используем */
    if (f_sig_tbl[sig].gen_func_dac1)
        ch_cnt++;
    if (f_sig_tbl[sig].gen_func_dac2)
        ch_cnt++;
    if (f_sig_tbl[sig].gen_dout)
        ch_cnt++;


    /* задаем размер буфера под все каналы! */
    err = X502_OutCycleLoadStart(hnd, f_sig_tbl[sig].size*ch_cnt);
    if (err != X502_ERR_OK)
        fprintf(stderr, "Ошибка старта загрузки данных: %s!", X502_GetErrorString(err));

    /* в примере показано, что загружать можно поблочно, чтобы не выделять
     * большой размер памяти на весь сигнал в программе. Но можно записать
     * весь сигнал и за один L502_Send() */
    while ((cntr != f_sig_tbl[sig].size) && (err == X502_ERR_OK)) {
        uint32_t block_size = OUT_BLOCK_SIZE;
        /* последний блок может быть меньшего размера, если размер буфера не кратен
         * блоку */
        if (block_size >(f_sig_tbl[sig].size-cntr))
            block_size=f_sig_tbl[sig].size-cntr;

        err = f_load_block(hnd, cntr, block_size, sig, ch_cnt);
        if (!err)
            cntr+=block_size;
    }

    /* делаем активным загруженный сигнал */
    if (err == X502_ERR_OK) {
       err = X502_OutCycleSetup(hnd,0);
       if (err != X502_ERR_OK)
            fprintf(stderr, "Ошибка установки циклического сигнала: %s!", X502_GetErrorString(err));
    }
    return err;
}


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

    if ((pci_devcnt+usb_devcnt + ip_cnt) != 0) {
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
        scanf("%d", &dev_ind);

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

int main(int argc, char **argv) {
    int32_t err = 0;
    uint32_t ver;
    t_x502_hnd hnd = NULL;

#if _WIN32
    /* устанавливаем локаль, чтобы можно было выводить по-русски в CP1251 без перевода в OEM */
    setlocale(LC_CTYPE, "");
#endif
    /* получаем версию библиотеки */
    ver = X502_GetLibraryVersion();
    printf("Верисия библиотеки: %d.%d.%d\n", (ver >> 24)&0xFF, (ver>>16)&0xFF, (ver>>8)&0xFF);

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
            /* Разрешаем все каналы на вывод (в этом примере мы не будем
               использовать асинхронный вывод), а какие каналы реально будут
               использованы определяем когда подаем данные в L502_PrepareData */
            err = X502_StreamsEnable(hnd, X502_STREAM_ALL_OUT);
            if (err != X502_ERR_OK) {
                fprintf(stderr, "Ошибка разрешения потоков (%d): %s!", err,
                        X502_GetErrorString(err));
            }
        }

        if (err == X502_ERR_OK) {
            /* если хотим, то можем загрузить синхнал до разрешения
             * синхронного ввода. Тогда мы могли бы его запустить, наприемер,
               всесте с АЦП по L502_StreamsStart() */
            err = f_load_cycle_signal(hnd, 0);

            /* разрешаем синхронный ввод. В принципе можно его разрешить только
             * после загрузки первого синала в нашем примере. Но данный пример
             * показывает что мы можем сделать это и до выставления сигнала,
             * например если захотим запустить ввод АЦП до того как будет нужно
             * подать циклический сигнал на вывод */
            if (err == X502_ERR_OK)
                err = X502_StreamsStart(hnd);

            if (err == X502_ERR_OK) {
                int exit = 0;

                printf("Введите одно из следующего:\n");
                printf("  число от 1 до %d - установить сигнал с указанным номером\n",
                       sizeof(f_sig_tbl)/sizeof(f_sig_tbl[0]));
                printf("  s или stop       - останов генерации сигнала\n");
                printf("  e или exit       - выход из программы\n");

                /* Цикл ввода команд */
                while (!err && !exit) {
                    char cmd[512];
                    printf(">");
                    fflush(stdout);
                    scanf("%s", cmd);
                    if (!strcmp(cmd, "exit") || !strcmp(cmd,"e")) {
                        exit=1;
                    } else if (!strcmp(cmd, "stop")||!strcmp(cmd,"s")) {
                        err = X502_OutCycleStop(hnd,0);
                        if (err != X502_ERR_OK) {
                            fprintf(stderr, "Ошибка останова циклического сигнала (%d): %s\n",
                                    err, X502_GetErrorString(err));
                        }
                    } else {
                        int sig = atoi(cmd);
                        if ((sig<=0) || (sig>sizeof(f_sig_tbl)/sizeof(f_sig_tbl[0]))) {
                            fprintf(stderr, "Неверный номер сигнала или неизвестная команда\n");
                        } else {
                            err = f_load_cycle_signal(hnd, sig-1);
                        }
                    }
                }

                X502_StreamsStop(hnd);
            }            
        }

        /* закрываем связь с модулем */
        X502_Close(hnd);
        /* освобождаем описатель */
        X502_Free(hnd);
    }
    return err;
}
  

