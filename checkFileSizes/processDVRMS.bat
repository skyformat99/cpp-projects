@echo off

REM File to process ...
IF NOT EXIST "%~dp1CommFree\%~n1 - NoC.dvr-ms" (
  "C:\Program Files (x86)\DVRMSToolbox\DVRMStoMPEG" /if="%~dpn1.dvr-ms" /of="%~dp1CommFree\%~n1 - NoC.dvr-ms" /act=dvrmscutoutcommercials
) ELSE (
  echo Skipping "%~n1" - exists!
)
REM echo Processing %~dp1CommFree\%~n1 - NoC.dvr-ms ...

REM Options for DVRMStoMPEG:
REM
REM converttompgwocommercials  - Converts to an MPEG
REM dvrmscutoutcommercials     - Cuts commercials

REM "C:\Program Files (x86)\DVRMSToolbox\DVRMStoMPEG" /if="%~dpn1.dvr-ms" /of="%~dp1CommFree\%~n1 - NoC.dvr-ms" /act=dvrmscutoutcommercials

REM FFMPEG Pass 1
REM "C:\Program Files (x86)\DVRMSToolbox\Applications\ffmpeg\ffmpeg_MPGMUX.exe" -i "%~dpn1 - NoC.dvr-ms" -vcodec mpeg2video -s 720x480 -b 6291456 -ab 192000 -ar 48000 -ac 2 -pass 1 -passlogfile "%~dpn1 - NoC" "%~dpn1.mpg" 

REM del "%~dpn1.mpg"

REM FFMPEG Pass 2
REM "C:\Program Files (x86)\DVRMSToolbox\Applications\ffmpeg\ffmpeg_MPGMUX.exe" -i "%~dpn1 - NoC.dvr-ms" -vcodec mpeg2video -s 720x480 -b 6291456 -ab 192000 -ar 48000 -ac 2 -pass 2 -passlogfile "%~dpn1 - NoC" "%~dpn1.mpg" 

REM del "%~dpn1 - NoC*.log"
REM del "%~dpn1 - NoC.dvr-ms"
REM del "%~dpn1*.edl"
