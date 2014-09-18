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

#ifndef LSDK_H_
#define LSDK_H_


#include "stddef.h"

#ifdef _WIN32
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include "Windows.h"

    #define X502_EXPORT(type) type APIENTRY
#else
    #ifndef APIENTRY
        #define APIENTRY
    #endif

    #define X502_EXPORT(type) __attribute__ ((visibility("default"))) type
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


#endif
