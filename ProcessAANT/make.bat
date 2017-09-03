@echo off
del *.exe
del *.obj

cl /nologo /EHsc ToolboxData.cpp processAANT.cpp /Fe"processAANT.exe"