@echo off
cls

SET MY_INPUT=%1
IF NOT DEFINED MY_INPUT SET MY_INPUT=ALL

IF /I %MY_INPUT% EQU ALL GOTO clean
IF /I %MY_INPUT% EQU classes GOTO clean
IF /I %MY_INPUT% EQU exec GOTO exec
IF /I %MY_INPUT% EQU tests GOTO tests
IF /I %MY_INPUT% EQU ? GOTO help

GOTO end

:help

echo * * * make_new.bat file * * *
echo.
echo Options:
echo.
echo   clean    - Cleans the *.exe, *.obj, *.manifest files
echo   classes  - Cleans and compiles the C++ class files only
echo   exec     - Compiles and links the consoleUpload file
echo   tests    - Compiles and links the test* files only
echo   ?        - Shows this help file
echo   ALL      - Cleans, compiles C++ classes, and makes consoleUpload
echo
echo Note: If a second command is passed, it will be passed as
echo   an argument to the 'cl' calls. For example, to set the DEBUG
echo   pre-compiler directive, pass /DDEBUG as the second argument.

GOTO end;

:clean

echo * * * Deleting objects and executables ...
del *.exe
del *.obj
del *.manifest

IF /I %MY_INPUT% EQU CLEAN GOTO end

:classes

echo * * * Compiling classes

cl /c /EHsc /O2 /nologo %2 XMLDocument.cpp XMLNode.cpp XMLNodeList.cpp

cl /c /EHsc /O2 /nologo %2 ExperimentCollection.cpp Experiment.cpp Tank.cpp Block.cpp Module.cpp

cl /c /EHsc /O2 /nologo %2 SingleUnitDir.cpp 

cl /c /EHsc /O2 /nologo %2 MySQLpp.cpp

cl /c /EHsc /O2 /nologo %2 PooledSort.cpp SpikeList.cpp Timer.cpp

cl /c /EHsc /O2 /nologo %2 TTank.cpp

IF /I %MY_INPUT% EQU classes GOTO end

:exec

echo * * * Compiling consoleUpload class
cl /EHsc /O2 /nologo %2 consoleUpload.cpp XMLDocument.obj XMLNode.obj XMLNodeList.obj ExperimentCollection.obj Experiment.obj Tank.obj Block.obj Module.obj SingleUnitDir.obj MySQLpp.obj TTank.obj PooledSort.obj SpikeList.obj Timer.obj /link  libmysql.lib msxml2.lib ole32.lib oleaut32.lib comsuppw.lib libmat.lib libmx.lib 

GOTO end

:tests

echo * * * Compiling test classes
REM testDomDocument needs XML libraries
cl /EHsc /O2 /nologo testDomDocument.cpp XMLDocument.obj XMLNode.obj XMLNodeList.obj /link msxml2.lib ole32.lib oleaut32.lib comsuppw.lib

REM testEC needs XML, Matlab, MySQL libraries
cl /EHsc /O2 /nologo testEC.cpp ExperimentCollection.obj Experiment.obj Tank.obj Block.obj Module.obj XMLDocument.obj XMLNode.obj XMLNodeList.obj SingleUnitDir.obj PooledSort.obj SpikeList.obj MySQLpp.obj TTank.obj Timer.obj /link msxml2.lib ole32.lib oleaut32.lib comsuppw.lib libmat.lib libmx.lib libmysql.lib

REM testMySQLpp needs MySQL libraries
cl /EHsc /O2 /nologo testMySQLpp.cpp MySQLpp.obj /link libmysql.lib

REM testPooledSort needs Matlab libraries
cl /EHsc /O2 /nologo testPooledSort.cpp PooledSort.obj /link libmat.lib libmx.lib 

REM testSingleUnitDir needs no libraries
cl /EHsc /O2 /nologo testSingleUnitDir.cpp SingleUnitDir.obj

REM testSpikeList needs Matlab libraries
cl /EHsc /O2 /nologo testSpikeList.cpp SpikeList.obj PooledSort.obj /link libmat.lib libmx.lib  

REM testTTank needs no libraries
cl /EHsc /O2 /nologo testTTank.cpp TTank.obj

:end