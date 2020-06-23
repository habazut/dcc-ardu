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
    power=0;
    conversionPromille=CURRENT_CONVERSION_PROMILLE;                 // see CurrentMonitor.h
    vccPromille=0;
    errors=0;
} // CurrentMonitor::CurrentMonitor

// Returns the promille current readings must be corrected
// because Vref = Vcc is off. So if Vcc is 90.9% this
// returns 1000/909=1100.
int CurrentMonitor::vccCorrection() {
  long int result;
  long int debugresult;
  int returnval;

  // Read 1.1V reference against AVcc
#if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
#elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
  ADMUX = _BV(MUX5) | _BV(MUX0);
#elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
  ADMUX = _BV(MUX3) | _BV(MUX2);
#else
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
#endif
  delayMicroseconds(600); // My tests have given that the results stabilize at approx 500us (and above)
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA, ADSC));
  result = ADCL;
  result |= ADCH << 8;
  // 1.1*1024=1126.4
  returnval = 1000L*225/result;
#define DEBUGPRINT
#ifdef DEBUGPRINT
  debugresult = 1126400L / result; // Calculate Vcc (in mV); 1126400 = 1.1*1024*1000
  INTERFACE.print(F("<V "));
  INTERFACE.print(debugresult);
  INTERFACE.print(F(" "));
  INTERFACE.print(returnval);
  INTERFACE.println(F(">"));
#endif
  return returnval;
}

void CurrentMonitor::on() {
    digitalWrite(signalpin, HIGH);
    power = 1;
}

void CurrentMonitor::off() {
    digitalWrite(signalpin, LOW);
    power = 0;
}

unsigned int CurrentMonitor::read() {
    if( vccPromille == 0) {
	vccPromille = (vccCorrection() + vccCorrection())/2;  // Average over 2 readings
    }
    return (unsigned int)(((unsigned long int)conversionPromille * vccPromille * analogRead(currentpin)) / 1000000L);  // Force long int calc
}

void CurrentMonitor::check(){
  current = read();
  if(current > currentlimit) {                     // current overload
      errors++;                                    // start counting
      /*INTERFACE.print(".");*/
    if (errors >= SHORT_SENSITIVITY){
      off();                                       // turn off this track
      INTERFACE.print(F("<p2 "));                  // print corresponding error message
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

