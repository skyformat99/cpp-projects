@echo off
cls

SET MY_INPUT=%1
IF NOT DEFINED MY_INPUT SET /P MY_INPUT="Enter State Code: "
IF NOT DEFINED MY_INPUT GOTO error

IF /I %MY_INPUT% EQU CO GOTO CO
IF /I %MY_INPUT% EQU CT GOTO CT
IF /I %MY_INPUT% EQU MA GOTO MA
IF /I %MY_INPUT% EQU MD GOTO MD
IF /I %MY_INPUT% EQU MI GOTO MI
IF /I %MY_INPUT% EQU MO GOTO MO
IF /I %MY_INPUT% EQU NC GOTO NC
IF /I %MY_INPUT% EQU OH GOTO OH
IF /I %MY_INPUT% EQU PA GOTO PA
IF /I %MY_INPUT% EQU TN GOTO TN
IF /I %MY_INPUT% EQU UT GOTO UT

REM * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
:error
REM * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

ECHO Error ... %1 not a valid state code!
GOTO :EOF

REM * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
:MA
REM * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

SET ST="MA"
IF EXIST %ST%*.kml (del %ST%*.kml)

ECHO %ST% Running %% Caucasian ...
IF EXIST temp.txt (del temp.txt)
echo 4         >  temp.txt
echo 0.4       >> temp.txt
echo 1         >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Percent Caucasian.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running %% AA ...
IF EXIST temp.txt (del temp.txt)
echo 6         >  temp.txt
echo 0         >> temp.txt
echo 0.3       >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Percent AA.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running %% Short Commute ...
IF EXIST temp.txt (del temp.txt)
echo 7         >  temp.txt
echo .1        >> temp.txt
echo .7        >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Percent Short Commute.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running %% with Doctorate ...
IF EXIST temp.txt (del temp.txt)
echo 8         >  temp.txt
echo 0         >> temp.txt
echo .2        >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Percent Doctorate.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running Median Household Income ...
IF EXIST temp.txt (del temp.txt)
echo 9         >  temp.txt
echo 20000     >> temp.txt
echo 150000    >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Med House Income.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running %% Receiving SSI ...
IF EXIST temp.txt (del temp.txt)
echo 11        >  temp.txt
echo 0         >> temp.txt
echo .15       >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Percent SSI.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running Median Year Built ...
IF EXIST temp.txt (del temp.txt)
echo 13        >  temp.txt
echo 1935      >> temp.txt
echo 1990      >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Med Yr Built.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running Median Home Value ...
IF EXIST temp.txt (del temp.txt)
echo 15        >  temp.txt
echo 300000    >> temp.txt
echo 1000000   >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Med Home Value.kml" ff003300 ffffff00 < temp.txt

GOTO :EOF

REM * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
:PA
REM * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

SET ST="PA"
IF EXIST %ST%*.kml (del %ST%*.kml)

ECHO %ST% Running %% Caucasian ...
IF EXIST temp.txt (del temp.txt)
echo 4         >  temp.txt
echo 0.4       >> temp.txt
echo 1         >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Percent Caucasian.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running %% AA ...
IF EXIST temp.txt (del temp.txt)
echo 6         >  temp.txt
echo 0         >> temp.txt
echo 0.3       >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Percent AA.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running %% Short Commute ...
IF EXIST temp.txt (del temp.txt)
echo 7         >  temp.txt
echo .1        >> temp.txt
echo .7        >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Percent Short Commute.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running %% with Doctorate ...
IF EXIST temp.txt (del temp.txt)
echo 8         >  temp.txt
echo 0         >> temp.txt
echo .1        >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Percent Doctorate.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running Median Household Income ...
IF EXIST temp.txt (del temp.txt)
echo 9         >  temp.txt
echo 20000     >> temp.txt
echo 150000    >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Med House Income.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running %% Receiving SSI ...
IF EXIST temp.txt (del temp.txt)
echo 11        >  temp.txt
echo 0         >> temp.txt
echo .15       >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Percent SSI.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running Median Year Built ...
IF EXIST temp.txt (del temp.txt)
echo 13        >  temp.txt
echo 1935      >> temp.txt
echo 1990      >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Med Yr Built.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running Median Home Value ...
IF EXIST temp.txt (del temp.txt)
echo 15        >  temp.txt
echo 200000    >> temp.txt
echo 800000   >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Med Home Value.kml" ff003300 ffffff00 < temp.txt

IF EXIST temp.txt (del temp.txt)
GOTO :EOF

REM * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
:OH
REM * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

SET ST="OH"
IF EXIST %ST%*.kml (del %ST%*.kml)

ECHO %ST% Running %% Caucasian ...
IF EXIST temp.txt (del temp.txt)
echo 4         >  temp.txt
echo 0.4       >> temp.txt
echo 1         >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Percent Caucasian.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running %% AA ...
IF EXIST temp.txt (del temp.txt)
echo 6         >  temp.txt
echo 0         >> temp.txt
echo 0.3       >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Percent AA.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running %% Short Commute ...
IF EXIST temp.txt (del temp.txt)
echo 7         >  temp.txt
echo .1        >> temp.txt
echo .7        >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Percent Short Commute.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running %% with Doctorate ...
IF EXIST temp.txt (del temp.txt)
echo 8         >  temp.txt
echo 0         >> temp.txt
echo .1        >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Percent Doctorate.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running Median Household Income ...
IF EXIST temp.txt (del temp.txt)
echo 9         >  temp.txt
echo 20000     >> temp.txt
echo 150000    >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Med House Income.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running %% Receiving SSI ...
IF EXIST temp.txt (del temp.txt)
echo 11        >  temp.txt
echo 0         >> temp.txt
echo .15       >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Percent SSI.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running Median Year Built ...
IF EXIST temp.txt (del temp.txt)
echo 13        >  temp.txt
echo 1935      >> temp.txt
echo 1990      >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Med Yr Built.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running Median Home Value ...
IF EXIST temp.txt (del temp.txt)
echo 15        >  temp.txt
echo 100000    >> temp.txt
echo 500000   >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Med Home Value.kml" ff003300 ffffff00 < temp.txt

IF EXIST temp.txt (del temp.txt)
GOTO :EOF

REM * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
:CO
REM * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

SET ST="CO"
IF EXIST %ST%*.kml (del %ST%*.kml)

ECHO %ST% Running %% Caucasian ...
IF EXIST temp.txt (del temp.txt)
echo 4         >  temp.txt
echo 0.4       >> temp.txt
echo 1         >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Percent Caucasian.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running %% AA ...
IF EXIST temp.txt (del temp.txt)
echo 6         >  temp.txt
echo 0         >> temp.txt
echo 0.3       >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Percent AA.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running %% Short Commute ...
IF EXIST temp.txt (del temp.txt)
echo 7         >  temp.txt
echo .1        >> temp.txt
echo .7        >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Percent Short Commute.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running %% with Doctorate ...
IF EXIST temp.txt (del temp.txt)
echo 8         >  temp.txt
echo 0         >> temp.txt
echo .1        >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Percent Doctorate.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running Median Household Income ...
IF EXIST temp.txt (del temp.txt)
echo 9         >  temp.txt
echo 20000     >> temp.txt
echo 150000    >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Med House Income.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running %% Receiving SSI ...
IF EXIST temp.txt (del temp.txt)
echo 11        >  temp.txt
echo 0         >> temp.txt
echo .15       >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Percent SSI.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running Median Year Built ...
IF EXIST temp.txt (del temp.txt)
echo 13        >  temp.txt
echo 1935      >> temp.txt
echo 1990      >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Med Yr Built.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running Median Home Value ...
IF EXIST temp.txt (del temp.txt)
echo 15        >  temp.txt
echo 100000    >> temp.txt
echo 500000   >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Med Home Value.kml" ff003300 ffffff00 < temp.txt

IF EXIST temp.txt (del temp.txt)
GOTO :EOF

REM * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
:TN
REM * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

SET ST="TN"
IF EXIST %ST%*.kml (del %ST%*.kml)

ECHO %ST% Running %% Caucasian ...
IF EXIST temp.txt (del temp.txt)
echo 4         >  temp.txt
echo 0.4       >> temp.txt
echo 1         >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Percent Caucasian.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running %% AA ...
IF EXIST temp.txt (del temp.txt)
echo 6         >  temp.txt
echo 0         >> temp.txt
echo 0.3       >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Percent AA.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running %% Short Commute ...
IF EXIST temp.txt (del temp.txt)
echo 7         >  temp.txt
echo .1        >> temp.txt
echo .7        >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Percent Short Commute.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running %% with Doctorate ...
IF EXIST temp.txt (del temp.txt)
echo 8         >  temp.txt
echo 0         >> temp.txt
echo .1        >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Percent Doctorate.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running Median Household Income ...
IF EXIST temp.txt (del temp.txt)
echo 9         >  temp.txt
echo 20000     >> temp.txt
echo 150000    >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Med House Income.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running %% Receiving SSI ...
IF EXIST temp.txt (del temp.txt)
echo 11        >  temp.txt
echo 0         >> temp.txt
echo .15       >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Percent SSI.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running Median Year Built ...
IF EXIST temp.txt (del temp.txt)
echo 13        >  temp.txt
echo 1935      >> temp.txt
echo 1990      >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Med Yr Built.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running Median Home Value ...
IF EXIST temp.txt (del temp.txt)
echo 15        >  temp.txt
echo 100000    >> temp.txt
echo 500000   >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Med Home Value.kml" ff003300 ffffff00 < temp.txt

IF EXIST temp.txt (del temp.txt)
GOTO :EOF

REM * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
:MO
REM * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

SET ST="MO"
IF EXIST %ST%*.kml (del %ST%*.kml)

ECHO %ST% Running %% Caucasian ...
IF EXIST temp.txt (del temp.txt)
echo 4         >  temp.txt
echo 0.4       >> temp.txt
echo 1         >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Percent Caucasian.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running %% AA ...
IF EXIST temp.txt (del temp.txt)
echo 6         >  temp.txt
echo 0         >> temp.txt
echo 0.3       >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Percent AA.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running %% Short Commute ...
IF EXIST temp.txt (del temp.txt)
echo 7         >  temp.txt
echo .1        >> temp.txt
echo .7        >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Percent Short Commute.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running %% with Doctorate ...
IF EXIST temp.txt (del temp.txt)
echo 8         >  temp.txt
echo 0         >> temp.txt
echo .1        >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Percent Doctorate.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running Median Household Income ...
IF EXIST temp.txt (del temp.txt)
echo 9         >  temp.txt
echo 20000     >> temp.txt
echo 150000    >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Med House Income.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running %% Receiving SSI ...
IF EXIST temp.txt (del temp.txt)
echo 11        >  temp.txt
echo 0         >> temp.txt
echo .15       >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Percent SSI.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running Median Year Built ...
IF EXIST temp.txt (del temp.txt)
echo 13        >  temp.txt
echo 1935      >> temp.txt
echo 1990      >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Med Yr Built.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running Median Home Value ...
IF EXIST temp.txt (del temp.txt)
echo 15        >  temp.txt
echo 100000    >> temp.txt
echo 500000   >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Med Home Value.kml" ff003300 ffffff00 < temp.txt

IF EXIST temp.txt (del temp.txt)
GOTO :EOF

REM * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
:UT
REM * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

SET ST="UT"
IF EXIST %ST%*.kml (del %ST%*.kml)

ECHO %ST% Running %% Caucasian ...
IF EXIST temp.txt (del temp.txt)
echo 4         >  temp.txt
echo 0.4       >> temp.txt
echo 1         >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Percent Caucasian.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running %% AA ...
IF EXIST temp.txt (del temp.txt)
echo 6         >  temp.txt
echo 0         >> temp.txt
echo 0.3       >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Percent AA.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running %% Short Commute ...
IF EXIST temp.txt (del temp.txt)
echo 7         >  temp.txt
echo .1        >> temp.txt
echo .7        >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Percent Short Commute.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running %% with Doctorate ...
IF EXIST temp.txt (del temp.txt)
echo 8         >  temp.txt
echo 0         >> temp.txt
echo .1        >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Percent Doctorate.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running Median Household Income ...
IF EXIST temp.txt (del temp.txt)
echo 9         >  temp.txt
echo 20000     >> temp.txt
echo 150000    >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Med House Income.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running %% Receiving SSI ...
IF EXIST temp.txt (del temp.txt)
echo 11        >  temp.txt
echo 0         >> temp.txt
echo .15       >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Percent SSI.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running Median Year Built ...
IF EXIST temp.txt (del temp.txt)
echo 13        >  temp.txt
echo 1935      >> temp.txt
echo 1990      >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Med Yr Built.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running Median Home Value ...
IF EXIST temp.txt (del temp.txt)
echo 15        >  temp.txt
echo 100000    >> temp.txt
echo 500000   >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Med Home Value.kml" ff003300 ffffff00 < temp.txt

IF EXIST temp.txt (del temp.txt)
GOTO :EOF

REM * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
:CT
REM * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

SET ST="CT"
IF EXIST %ST%*.kml (del %ST%*.kml)

ECHO %ST% Running %% Caucasian ...
IF EXIST temp.txt (del temp.txt)
echo 4         >  temp.txt
echo 0.4       >> temp.txt
echo 1         >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Percent Caucasian.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running %% AA ...
IF EXIST temp.txt (del temp.txt)
echo 6         >  temp.txt
echo 0         >> temp.txt
echo 0.3       >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Percent AA.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running %% Short Commute ...
IF EXIST temp.txt (del temp.txt)
echo 7         >  temp.txt
echo .1        >> temp.txt
echo .7        >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Percent Short Commute.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running %% with Doctorate ...
IF EXIST temp.txt (del temp.txt)
echo 8         >  temp.txt
echo 0         >> temp.txt
echo .1        >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Percent Doctorate.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running Median Household Income ...
IF EXIST temp.txt (del temp.txt)
echo 9         >  temp.txt
echo 20000     >> temp.txt
echo 150000    >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Med House Income.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running %% Receiving SSI ...
IF EXIST temp.txt (del temp.txt)
echo 11        >  temp.txt
echo 0         >> temp.txt
echo .15       >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Percent SSI.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running Median Year Built ...
IF EXIST temp.txt (del temp.txt)
echo 13        >  temp.txt
echo 1935      >> temp.txt
echo 1990      >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Med Yr Built.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running Median Home Value ...
IF EXIST temp.txt (del temp.txt)
echo 15        >  temp.txt
echo 100000    >> temp.txt
echo 500000   >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Med Home Value.kml" ff003300 ffffff00 < temp.txt

IF EXIST temp.txt (del temp.txt)
GOTO :EOF

REM * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
:MI
REM * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

SET ST="MI"
IF EXIST %ST%*.kml (del %ST%*.kml)

ECHO %ST% Running %% Caucasian ...
IF EXIST temp.txt (del temp.txt)
echo 4         >  temp.txt
echo 0.4       >> temp.txt
echo 1         >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Percent Caucasian.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running %% AA ...
IF EXIST temp.txt (del temp.txt)
echo 6         >  temp.txt
echo 0         >> temp.txt
echo 0.3       >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Percent AA.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running %% Short Commute ...
IF EXIST temp.txt (del temp.txt)
echo 7         >  temp.txt
echo .1        >> temp.txt
echo .7        >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Percent Short Commute.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running %% with Doctorate ...
IF EXIST temp.txt (del temp.txt)
echo 8         >  temp.txt
echo 0         >> temp.txt
echo .1        >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Percent Doctorate.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running Median Household Income ...
IF EXIST temp.txt (del temp.txt)
echo 9         >  temp.txt
echo 20000     >> temp.txt
echo 150000    >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Med House Income.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running %% Receiving SSI ...
IF EXIST temp.txt (del temp.txt)
echo 11        >  temp.txt
echo 0         >> temp.txt
echo .15       >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Percent SSI.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running Median Year Built ...
IF EXIST temp.txt (del temp.txt)
echo 13        >  temp.txt
echo 1935      >> temp.txt
echo 1990      >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Med Yr Built.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running Median Home Value ...
IF EXIST temp.txt (del temp.txt)
echo 15        >  temp.txt
echo 100000    >> temp.txt
echo 500000   >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Med Home Value.kml" ff003300 ffffff00 < temp.txt

IF EXIST temp.txt (del temp.txt)
GOTO :EOF

REM * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
:MD
REM * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

SET ST="MD"
IF EXIST %ST%*.kml (del %ST%*.kml)

ECHO %ST% Running %% Caucasian ...
IF EXIST temp.txt (del temp.txt)
echo 4         >  temp.txt
echo 0.4       >> temp.txt
echo 1         >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Percent Caucasian.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running %% AA ...
IF EXIST temp.txt (del temp.txt)
echo 6         >  temp.txt
echo 0         >> temp.txt
echo 0.3       >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Percent AA.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running %% Short Commute ...
IF EXIST temp.txt (del temp.txt)
echo 7         >  temp.txt
echo .1        >> temp.txt
echo .7        >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Percent Short Commute.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running %% with Doctorate ...
IF EXIST temp.txt (del temp.txt)
echo 8         >  temp.txt
echo 0         >> temp.txt
echo .1        >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Percent Doctorate.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running Median Household Income ...
IF EXIST temp.txt (del temp.txt)
echo 9         >  temp.txt
echo 20000     >> temp.txt
echo 150000    >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Med House Income.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running %% Receiving SSI ...
IF EXIST temp.txt (del temp.txt)
echo 11        >  temp.txt
echo 0         >> temp.txt
echo .15       >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Percent SSI.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running Median Year Built ...
IF EXIST temp.txt (del temp.txt)
echo 13        >  temp.txt
echo 1935      >> temp.txt
echo 1990      >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Med Yr Built.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running Median Home Value ...
IF EXIST temp.txt (del temp.txt)
echo 15        >  temp.txt
echo 100000    >> temp.txt
echo 500000   >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Med Home Value.kml" ff003300 ffffff00 < temp.txt

IF EXIST temp.txt (del temp.txt)
GOTO :EOF

REM * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
:NC
REM * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

SET ST="NC"
IF EXIST %ST%*.kml (del %ST%*.kml)

ECHO %ST% Running %% Caucasian ...
IF EXIST temp.txt (del temp.txt)
echo 4         >  temp.txt
echo 0.4       >> temp.txt
echo 1         >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Percent Caucasian.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running %% AA ...
IF EXIST temp.txt (del temp.txt)
echo 6         >  temp.txt
echo 0         >> temp.txt
echo 0.3       >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Percent AA.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running %% Short Commute ...
IF EXIST temp.txt (del temp.txt)
echo 7         >  temp.txt
echo .1        >> temp.txt
echo .7        >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Percent Short Commute.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running %% with Doctorate ...
IF EXIST temp.txt (del temp.txt)
echo 8         >  temp.txt
echo 0         >> temp.txt
echo .1        >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Percent Doctorate.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running Median Household Income ...
IF EXIST temp.txt (del temp.txt)
echo 9         >  temp.txt
echo 20000     >> temp.txt
echo 150000    >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Med House Income.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running %% Receiving SSI ...
IF EXIST temp.txt (del temp.txt)
echo 11        >  temp.txt
echo 0         >> temp.txt
echo .15       >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Percent SSI.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running Median Year Built ...
IF EXIST temp.txt (del temp.txt)
echo 13        >  temp.txt
echo 1935      >> temp.txt
echo 1990      >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Med Yr Built.kml" ff003300 ffffff00 < temp.txt

ECHO %ST% Running Median Home Value ...
IF EXIST temp.txt (del temp.txt)
echo 15        >  temp.txt
echo 100000    >> temp.txt
echo 500000   >> temp.txt
echo 96        >> temp.txt
engine "ACS 2009 Data - %ST%.csv" "%ST% Med Home Value.kml" ff003300 ffffff00 < temp.txt

IF EXIST temp.txt (del temp.txt)
GOTO :EOF