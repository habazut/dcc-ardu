/**********************************************************************

SerialCommand.h
COPYRIGHT (c) 2013-2016 Gregg E. Berman
COPYRIGHT (c) 2020      Harald Barth

Part of DCC++ BASE STATION for the Arduino

**********************************************************************/

#ifndef SerialCommand_h
#define SerialCommand_h

#include "PacketRegister.h"
#include "CurrentMonitor.h"
#include "VoltageMonitor.h"

#define  MAX_COMMAND_LENGTH         30

struct SerialCommand{
  static char commandString[MAX_COMMAND_LENGTH+1];
  static volatile RegisterList *mRegs, *pRegs;
  static void init(volatile RegisterList *, volatile RegisterList *);
  static void parse(char *);
  static void process();
  static void printHeader();
}; // SerialCommand
  
#endif




