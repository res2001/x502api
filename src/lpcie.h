/***************************************************************************//**
    @file lpcie.h
    Файл содержит определения типов, констант и функций, которые предполагаются
    быть одинаковыми для работы со всеми модулями, поддерживаемыми драйвером
    lpcie.

    @warning Эти определения не являются законченными и могут измениться
    в будущих версиях библиотеки. Для работы с плато L502 рекомендуется
    использовать только функции, определенные в l502api.h

    @date 23.05.2012
    @author Borisov Alexey <borisov@lcard.ru>
*******************************************************************************/

#ifndef LPCIE_H_
#define LPCIE_H_


#include "stddef.h"

#ifdef _WIN32
    #include "Windows.h"

    #define LPCIE_EXPORT(type) type APIENTRY
#else
    #define LPCIE_EXPORT(type) __attribute__ ((visibility("default"))) type
#endif

#if defined(__STDC__)
# define PREDEF_STANDARD_C_1989
# if defined(__STDC_VERSION__)
#  define PREDEF_STANDARD_C_1990
#  if (__STDC_VERSION__ >= 199409L)
#   define PREDEF_STANDARD_C_1994
#  endif
#  if (__STDC_VERSION__ >= 199901L)
#   define PREDEF_STANDARD_C_1999
#  endif
# endif
#endif


#ifndef STDINT_DEFINED
    /* включаем стандартный файл stdint.h для компиляторов, поддерживающих C99,
       для GCC и для MSVC начиная с версии 16.0 (Visual Studio 10.0) */
    #if defined  (PREDEF_STANDARD_C_1999) || defined   (  __GNUC__  ) \
        || (defined (_MSC_VER) && (_MSC_VER >= 1600)) \
        || (defined (__BORLANDC__) && (__BORLANDC__ >= 0x560))
        #include "stdint.h"
    #else
        #include "pstdint.h"
    #endif
#endif




#ifdef __cplusplus
extern "C" {
#endif

/** Максимальный размер строки с названием устройства */
#define LPCIE_DEVNAME_SIZE     32
/** Максимальный размер серийного номера */
#define LPCIE_SERIAL_SIZE      32

/** Флаги, описывающие модуль */
typedef enum {
    /** Признак наличия ЦАП */
    LPCIE_DEVINFO_FLAGS_DAC_PRESENT = 0x0001,
    /** Признак наличия гальваноразвязки */
    LPCIE_DEVINFO_FLAGS_GAL_PRESENT = 0x0002,
    /** Признак наличия сигнального процессора */
    LPCIE_DEVINFO_FLAGS_BF_PRESENT  = 0x0004,
    /** Признак, что устройство уже открыто */
    LPCIE_DEVINFO_FLAGS_DEV_OPENED  = 0x0100
} lpcie_devinfo_flags;

/** Флаги, которые можно передать в LPCIE_GetDevInfoList() */
typedef enum {
    /** Признак, что нужно искать только устройства, которые еще не открыты */
    LPCIE_GETDEVS_FLAGS_ONLY_NOT_OPENED = 0x1
} t_lpcie_getdevs_flags;


/** Структура, описывающая не открытое устройство, по которой с ним можно
    установить связь */
typedef struct {
    char devname[LPCIE_DEVNAME_SIZE]; /** название устройства */
    char serial[LPCIE_SERIAL_SIZE]; /** серийный номер */
    char res[124]; /** резерв */
    uint32_t flags; /** флаги из #lpcie_devinfo_flags, описывающие устройство */
    void* internal; /** непрозрачный указатель на структуру с платформозависимой
                      информацией об устройстве. Выделяется библиотекой при
                      вызове LPCIE_GetDevInfoList() и должна быть освобождена
                      с помощью LPCIE_FreeDevInfoList() */
} t_lpcie_devinfo;



/***************************************************************************//**
    @brief Получение спискаподключенных устройств

    Функция находит все подключенные устройства в системе, поддерживаемые
    драйвером lpcie и для каждого найденного устройства заполняет структуру
    из переданного массива (если в нем есть место) значениями, описывающими
    это устройство. При этом так же выделяется память для хранения платформо-
    зависимой информации для возможности последующего открытия устройства.
    Эта память должна быть после очищена с помощью LPCIE_FreeDevInfoList().
    @param[out] list     Массив структур, которые должны быть заполнены информацией
                         о подключенных устройствах. Может быть NULL, если size=0,
                         а devcnt!=NULL, в случае, если нужно получить количество
                         устройств.
    @param[in] size      Количество структур в массиве list. Функция заполнит
                         максимум size структур в list.
    @param[in] flags     Флаги из #t_lpcie_getdevs_flags.
    @param[out] devcnt   Если не NULL, то в этой переменной возвращается, сколько
                         всего найдено устройств - может быть больше size
    @return              Если меньше нуля - код ошибки,
                         если >= 0, количество заполненных структур в list
                         (равно min(size, devcnt))
 ******************************************************************************/
LPCIE_EXPORT(int32_t) LPCIE_GetDevInfoList(t_lpcie_devinfo* list, uint32_t size, uint32_t flags,
                             uint32_t* devcnt);

/***************************************************************************//**
    @brief Освобождение памяти, выделенной LPCIE_GetDevInfoList().

    Для каждого элемента массива с информацией о устройстве, заполненного
    до этого с помощью LPCIE_GetDevInfoList(), данная функция осовобождает
    память с платформо-зависимой информацией, указатель на которую был
    сохранен в поле intptr каждого элемента массива.

    После этого данная структура уже не может быть использована для открытия
    устройства.

    Функция должна вызываться всякий раз, когда полученная с помощью
    LPCIE_GetDevInfoList() информация больше не требуется.

    @note Если сам массив list выделялся динамически, то его нужно освободить
    вручную.

    @param[in] list   Массив структур с информацией о подключенных устройствах
    @param[in] size   Количество элементов в массиве
    @return           Код ошибки
    ***************************************************************************/
LPCIE_EXPORT(int32_t) LPCIE_FreeDevInfoList(t_lpcie_devinfo *list, uint32_t size);



#ifdef __cplusplus
}
#endif


#endif
