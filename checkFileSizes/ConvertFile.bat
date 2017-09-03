@ECHO OFF
SETLOCAL

:: Calls the converter if necessary, to convert to .dvr-ms
IF EXIST "%~dpn1 - DVRMS.dvr-ms" ( 
  echo %~dpn1 - DVRMS.dvr-ms Exists 
) ELSE ( 
  C:\Windows\eHome\WTVConverter %1 "%~dpn1 - DVRMS.dvr-ms" /ShowUI 
)

:: Gets the file size of the old .wtv file
FOR %%I in (%1) DO SET OLDFS=%%~zI
REM ECHO WTV file size: %OLDFS%

:: Gets the file size of the new .dvr-ms file
FOR %%I in ("%~dpn1 - DVRMS.dvr-ms") DO SET NEWFS=%%~zI
REM ECHO DVR-MS file size: %NEWFS%

:: Checks the size and writes the result to the RES variable
compareSizes %OLDFS% %NEWFS% %2 RES > temp.bat
CALL temp.bat
IF EXIST "temp.bat" DEL "temp.bat"

:: If sizes are within tolerance, can delete .wtv file
IF %RES%==true (
  DEL %1
) ELSE (
  echo Do not delete file %1
)