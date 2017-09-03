@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\bin\vcvars32.bat"
del *.obj
del readIFO.exe
REM cl /c /D"LDRIVE" /D"DEBUG" /D"VERBOSE" ripDVD.cpp VideoUnit.cpp /EHsc /nologo %1
REM cl /D"LDRIVE" /D"DEBUG" /D"VERBOSE" readIFO.cpp /EHsc /nologo %1 /link ripDVD.obj VideoUnit.obj
cl /c /D"DDRIVE" ripDVD.cpp VideoUnit.cpp /EHsc /nologo %1
cl /D"DDRIVE" readIFO.cpp /EHsc /nologo %1 /link ripDVD.obj VideoUnit.obj
del *.obj
