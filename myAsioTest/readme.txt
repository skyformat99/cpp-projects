C++ ASIO / JNI JAsioWave Wrapper Test
Adam Dziorny
10/31/2010

Included Files
==============

AsioWave.cpp, AsioWave.h
 - These files define the AsioWave class in C++, which communicates
   with the ASIO system using the COM drivers provided by Steinberg
   Media Technologies. 

asioWave.dll
 - A dynamic linked library containing the JAsioWave functions, the 
   AsioWave class, and all helper functions / classes. The Java wrapper
   class JAsioWave.java requires this file in its library path.

JAsioWave.java, JAsioWave.class
 - The Java wrapper class, containing the JAsioWave java class and the
   native method calls. These native method calls link to the functions
   written in C++ and implemented in JAsioWave.cpp.

JAsioException.java, JAsioException.class
 - Simple exception class, extending RuntimeException, which is thrown
   when an exception is thrown in the C++ wrapper code. This should 
   eliminate unstable exits from the C++ code.

JAsioWave.cpp, JAsioWave.h
 - These files contain the C++ side of the JNI wrapping of the Java class
   JAsioWave. Note that the file JAsioWave.h is machine-generated and should
   not be modified.

readme.txt 
 - This file.

Sample Call Sequence
====================

Assume we're making a call in Java to JAsioWave.displayChannelInfo, a public
member function inside this class which displays info on each of the channels. 
The sequence of calls [filename] is as follows:

[JAsioWave.class] displayChannelInfo                  // Original calling method
[JAsioWave.class] _displayChannelInfo                 // native method
[JAsioWave.cpp] Java_JAsioWave__1displayChannelInfo   // JNI call from Java
[AsioWave.cpp] displayChannelInfo                     // AsioWave object call
[asiodriver] ASIOGetChannelInfo                       // COM, via asiodriver interface

This method does not return any information, but clearly returning information 
would follow the same steps in the reverse direction.

Build Procedure
===============

The initial build of the JNI-linked class required the following:

// Compiling the Java wrapper class normally
javac JAsioWave.java

// Using the Java header generator to generate JNI headers for
// each of the native methods in the JAsioWave class
javah -jni JAsioWave

This header file, called JAsioWave.h, is machine-generated and not
changed. Next, JAsioWave.cpp was created with function declarations
copy-and-pasted from the header. The bodies of these functions
linked to the AsioWave class, to perform operations on the
AsioDrivers class (provided by Steinberg Media Technologies).

These files were all compiled and linked into the asioWave.dll library
using the following commands (broken apart for clarity):

cl /c /EHsc /nologo /D"MYDEBUG"=1 
  /I"C:/usr/cpp/asiosdk2/common" 
  /I"c:/usr/cpp/asiosdk2/host"  
  /I"c:/usr/cpp/asiosdk2/host/pc" 
  /I"C:/Program Files (x86)/Java/jdk1.6.0_22/include" 
  /I"C:/Program Files (x86)/Java/jdk1.6.0_22/include/win32" 
  "C:\usr\cpp\ASIOSDK2\host\asiodrivers.cpp" 
  "C:\usr\cpp\ASIOSDK2\host\pc\asiolist.cpp" 
  "C:\usr\cpp\ASIOSDK2\common\asio.cpp" 
  "C:\usr\cpp\ASIOSDK2\common\combase.cpp" 
  AsioWave.cpp JAsioWave.cpp

link /NOLOGO /DLL /OUT:"asioWave.dll" 
  JAsioWave.obj AsioWave.obj 
  asio.obj asiodrivers.obj asiolist.obj combase.obj 
  ole32.lib Advapi32.lib User32.lib 

The Java wrapper class JAsioWave.java was then recompiled using the following:

javac JAsioWave.java JAsioException.java

And JAsioWave was executed, with the code in the main entry point method running
a test sequence consisting of getting/displaying the available devices, loading
the first device (presumably the fast track), viewing channel information, creating
the buffers and playing a tone of the given spec's. This was executed using the 
following command:

java JAsioWave
