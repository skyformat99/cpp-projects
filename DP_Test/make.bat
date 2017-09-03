@echo off

del *.obj
del *.exe
del *.tlh
del *.tli

cl /nologo /EHsc /c ProcessQueue.cpp TestElement.cpp Transducers.cpp

cl /nologo /EHsc multithread2.cpp /link ProcessQueue.obj TestElement.obj 

cl /nologo /EHsc rco_test.cpp /link Transducers.obj