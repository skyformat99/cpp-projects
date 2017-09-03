@echo off

del repRB.obj
del repRB.exe

cl /nologo %1 %2 /EHsc repRB.cpp
