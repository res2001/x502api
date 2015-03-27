# makefile для сборки примера с помощью компиляторов mingw (под Windows)
# или GCC (под Linux). Необходимо определить 3 переменные:
#
# CC                      - имя команды для вызова компилятора
# X502API_LIBRARIES_DIR   - путь к файлу .a или .so библиотек x502api, l502api, e502api (если не стандартный)
# X502API_INCLUDE_DIR     - путь к заголовочным файлам x502api (x502api.h, l502api.h, e502api.h, lcard_pstding.h)
#
# Ниже приведено несколько примеров в закоментированном виде

#--- Linux с заголовками и библиотекой в стандартных директориях: компилятор GCC
#CC = gcc

#--- Вариант запуска из MSYS со стандартным 32-битным mingw 
#CC = gcc
#X502API_LIBRARIES_DIR = "/c/Program Files/L-Card/lpcie/lib/mingw"
#X502API_INCLUDE_DIR = "/c/Program Files/L-Card/lpcie/include"


#---  64-битный вариант mingw w64, идущий вместе с cygwin  --------
#CC = x86_64-w64-mingw32-gcc
#X502API_LIBRARIES_DIR = "/cygdrive/c/Program Files (x86)/L-Card/lpcie/lib/mingw64"
#X502API_INCLUDE_DIR = "/cygdrive/c/Program Files (x86)/L-Card/lpcie/include"

#---  32-битный вариант mingw w64, идущий вместе с cygwin  --------
#CC = i686-w64-mingw32-gcc
#X502API_LIBRARIES_DIR = "/cygdrive/c/Program Files (x86)/L-Card/lpcie/lib/mingw"
#X502API_INCLUDE_DIR = "/cygdrive/c/Program Files (x86)/L-Card/lpcie/include"

#---  32-битный вариант mingw, идущий вместе с cygwin  --------
#CC = i686-pc-mingw32-gcc
#X502API_LIBRARIES_DIR = "/cygdrive/c/Program Files (x86)/L-Card/lpcie/lib/mingw"
#X502API_INCLUDE_DIR = "/cygdrive/c/Program Files (x86)/L-Card/lpcie/include"


FLAGS =

ifdef X502API_LIBRARIES_DIR
	FLAGS += -L $(X502API_LIBRARIES_DIR)
endif

ifdef X502API_INCLUDE_DIR
	FLAGS += -I $(X502API_INCLUDE_DIR)
endif



all:
	$(CC) main.c $(FLAGS) -lx502api -ll502api -le502api -o x502_stream_read
