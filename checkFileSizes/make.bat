@echo off
del *.exe
del *.obj
cl /nologo /EHsc %1 checkFileSizes.cpp
cl /nologo compareSizes.cpp