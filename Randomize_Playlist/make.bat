@echo off
del *.obj
del *.exe
cl /nologo /O2 randomizePlaylist.cpp
pause
