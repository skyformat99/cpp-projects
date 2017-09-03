@echo off

REM The full file-name comes in as %1 to this batch file
REM We need to ensure that '-' is in the filename (otherwise print error)
REM If it is, get the part before and set to the artist
REM Then get the part after and set to name
REM Set any other defaults (album, track #) and return

SETLOCAL

stringOp -f %1 "-" "POS" > temp.bat
CALL temp.bat
IF EXIST temp.bat DEL temp.bat

IF %POS% == "-1" (
  ECHO ERROR :: %1 :: NO DASH
  GOTO end
)

REM echo Position: %POS%

REM echo Name: "%~nx1"

stringOp -F "%~nx1" "-" "ARTIST" > temp.bat
CALL temp.bat
IF EXIST temp.bat DEL temp.bat

REM echo Artist: %ARTIST%

stringOp -B "%~nx1" "-" "TITLE" > temp.bat
CALL temp.bat
IF EXIST temp.bat DEL temp.bat

REM echo Title: %TITLE%

echo mp3info -t %TITLE% -a %ARTIST% -n "01" -l "" -c "Single File" %1  >> todo.txt

:end