/* Данный пример представляет из себя консольную программу на языке C,
   демонстрирующую, как можно выполнить поиск устройств E502 в локальной сети.

   Пример запускается и непрерывно отслеживает появление (или исчезновение)
   устройств в локальной сети, выводя информацию на консоль.

   Поиск выполняется до нажатия любой клавиши на Windows или  CTRL+C на Linux.

   Данный пример содержит проект для Visual Studio 2008, а также может быть собран
   gcc в Linux или mingw в Windows через makefile или с помощью cmake (подробнее
   в комментариях в соответствующих файлах).

   Для того чтобы собрать проект в Visual Studio, измените путь к заголовочным файлам
   (Проект (Project) -> Свойства (Properties) -> Свойства конфигурации (Configuration Properties)
   -> С/С++ -> Общие (General) -> Дополнительные каталоги включения (Additional Include Directories))
   на тот, где у вас лежат заголовочный файлы x502api.h и e502api.h и измените путь к библиотекам
   (Проект (Project) -> Свойства (Properties) -> Свойства конфигурации (Configuration Properties) ->
   Компановщик (Linker) -> Общие (General) -> Дополнительные катологи библиотек (Additional Library Directories)).

   Внимание!!: Если Вы собираете проект под Visual Studio и взяли проект с сайта (а не из SDK),
   то для корректного отображения русских букв в программе нужно изменить кодировку
   или указать сохранение с сигнатурой кодировки для UTF-8:
   выберите Файл (File) -> Дополнительные параметры сохранения (Advanced Save Options)...
   и в поле Кодировка (Encoding) выберите Юникод (UTF8, с сигнатурой)/Unicode (UTF-8 with signature)
   и сохраните изменения в файле.

   */

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


#ifdef _WIN32
    #define INST_NAME_PRINT_FMT "%ls"
#else
    #define INST_NAME_PRINT_FMT "%s"
#endif

/* признак необходимости завершить сбор данных */
static int f_out = 0;

#ifndef _WIN32
/* Обработчик сигнала завершения для Linux */
static void f_abort_handler(int sig) {
    f_out = 1;
}
#endif






int main(int argc, char** argv) {
    int32_t err = X502_ERR_OK;
    int32_t stop_err;
    uint32_t ver;
    t_e502_eth_svc_browse_hnd browse_hnd;
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


    /* Начинаем процесс поиска устройств в локальной сети */
    err = E502_EthSvcBrowseStart(&browse_hnd, 0);
    if (err != X502_ERR_OK) {
        fprintf(stderr, "Ошибка запуска поиска устройств в сети (%d): %s\n",
                err, X502_GetErrorString(err));
    } else {
        printf("Запущен поиск устройств в локальной сети. Для останова нажмите %s\n",
#ifdef _WIN32
               "любую клавишу"
#else
               "CTRL+C"
#endif
               );
        while (!f_out && (err == X502_ERR_OK)) {
            t_e502_eth_svc_record_hnd svc_rec;
            uint32_t event;
            /* пробуем получить запись, сообщающую о изменении информации о подулюенных
             * устройтсвах */
            err = E502_EthSvcBrowseGetEvent(browse_hnd, &svc_rec, &event, NULL, 300);
            if (err != X502_ERR_OK) {
                fprintf(stderr, "Ошибка получения записи о найденном устройстве (%d): %s\n",
                        err, X502_GetErrorString(err));
            } else if (event != E502_ETH_SVC_EVENT_NONE) {
                char inst_name[X502_INSTANCE_NAME_SIZE];
                char dev_serial[X502_SERIAL_SIZE];
                uint32_t addr;
                int32_t cur_err;
#ifdef _WIN32
                WCHAR inst_name_w[X502_INSTANCE_NAME_SIZE];
#endif

                /* значение события, отличное от E502_ETH_SVC_EVENT_NONE, говорит
                 * о том, что изменение произошло. Для каждого события мы можем
                 * получить имя экземпляра и сер. номер устройства, для которого
                 * произошло событие */
                cur_err = E502_EthSvcRecordGetInstanceName(svc_rec, inst_name);
                if (cur_err == X502_ERR_OK)
                    cur_err = E502_EthSvcRecordGetDevSerial(svc_rec, dev_serial);
#ifdef _WIN32
                if (cur_err == X502_ERR_OK) {
                    /* Имя устройства всегда в кодировке UTF-8. В Windows для
                     * представления юникодных строк используется формат UTF16
                     * и строка представляется в виде массива WCHAR.
                     * Для перевода используем функцию преобразования */
                    if (MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, inst_name,
                                        -1, inst_name_w, X502_INSTANCE_NAME_SIZE)==0) {
                        cur_err = X502_ERR_INSTANCE_NAME_ENCODING;
                    }
                }
#endif

                if (cur_err != X502_ERR_OK) {
                    fprintf(stderr, "Ошибка получения параметров устройства (%d): %s\n",
                            cur_err, X502_GetErrorString(cur_err));
                } else {
                    if ((event == E502_ETH_SVC_EVENT_ADD) ||
                            (event == E502_ETH_SVC_EVENT_CHANGED)) {
                        /* События ADD и CHANGE означают, что устройство присутствует,
                         * и мы можем получить IP-адрес устройства */
                        cur_err = E502_EthSvcRecordResolveIPv4Addr(svc_rec, &addr, 4000);

                        if (cur_err == X502_ERR_OK) {
                            printf("%s: " INST_NAME_PRINT_FMT ", S/N: %s, Адрес = %d.%d.%d.%d\n",
                                   event == E502_ETH_SVC_EVENT_ADD ? "Новое устройтсво" : "Изм. параметров",
#ifdef _WIN32
                                   inst_name_w,
#else
                                   inst_name,
#endif
                                   dev_serial,
                                   (addr >> 24) & 0xFF, (addr >> 16) & 0xFF,
                                   (addr >>  8) & 0xFF, addr & 0xFF
                                   );

                            fflush(stdout);
                        } else {
                            fprintf(stderr, "Ошибка получения адреса устройства (%d): %s\n",
                                    cur_err, X502_GetErrorString(cur_err));
                        }
                    } else if (event == E502_ETH_SVC_EVENT_REMOVE) {
                        printf("Устройство удалено: " INST_NAME_PRINT_FMT ", S/N: %s\n",
#ifdef _WIN32
                                inst_name_w,
#else
                                inst_name,
#endif
                                dev_serial);
                    }
                }

                /* Для всех событий, кроме NONE, выделяется запись, которую
                 * нужно освободить вручную */
                E502_EthSvcRecordFree(svc_rec);
            }

#ifdef _WIN32
            /* проверка нажатия клавиши для выхода */
            if (err == X502_ERR_OK) {
                if (_kbhit())
                    f_out = 1;
            }
#endif
        }
        stop_err = E502_EthSvcBrowseStop(browse_hnd);
        if (stop_err != X502_ERR_OK) {
            fprintf(stderr, "Ошибка останова поиска сервисов (%d): %s\n",
                    stop_err, X502_GetErrorString(stop_err));
            if (err == X502_ERR_OK)
                err = stop_err;
        } else {
            printf("Останов поиска сервисов выполнен успешно!\n");
        }
    }

    return err;
}
