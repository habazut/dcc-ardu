/**********************************************************************

VoltageMonitor.cpp
COPYRIGHT (c) 2020      Harald Barth

Part of DCC++ BASE STATION for the Arduino

**********************************************************************/

#include "DCCpp_Uno.h"
#include "VoltageMonitor.h"
#include "Comm.h"

///////////////////////////////////////////////////////////////////////////////

class VoltageMonitor;

VoltageMonitor::VoltageMonitor(byte sp, byte vp){
    this->signalpin=sp;
    this->voltagepin=vp;
    for(int n=0; n<vcount; n++)
	voltage[n]=0;
    conversionPercent=300;           // see VoltageMonitor.h
} // VoltageMonitor::VoltageMonitor
  
unsigned int VoltageMonitor::read() {
    return (unsigned int) analogRead(voltagepin);
}

void VoltageMonitor::check(){
  voltage[v] = read();
  v = (v+1) % vcount;
} // VoltageMonitor::check  

unsigned int VoltageMonitor::getVoltage() {
    unsigned int vmax = 0;
    for (int n=0; n<vcount; n++) {
	if(voltage[n]>vmax) {
	    vmax = voltage[n];
	}
    }
    return vmax;
}
