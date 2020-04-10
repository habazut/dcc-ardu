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
    conversionFactor=3;
} // CurrentMonitor::CurrentMonitor
  
/* Note: millis() uses TIMER-0.  For UNO, we change the scale on Timer-0. */
/* For MEGA we do not.  This means millis() on the UNO is approx 8 to 10  */
/* times too fast (actual scale depending on DCC packet output) and       */
/* CURENT_SAMPLE_TIME is different for UNO than MEGA to compensate        */

boolean CurrentMonitor::checkTime(){
  unsigned long now;
  now = millis();
  if((unsigned long)(sampleTime-tickCounter) < SAMPLE_TICKS)            // no need to check current yet
    return(false);
  sampleTime=tickCounter;
  return(true);  
} // CurrentMonitor::checkTime
  
unsigned int CurrentMonitor::read() {
    return (unsigned int) conversionFactor * analogRead(currentpin);
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

