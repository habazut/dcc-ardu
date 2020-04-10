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

CurrentMonitor::CurrentMonitor(byte sp, byte cp, const char *msg){
    this->signalpin=sp;
    this->currentpin=cp;
    this->msg=msg;
    current=0;
    conversionPercent=300;           // see CurrentMonitor.h
} // CurrentMonitor::CurrentMonitor
  
boolean CurrentMonitor::checkTime(){
  if((unsigned long)(sampleTime-tickCounter) < SAMPLE_TICKS)            // no need to check current yet
    return(false);
  sampleTime=tickCounter;
  return(true);  
} // CurrentMonitor::checkTime
  
unsigned int CurrentMonitor::read() {
    return (unsigned int)(((unsigned long int)conversionPercent * analogRead(currentpin)) / 100);  // Force long int calc
}

void CurrentMonitor::check(){
  current=read()/2 + current/2;                                        // simplified INTERGER arithmetics!!
  if(current>CURRENT_SAMPLE_MAX){                                      // current overload and pin is on
    digitalWrite(signalpin,LOW);                                       // disable pin in question
    INTERFACE.print(msg);                                              // print corresponding error message
  }    
} // CurrentMonitor::check  

unsigned int CurrentMonitor::getCurrent() {
    return current;
}

long int CurrentMonitor::sampleTime=0;

