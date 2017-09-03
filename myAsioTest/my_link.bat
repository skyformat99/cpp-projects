@echo off
del *.exe
link /NOLOGO %1 testAsioWave.obj AsioWave.obj AsioDebugger.obj asio.obj asiodrivers.obj asiolist.obj combase.obj ole32.lib Advapi32.lib User32.lib /OUT:"testAsioWave.exe"
