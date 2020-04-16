/**********************************************************************

CurrentMonitor.h
COPYRIGHT (c) 2013-2016 Gregg E. Berman
              2016-2020 Harald Barth

Part of DCC++ BASE STATION for the Arduino

**********************************************************************/

#ifndef CurrentMonitor_h
#define CurrentMonitor_h

#include "Arduino.h"
#include "Config.h"

#if MOTOR_SHIELD_TYPE == 0
#define CURRENT_CONVERSION_PERCENT 296   // 0.0049/1.65*1000*100
#endif
#if MOTOR_SHIELD_TYPE == 1
#define CURRENT_CONVERSION_PERCENT 933   // 0.0049/0.525*1000*100
#endif
// These value will not work because if then DAC returns 1 then we
// already draw 466mA so the resolution is totally broken (a value
// of 140 is 65A) and we overflow an unsigned int anyway.
//#if MOTOR_SHIELD_TYPE == BTS7960B_XX
//#define CURRENT_CONVERSION_PERCENT 46666       0.0049/0.0105*1000*100 ???????
//#endif

class CurrentMonitor {

  static long int sampleTime;
  byte signalpin;
  byte currentpin;
  byte power;
  int current;                    // Real (corrected) current in mA, range 1mA to ~ 30A.
  int conversionPercent;          // Percentvalue to get mA from internal 0-1023 value.
                                  // For a factor of 3 use 300, for 1.5 use 150
  int currentlimit;               // limit for this output in mA
  const char *msg;

public:
  CurrentMonitor(byte, byte, int, const char *);
  void on();
  void off();
  void check();
  inline byte powerstatus() {
      return power;
  }
  unsigned int read();
  unsigned int getCurrent();
};

#endif

