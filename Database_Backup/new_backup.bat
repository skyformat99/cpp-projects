:: - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
:: Back Up the UberWalton dB System
::
::   Things to keep current:
::      - In this file, line 35, ensure "BACKUP_LOC" is set 
::        to the current backup location, and "LOG_LOC" is set
::        properly.
::
::      - Set the number of desired backups to keep as the final
::        parameter to the backupUtil.exe call on line 46 (Default: 5)
::
::      - In the databases.txt file, ensure each dB to be backed
::        up is written on a separate line (with no trailing newline)
::
::      - Verify that the WinZip directory is correct (line 65), and that 
::        the command line extensions for winzip (i.e. "wzzip.exe") are 
::        installed. NB: in Win7 we can use the built-in "zip" function 
::        as an appropriate substitute (ACD 9/2/09).
::
:: (c) ACD 31 Jan 08
:: - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

@ECHO OFF

:: Sets the log file variable
..\Date_Format\date_format.exe "set LOGFILE=log_%%Y_%%m_%%d.txt" > temp.bat
CALL temp.bat
IF EXIST "temp.bat" DEL "temp.bat"

:: And start writing our log
ECHO -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- >> %LOGFILE%
CALL date /t                                               >> %LOGFILE%
CALL time /t                                               >> %LOGFILE%
ECHO.                                                      >> %LOGFILE%

:: Set backup location - NO TRAILING SLASHES for either
set BACKUP_LOC=C:\dB_backups
set LOG_LOC=C:\dB_logs
ECHO Backup Location: %BACKUP_LOC%                         >> %LOGFILE%
ECHO Log Location: %LOG_LOC%                               >> %LOGFILE%

:: Generate list of files in the backup location
dir /ON /B %BACKUP_LOC%\db*.zip > temp.txt

:: Run the backup utility to remove old files
ECHO.                                                      >> %LOGFILE%
CALL time /t                                               >> %LOGFILE%
.\Backup_Util\backupUtil.exe .\temp.txt %BACKUP_LOC% 15    >> %LOGFILE%
IF EXIST "temp.txt" DEL "temp.txt"

:: Now dump out the mysql databases into a .txt file per database
ECHO.                                                      >> %LOGFILE%
CALL time /t                                               >> %LOGFILE%
ECHO Dumping databases ...                                 >> %LOGFILE%
FOR /F %%i IN (databases.txt) DO mysqldump -u joe --password=joepass --routines --quick --no-create-info --result-file=backup_%%i.sql %%i 

:: Sets the zip file variable
..\Date_Format\date_format.exe "set ZIPFILE=db_%%Y_%%m_%%d.zip" > temp.bat
CALL temp.bat
IF EXIST "temp.bat" DEL "temp.bat"

:: Now zip up all created .sql files ...
ECHO.                                                      >> %LOGFILE%
CALL time /t                                               >> %LOGFILE%
ECHO Zipping up all files ...                              >> %LOGFILE%
ECHO Zip File: %ZIPFILE%                                   >> %LOGFILE%
zip %ZIPFILE% *.sql                                        >> %LOGFILE%

IF NOT ErrorLevel 1 GOTO PastZip
ECHO **** Error Detected With WZZIP *****                  >> %LOGFILE%
GOTO CleanUp

:PastZip

:: Delete all .sql files
ECHO.                                                      >> %LOGFILE%
CALL time /t                                               >> %LOGFILE%
ECHO Deleting all .sql files ...                           >> %LOGFILE%
DEL *.sql

:: Copy new .zip file over to the backup directory
ECHO.                                                      >> %LOGFILE%
CALL time /t                                               >> %LOGFILE%
ECHO Copying and verifying ...                             >> %LOGFILE%
copy %ZIPFILE% %BACKUP_LOC%\%ZIPFILE% >NUL

:: Check the MD5 sum on the old and new file
.\md5sums -u %ZIPFILE% > cmp1.txt
.\md5sums -u %BACKUP_LOC%\%ZIPFILE% > cmp2.txt

:: Echo out the MD5 sums
ECHO MD5 Local:                                            >> %LOGFILE%
TYPE cmp1.txt                                              >> %LOGFILE%
ECHO MD5 Copied:                                           >> %LOGFILE%
TYPE cmp2.txt                                              >> %LOGFILE%

FC cmp1.txt cmp2.txt | FIND "FC: no differences encountered" >NUL
SET RESULT=%ERRORLEVEL%

IF %RESULT% == 0 ECHO Copy verification successful!        >> %LOGFILE%
IF %RESULT% == 1 ECHO Copy verification unsuccessful ***!  >> %LOGFILE%

IF EXIST "cmp1.txt" DEL "cmp1.txt"
IF EXIST "cmp2.txt" DEL "cmp2.txt"

:: Delete the original .zip file
DEL %ZIPFILE%

:CleanUp

:: Copy the log file to it's new home
ECHO.                                                      >> %LOGFILE%
CALL time /t                                               >> %LOGFILE%
ECHO Copying log file, deleting original ...               >> %LOGFILE%
copy %LOGFILE% %LOG_LOC%\%LOGFILE% >NUL
DEL %LOGFILE%

:: Reset the variables used
SET RESULT=
SET ZIPFILE=
SET BACKUP_LOC=
SET LOG_LOC=
SET LOGFILE=