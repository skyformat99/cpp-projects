@echo off
del jasiohost.jar
del com\synthbot\jasiohost\*.class
javac com\synthbot\jasiohost\*.java
jar -cfm jasiohost.jar Manifest.txt com\synthbot\jasiohost\*.class