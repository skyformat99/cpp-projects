@echo off
cls

SET MY_INPUT=%1
IF NOT DEFINED MY_INPUT SET MY_INPUT=1

del *.exp
del *.lib
del *.obj

REM /D"MYTXTDEBUG"
cl /c /D"MYDEBUG"=%MY_INPUT% /EHsc /nologo /I"C:/usr/cpp/asiosdk2/common" /I"c:/usr/cpp/asiosdk2/host"  /I"c:/usr/cpp/asiosdk2/host/pc" /I"C:/Program Files (x86)/Java/jdk1.6.0_22/include" /I"C:/Program Files (x86)/Java/jdk1.6.0_22/include/win32" "C:\usr\cpp\ASIOSDK2\host\asiodrivers.cpp" "C:\usr\cpp\ASIOSDK2\host\pc\asiolist.cpp" "C:\usr\cpp\ASIOSDK2\common\asio.cpp" "C:\usr\cpp\ASIOSDK2\common\combase.cpp" AsioWave.cpp testAsioWave.cpp JAsioWave.cpp AsioDebugger.cpp