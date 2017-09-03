@ECHO OFF
FOR %%I IN ("C:\Users\Public\Recorded TV\*.wtv") DO CALL "c:\usr\cpp\checkFileSizes\convertFile.bat" "%%I" .01
