/**********************************************************************

CurrentMonitor.cpp
COPYRIGHT (c) 2013-2016 Gregg E. Berman
              2016-2020 Harald Barth

Part of DCC++ BASE STATION for the Arduino

**********************************************************************/

#include "DCCpp_Uno.h"
#include "CurrentMonitor.h"
#include "Comm.h"

///////////////////////////////////////////////////////////////////////////////

class CurrentMonitor;

CurrentMonitor::CurrentMonitor(byte sp, byte cp, int cl, const char *msg){
    this->signalpin=sp;
    this->currentpin=cp;
    this->currentlimit=cl;
    this->msg=msg;
    current=0;
    conversionPercent=CURRENT_CONVERSION_PERCENT;                 // see CurrentMonitor.h
} // CurrentMonitor::CurrentMonitor
  
unsigned int CurrentMonitor::read() {
    return (unsigned int)(((unsigned long int)conversionPercent * analogRead(currentpin)) / 100);  // Force long int calc
}

void CurrentMonitor::check(){
  int c = read();
  current=c/2 + current/2;                         // simplified INTERGER arithmetics to smooth current
  if(c > 2*currentlimit || current>currentlimit){  // current overload: 2x current - cut direct, otherwise
                                                   // use smoothed value. This algorithm can be improved.
    digitalWrite(signalpin,LOW);                   // disable pin in question
    INTERFACE.print(F("<p2 "));                    // print corresponding error message
    INTERFACE.print(msg);
    INTERFACE.print(F(" "));
    if (c > 2*currentlimit)
      INTERFACE.print(c);                          // momentary current
    else
      INTERFACE.print(current);                    // smoothed current over time
    INTERFACE.print(F(">"));
    current = currentlimit;                        // so we don't get false triggers next time
                                                   // because of smoothing. If overcurrent persists
                                                   // next read() will trigger again.
  }    
} // CurrentMonitor::check  

unsigned int CurrentMonitor::getCurrent() {
    return current;
}

long int CurrentMonitor::sampleTime=0;

