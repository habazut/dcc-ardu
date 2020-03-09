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

CurrentMonitor::CurrentMonitor(int sp, int cp, const char *msg){
    this->signalpin=sp;
    this->currentpin=cp;
    this->msg=msg;
    current=0;
  } // CurrentMonitor::CurrentMonitor
  
/* Note: millis() uses TIMER-0.  For UNO, we change the scale on Timer-0. */
/* For MEGA we do not.  This means millis() on the UNO is approx 8 to 10  */
/* times too fast (actual scale depending on DCC packet output) and       */
/* CURENT_SAMPLE_TIME is different for UNO than MEGA to compensate        */

boolean CurrentMonitor::checkTime(){
  unsigned long now;
  now = millis();
  if((unsigned long)(now-sampleTime) < CURRENT_SAMPLE_TIME)            // no need to check current yet
    return(false);
  sampleTime=now;
  return(true);  
} // CurrentMonitor::checkTime
  
void CurrentMonitor::check(){
  current=analogRead(currentpin)*CURRENT_SAMPLE_SMOOTHING+current*(1.0-CURRENT_SAMPLE_SMOOTHING); // compute new exponentially-smoothed current
  if(current>CURRENT_SAMPLE_MAX){                                                                 // current overload and pin is on
    digitalWrite(signalpin,LOW);                                                                  // disable pin in question
    INTERFACE.print(msg);                                                                         // print corresponding error message
  }    
} // CurrentMonitor::check  

long int CurrentMonitor::sampleTime=0;

