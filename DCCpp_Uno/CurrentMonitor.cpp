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
    errors=0;
} // CurrentMonitor::CurrentMonitor
  
unsigned int CurrentMonitor::read() {
    return (unsigned int)(((unsigned long int)conversionPercent * analogRead(currentpin)) / 100);  // Force long int calc
}

void CurrentMonitor::check(){
  current = read();
  if(current > currentlimit) {                     // current overload
      errors++;                                    // start counting
      /*INTERFACE.print(".");*/
    if (errors >= SHORT_SENSITIVITY){
	digitalWrite(signalpin,LOW);               // disable pin in question
	INTERFACE.print(F("<p2 "));                // print corresponding error message
	INTERFACE.print(msg);
	INTERFACE.print(" ");
	INTERFACE.print(current);
	INTERFACE.print(F(">"));
    }
  } else {
      errors = 0;                                  // current under limit, reset counter
  }
} // CurrentMonitor::check  

unsigned int CurrentMonitor::getCurrent() {
    return current;
}

long int CurrentMonitor::sampleTime=0;

