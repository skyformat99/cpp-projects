@echo off

FOR /D %%i IN (*.*) DO batch_rename "%%i"

REM SET userinput=

REM Set some env variables
REM SET dvdxml_dl="C:/Documents and Settings/Adam/My Documents/My Music/gd_dl/download.exe"
REM SET dvdxml_lk=http://www.dvdxml.com/download/download.php?xml_id=
REM SET dvdxml_lk="http://www.dvdxml.com/download/download.php?username=ltkafee&userpass=hear4me!&xml_id="

:start

REM SET /P userinput="> "

REM IF %userinput% == . (
REM   GOTO dl
REM ) 

REM timestamp 20000 "set dvdxml_ts=" >> temp_ts.bat
REM CALL temp_ts.bat

REM echo %dvdxml_dl% "http://www.dvdxml.com/download/download.php?username=ltkafee&userpass=hear4me!&xml_id=%userinput%&tok=%dvdxml_ts%" /overwrite /post  >> temp.bat

REM GOTO start

:dl

REM echo DO SOMETHING!

REM IF EXIST temp.bat DEL temp.bat
REM IF EXIST temp_ts.bat DEL temp_ts.bat

:clean

REM SET userinput=
REM SET dvdxml_dl=
REM SET dvdxml_lk=
REM SET dvdxml_ts=