@echo off

REM call "C:\Program Files\Microsoft Visual Studio 10.0\VC\bin\vcvars32.bat"

del *.exe
del *.obj

cl %1 /nologo /EHsc GeoElem.cpp Style.cpp StyleGroup.cpp Coords.cpp State.cpp FileCSV.cpp engine.cpp /Fe"engine.exe"

REM cl %1 /nologo /EHsc Style.cpp StyleGroup.cpp GeoElem.cpp Coords.cpp State.cpp FileCSV.cpp /Fe"engine.exe"