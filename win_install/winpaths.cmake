
# пути к внешним библиотекам по-умолчанию для ОС Windows


if(NOT PROGRAM_DIR)
    set(PROGRAM_DIR "c:/Program Files")
endif(NOT PROGRAM_DIR)

if(NOT PROGRAM_DIR32)
    set(PROGRAM_DIR32 "c:/Program Files (x86)")
endif(NOT PROGRAM_DIR32)

if(NOT LBOOT_EXEC)
    set(LBOOT_EXEC "c:/PRJ/builds/lboot_pc/lboot.exe")
endif(NOT LBOOT_EXEC)

if(NOT FFTW_INCLUDE_DIRS)
    set(FFTW_INCLUDE_DIRS "${PROGRAM_DIR32}/libfftw3")
endif(NOT FFTW_INCLUDE_DIRS)

if(NOT FFTW_LIBRARIES_DIRS)
    set(FFTW_LIBRARIES_DIRS "${PROGRAM_DIR32}/libfftw3")
endif(NOT FFTW_LIBRARIES_DIRS)

if(NOT FFTW_BINARY)
    set(FFTW_BINARY "${PROGRAM_DIR32}/libfftw3/libfftw3-3.dll")
endif(NOT FFTW_BINARY)

if(NOT MGL_INCLUDE_DIRS)
    set(MGL_INCLUDE_DIRS "${PROGRAM_DIR32}/MathGL/include")
endif(NOT MGL_INCLUDE_DIRS)

if(NOT MGL_BINARY)
set(MGL_BINARY "${PROGRAM_DIR32}/MathGL/bin/mgl.dll")
endif(NOT MGL_BINARY)

if(NOT MGL_LIBRARIES_DIRS)
    set(MGL_LIBRARIES_DIRS "${PROGRAM_DIR32}/MathGL/lib")
endif(NOT MGL_LIBRARIES_DIRS)

if(NOT PNG_LIBRARY)
    set(PNG_LIBRARY "${PROGRAM_DIR32}/libpng/lib/libpng16.lib")
endif(NOT PNG_LIBRARY)

if(NOT PNG_BINARY)
    set(PNG_BINARY "${PROGRAM_DIR32}/libpng/bin/libpng16.dll")
endif(NOT PNG_BINARY)

if(NOT ZLIB_BINARY)
    set(ZLIB_BINARY "${PROGRAM_DIR32}/zlib/bin/zlib.dll")
endif(NOT ZLIB_BINARY)

if(NOT QXT_CORE_BINARY)
    set(QXT_CORE_BINARY "c:/Qxt/bin/QxtCore.dll")
endif(NOT QXT_CORE_BINARY)

if(NOT NSIS_EXEC)
    set(NSIS_EXEC "${PROGRAM_DIR32}/NSIS/makensis")
endif(NOT NSIS_EXEC)

if(NOT MSVC_RT_REDISTR_DIR)
    set(MSVC_RT_REDISTR_DIR "${PROGRAM_DIR32}/Microsoft Visual Studio 9.0/VC/redist/x86/Microsoft.VC90.CRT")
endif(NOT MSVC_RT_REDISTR_DIR)

if(NOT PC_LIBXML_INCLUDE_DIRS)
    set(PC_LIBXML_INCLUDE_DIRS "${PROGRAM_DIR32}/libxml2/include")
endif(NOT PC_LIBXML_INCLUDE_DIRS)

if(NOT PC_LIBXML_LIBRARY_DIRS)
    set(PC_LIBXML_LIBRARY_DIRS "${PROGRAM_DIR32}/libxml2/lib")
endif(NOT PC_LIBXML_LIBRARY_DIRS)

if(NOT LIBXML_BINARY)
    set(LIBXML_BINARY "${PROGRAM_DIR32}/libxml2/bin/libxml2.dll")
endif(NOT LIBXML_BINARY)

if(NOT LIBXML_ZLIB_BINARY)
    set(LIBXML_ZLIB_BINARY "${PROGRAM_DIR32}/libxml2/bin/zlib1.dll")
endif(NOT LIBXML_ZLIB_BINARY)

if(NOT LIBXML_ICONV_BINARY)
    set(LIBXML_ICONV_BINARY "${PROGRAM_DIR32}/libxml2/bin/iconv.dll")
endif(NOT LIBXML_ICONV_BINARY)


if(NOT MC)
    set(MC "${PROGRAM_DIR}/Microsoft SDKs/Windows/v7.1/Bin/MC.Exe")
endif(NOT MC)

if(NOT VS32_SETENV)
    set(VS32_SETENV "${PROGRAM_DIR32}/Microsoft Visual Studio 9.0/VC/vcvarsall.bat")
endif(NOT VS32_SETENV)

if(NOT VS64_SETENV)
    set(VS64_SETENV "${PROGRAM_DIR32}/Microsoft Visual Studio 11.0/VC/vcvarsall.bat")
endif(NOT VS64_SETENV)

if(NOT WIN_SDK_DIR)
    set(WIN_SDK_DIR "${PROGRAM_DIR}/Microsoft SDKs/Windows/v7.1")
endif(NOT WIN_SDK_DIR)

set(WIN_SDK_SETENV "${WIN_SDK_DIR}/bin/setenv")

if(NOT NET_FRAMEVORKv2.0_DIR)
    set(NET_FRAMEVORKv2.0_DIR c:/Windows/Microsoft.NET/Framework/v2.0.50727)
endif(NOT NET_FRAMEVORKv2.0_DIR)

if(NOT NET_FRAMEVORKv3.5_DIR)
    set(NET_FRAMEVORKv3.5_DIR c:/Windows/Microsoft.NET/Framework/v3.5)
endif(NOT NET_FRAMEVORKv3.5_DIR)

if(NOT NET_FRAMEVORKv4.0_DIR)
    set(NET_FRAMEVORKv4.0_DIR c:/Windows/Microsoft.NET/Framework/v4.0.30319)
endif(NOT NET_FRAMEVORKv4.0_DIR)

if(NOT CYGWIN_PATH)
    set(CYGWIN_PATH "c:/cygwin")
endif(NOT CYGWIN_PATH)

if(NOT ICONV)
    set(ICONV  "${CYGWIN_PATH}/bin/iconv")
endif(NOT ICONV)

if(NOT SED)
    set(SED  "${CYGWIN_PATH}/bin/sed")
endif(NOT SED)

if(NOT GENDEF)
    set(GENDEF "${CYGWIN_PATH}/bin/gendef.exe")
endif(NOT GENDEF)

if(NOT DLLTOOL64)
    set(DLLTOOL64 "${CYGWIN_PATH}/bin/x86_64-w64-mingw32-dlltool.exe")
endif(NOT DLLTOOL64)

if(NOT DLLTOOL32)
    set(DLLTOOL32 "${CYGWIN_PATH}/bin/i686-w64-mingw32-dlltool.exe")
endif(NOT DLLTOOL32)






if(NOT EMBARCADERO_BIN_DIR)
    set(EMBARCADERO_BIN_DIR "${PROGRAM_DIR32}/Embarcadero/Studio/14.0/bin")
endif(NOT EMBARCADERO_BIN_DIR)

if(NOT BORLAND_IMPLIB)
    set(BORLAND_IMPLIB "${EMBARCADERO_BIN_DIR}/implib.exe")
endif(NOT BORLAND_IMPLIB)

if(NOT BORLAND_MKEXP)
    set(BORLAND_MKEXP "${EMBARCADERO_BIN_DIR}/mkexp.exe")
endif(NOT BORLAND_MKEXP)



if(NOT DDK_DIR)
    set(DDK_DIR c:/WinDDK/7600.16385.1)
endif(NOT DDK_DIR)

if(NOT SIGNTOOL)
    set(SIGNTOOL ${DDK_DIR}/bin/amd64/SignTool.exe)
endif(NOT SIGNTOOL)

if(NOT SIGN_CERT)
    set(SIGN_CERT c:/PRJ/MSCV-VSClass3.cer)
endif(NOT SIGN_CERT)

if(NOT SIGN_COMPANY_NAME)
    set(SIGN_COMPANY_NAME "L Card Ltd")
endif(NOT SIGN_COMPANY_NAME)

if(NOT DOTNET_SIGN_KEY)
    set(DOTNET_SIGN_KEY c:/PRJ/lcardNet.snk)
endif(NOT DOTNET_SIGN_KEY)

if(NOT CERT_TIMESTAMP_URL)
    set(CERT_TIMESTAMP_URL http://timestamp.verisign.com/scripts/timstamp.dll)
endif(NOT CERT_TIMESTAMP_URL)
