@echo off
del JAsioWave.jar
jar -cvf JAsioWave.jar JAsioWave.class asioWave.dll
del asioWave.dll
