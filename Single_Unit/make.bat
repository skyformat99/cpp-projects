@echo off

echo * * * Deleting objects and executables ...
del *.exe
del *.obj

echo * * * Compiling C++ Classes ...
cl /nologo /c /EHsc /O2 /I".\ClientServer" /I"." %1 %2 %3 %4 NumRecipe.cpp PooledSort.cpp SpikeList.cpp Timer.cpp MySQLpp.cpp TTank.cpp ClientServer\ByteBuffer.cpp ClientServer\Socket.cpp ClientServer\ClientSocket.cpp ClientServer\ClientFarm.cpp ClientServer\VectorUtilities.cpp

echo * * * Compiling lone classes ...
cl /nologo /O2 /EHsc /I".\ClientServer" testPooledSort.cpp PooledSort.obj SpikeList.obj ByteBuffer.obj /link libmat.lib libmx.lib Ws2_32.lib Mswsock.lib Advapi32.lib 

cl /nologo /O2 /EHsc /I".\ClientServer" poolPrint.cpp PooledSort.obj SpikeList.obj ByteBuffer.obj /link libmat.lib libmx.lib Ws2_32.lib Mswsock.lib Advapi32.lib 

cl /nologo /O2 /EHsc /I".\ClientServer" testSpikeList.cpp PooledSort.obj SpikeList.obj ByteBuffer.obj /link libmat.lib libmx.lib Ws2_32.lib Mswsock.lib Advapi32.lib 

REM Not quite sure what testPrintf actually did ...
REM cl /nologo /O2 /EHsc testPrintf.cpp Timer.obj PooledSort.obj SpikeList.obj ByteBuffer.obj /link libmat.lib libmx.lib Ws2_32.lib Mswsock.lib Advapi32.lib 

REM This was the compilation necessary when we were importing ClientServer components
REM cl /nologo /O2 /DWIN32 /EHsc /I".\ClientServer" performUpload.cpp NumRecipe.obj PooledSort.obj SpikeList.obj MySQLpp.obj Timer.obj ByteBuffer.obj ClientFarm.obj Socket.obj ClientSocket.obj VectorUtilities.obj /link libmat.lib libmx.lib libmysql.lib Ws2_32.lib Mswsock.lib Advapi32.lib 

cl /nologo /O2 /DWIN32 /I".\ClientServer" /EHsc performUpload.cpp PooledSort.obj SpikeList.obj MySQLpp.obj Timer.obj ByteBuffer.obj /link libmat.lib libmx.lib libmysql.lib Ws2_32.lib Mswsock.lib Advapi32.lib 

REM cl /nologo /O2 /DWIN32 verifyDatabase.cpp NumRecipe.obj PooledSort.obj SpikeList.obj Timer.obj ByteBuffer.obj /link libmat.lib libmx.lib libmysql.lib Ws2_32.lib Mswsock.lib Advapi32.lib 

del *.obj