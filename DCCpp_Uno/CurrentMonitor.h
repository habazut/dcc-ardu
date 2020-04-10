/**********************************************************************

CurrentMonitor.h
COPYRIGHT (c) 2013-2016 Gregg E. Berman
              2016-2020 Harald Barth

Part of DCC++ BASE STATION for the Arduino

**********************************************************************/

#ifndef CurrentMonitor_h
#define CurrentMonitor_h

#include "Arduino.h"

#define  CURRENT_SAMPLE_MAX        1000       // When to turn off tracks (in mA)
#define  SAMPLE_TICKS              5000       // 1 tick is 4us so 5000 is 20ms

class CurrentMonitor {

  static long int sampleTime;
  byte signalpin;
  byte currentpin;
  int current;                                // Real (corrected) current in mA, range 1mA to ~ 30A.
  int conversionFactor;                       // Value to multiply to get mA from internal 0-1023 value
  int conversionBias;                         // Value to add to get a zero read at no load
  const char *msg;

public:
  CurrentMonitor(byte, byte, const char *);
  static boolean checkTime();
  void check();
  unsigned int read();
  unsigned int getCurrent();
};

#endif

