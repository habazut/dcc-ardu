What’s DCC++
------------

DCC++ is an open-source hardware and software system for the operation of DCC-equipped model railroads.

This is a fork of the DCC++ Base Station code base.

The DCC++ Base Station consists of an Arduino micro controller fitted with an Arduino Motor Shield that can be connected directly to the tracks of a model railroad.

What’s in this Repository
-------------------------

This repository, BaseStation-Uno, contains a complete DCC++ Base Station sketch designed for compiling and uploading into an Arduino Uno.  All sketch files are in the folder named DCCpp_Uno. More information about the sketch can be found in the included PDF file.

To utilize this sketch, simply download a zip file of this repository and open the file DCCpp_Uno.ino within the DCCpp_Uno folder using your Arduino IDE. This can be done through the green <em>Clone or download...</em> button on the upper right. The master version is available <A HREF="https://github.com/habazut/dcc-ardu/archive/master.zip"> behind this ZIP archive link </A> as well.

 Please do not rename the folder containing the sketch code, nor add any files to that folder.  The Arduino IDE relies on the structure and name of the folder to properly display and compile the code.

The version on the Master branch is currently 11.0.3+haba. The high version number has been chose to avoid conflicts
with the classic DCC++ code but should not indicate that dcc-ardu is "more".

* Supports both the Arduino Uno and Arduino Mega (Mega untested)
* Built-in configuration for both the original Arduino Motor Shield as well as a Pololu MC33926 Motor Shield
* Built-in configuration and support of Ethernet Shields (for use with Mega only, untested)
* Fixed: CV write in service mode (bitwise and bytewise)
* Fixed: CV read in service mode (direct byte)
* Emitsts a trigger pulse on pin 4 at end of preamble to be used with a scope if enabled in Config.h

Some notes taken during the project can be found <A HREF="https://habazut.github.io/dcc-ardu/"> in the docs corner</A>.

This fork will be geared to be used together with JMRI and not as a standalone layout controller.

-Sat Apr 11 21:00:00 CEST 2020




