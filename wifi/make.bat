@echo off

echo * * * Deleting objects and executables ...
del test.exe
del *.obj

echo * * * Compiling C++ Classes ...
cl /nologo /c /EHsc /O2 WAP_Pool.cpp WAP.cpp Fingerprint.cpp GridPoint.cpp Grid.cpp

cl /nologo /c /O2 /EHsc http_test.cpp

cl /nologo /O2 /EHsc test.cpp WAP_Pool.obj WAP.obj

cl /nologo /O2 /EHsc locate.cpp Grid.obj GridPoint.obj Fingerprint.obj http_test.obj

del *.obj