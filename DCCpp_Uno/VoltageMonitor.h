/**********************************************************************

VoltageMonitor.h
COPYRIGHT (c) 2020      Harald Barth

Part of DCC++ BASE STATION for the Arduino

**********************************************************************/

#ifndef VoltageMonitor_h
#define VoltageMonitor_h

#include "Arduino.h"

#define VOLTARR 10
class VoltageMonitor {

  byte vcount=VOLTARR;
  byte signalpin;
  byte voltagepin;
  byte v=0;
  unsigned int voltage[VOLTARR];  // Real (corrected) current in mA, range 1mA to ~ 30A.
  int conversionPercent;          // Percentvalue to get mA from internal 0-1023 value.
                                  // For a factor of 3 use 300, for 1.5 use 150
public:
  VoltageMonitor(byte, byte);
  void check();
  unsigned int read();
  unsigned int getVoltage();
};

#endif

