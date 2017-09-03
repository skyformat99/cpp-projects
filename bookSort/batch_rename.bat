@echo off

IF EXIST temp.bat DEL temp.bat

IF /I %1 EQU "AAA" GOTO end

dir_rename %1 "SET new_dir=\"" \" >> temp.bat
call temp.bat
del temp.bat

rename %1 %new_dir%
move %new_dir% "AAA\"

SET new_dir=

:end

REM my_rename %1 "SET new_dir=" >> temp.bat
REM call temp.bat
REM del temp.bat

REM IF NOT EXIST "%new_dir%" mkdir "%new_dir%"

REM move %1 "%new_dir%\"

REM SET new_dir=
