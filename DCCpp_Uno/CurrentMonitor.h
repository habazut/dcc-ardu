/**********************************************************************

CurrentMonitor.h
COPYRIGHT (c) 2013-2016 Gregg E. Berman
              2016-2020 Harald Barth

Part of DCC++ BASE STATION for the Arduino

**********************************************************************/

#ifndef CurrentMonitor_h
#define CurrentMonitor_h

#include "Arduino.h"

#define  CURRENT_SAMPLE_SMOOTHING   0.01
#define  CURRENT_SAMPLE_MAX         300

#ifdef ARDUINO_AVR_UNO                        // Configuration for UNO
  #define  CURRENT_SAMPLE_TIME        20
#else                                         // Configuration for MEGA    
  #define  CURRENT_SAMPLE_TIME        2
#endif

struct CurrentMonitor{
  static long int sampleTime;
  int signalpin;
  int currentpin;
  float current;
  const char *msg;
  CurrentMonitor(int, int, const char *);
  static boolean checkTime();
  void check();
};

#endif

