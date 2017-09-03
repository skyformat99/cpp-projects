@echo off

SET MY_INPUT=%1
IF NOT DEFINED MY_INPUT SET MY_INPUT=ALL

IF /I %MY_INPUT% EQU ALL GOTO clean
IF /I %MY_INPUT% EQU CLEAN GOTO clean
IF /I %MY_INPUT% EQU classes GOTO classes
IF /I %MY_INPUT% EQU files GOTO files
IF /I %MY_INPUT% EQU execs GOTO execs

GOTO end

:clean

echo * * * Deleting objects and executables ...
del *.exe
del *.obj
del *.manifest

IF /I %MY_INPUT% EQU CLEAN GOTO end

:classes

echo * * * Compiling ...
cl /nologo /c /O2 /clr /I"C:\usr\cpp\Single_Unit" ByteBuffer.cpp Socket.cpp ClientSocket.cpp ServerSocket.cpp TankCollection.cpp ClientFarm.cpp VectorUtilities.cpp

cl /nologo /c /O2 /EHsc /I"C:\usr\cpp\Single_Unit" /I"C:\usr\cpp\Single_Unit\ClientServer" ../PooledSort.cpp ../NumRecipe.cpp ../SpikeList.cpp ../TTank.cpp

:files

cl /nologo /O2 /EHsc /I"C:\usr\cpp\Single_Unit" testServer.cpp ByteBuffer.obj Socket.obj ClientSocket.obj ServerSocket.obj TankCollection.obj ClientFarm.obj PooledSort.obj SpikeList.obj NumRecipe.obj VectorUtilities.obj /link /NODEFAULTLIB:LIBCMT Ws2_32.lib Mswsock.lib Advapi32.lib libmat.lib libmx.lib

REM cl /nologo /O2 /EHsc /I"C:\usr\cpp\Single_Unit" testClient.cpp ByteBuffer.obj Socket.obj ClientSocket.obj ServerSocket.obj TankCollection.obj ClientFarm.obj PooledSort.obj SpikeList.obj NumRecipe.obj VectorUtilities.obj /link /NODEFAULTLIB:LIBCMT Ws2_32.lib Mswsock.lib Advapi32.lib libmat.lib libmx.lib

cl /nologo /O2 /EHsc /I"C:\usr\cpp\Single_Unit" testClientFarm.cpp ByteBuffer.obj Socket.obj ClientSocket.obj ServerSocket.obj TankCollection.obj ClientFarm.obj PooledSort.obj SpikeList.obj NumRecipe.obj VectorUtilities.obj TTank.obj /link /NODEFAULTLIB:LIBCMT Ws2_32.lib Mswsock.lib Advapi32.lib libmat.lib libmx.lib 

:execs

cl /nologo /O2 /EHsc /I"C:\usr\cpp\Single_Unit" findTankPath.cpp ByteBuffer.obj Socket.obj ClientSocket.obj ServerSocket.obj TankCollection.obj ClientFarm.obj PooledSort.obj SpikeList.obj NumRecipe.obj VectorUtilities.obj TTank.obj /link /NODEFAULTLIB:LIBCMT Ws2_32.lib Mswsock.lib Advapi32.lib libmat.lib libmx.lib 

cl /nologo /O2 /EHsc /I"C:\usr\cpp\Single_Unit" pingServer.cpp ByteBuffer.obj Socket.obj ClientSocket.obj ServerSocket.obj TankCollection.obj ClientFarm.obj PooledSort.obj SpikeList.obj NumRecipe.obj VectorUtilities.obj TTank.obj /link /NODEFAULTLIB:LIBCMT Ws2_32.lib Mswsock.lib Advapi32.lib libmat.lib libmx.lib 

cl /nologo /O2 /EHsc /I"C:\usr\cpp\Single_Unit" findAllTanks.cpp ByteBuffer.obj Socket.obj ClientSocket.obj ServerSocket.obj TankCollection.obj ClientFarm.obj PooledSort.obj SpikeList.obj NumRecipe.obj VectorUtilities.obj TTank.obj /link /NODEFAULTLIB:LIBCMT Ws2_32.lib Mswsock.lib Advapi32.lib libmat.lib libmx.lib 

cl /nologo /O2 /EHsc /I"C:\usr\cpp\Single_Unit" verifySingleUnitData.cpp ByteBuffer.obj Socket.obj ClientSocket.obj ServerSocket.obj TankCollection.obj ClientFarm.obj PooledSort.obj SpikeList.obj NumRecipe.obj VectorUtilities.obj TTank.obj /link /NODEFAULTLIB:LIBCMT Ws2_32.lib Mswsock.lib Advapi32.lib libmat.lib libmx.lib 

:end
