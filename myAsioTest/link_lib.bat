@echo off
del *.dll
link /NOLOGO /DLL %1 JAsioWave.obj AsioWave.obj AsioDebugger.obj asio.obj asiodrivers.obj asiolist.obj combase.obj ole32.lib Advapi32.lib User32.lib /OUT:"asioWave.dll"
