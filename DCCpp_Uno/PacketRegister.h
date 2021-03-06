/**********************************************************************

PacketRegister.h
COPYRIGHT (c) 2013-2016 Gregg E. Berman
              2016-2020 Harald Barth

Part of DCC++ BASE STATION for the Arduino

**********************************************************************/

#ifndef PacketRegister_h
#define PacketRegister_h

#include "Arduino.h"
#include "CurrentMonitor.h"

// Define constants used for reading CVs from the Programming Track

#define  ACK_BASE_COUNT            100      // number of analogRead samples to take before each CV verify to establish a baseline current
#define  ACK_SAMPLE_SMOOTHING      0.7      // exponential smoothing to use in processing the analogRead samples after a CV verify (bit or byte) has been sent
#define  ACK_SAMPLE_THRESHOLD       55      // the threshold that the exponentially-smoothed analogRead samples 
                                            // (after subtracting the baseline current) must cross to establish ACKNOWLEDGEMENT
                                            // The value is when taken from CurrentMonitor::read() in mA.

// Define a series of registers that can be sequentially accessed over a loop to generate a repeating series of DCC Packets

struct Register{
  byte buf[7];   /* 56 bits: 6*8=48 bits of DCC data + 7 start/stop bits + 1 internal flag bit */
  byte nBits;
}; // Packet, for now named Register 
  
struct RegisterList{  
  int maxNumRegs;
  unsigned long packetsTransmitted;
  Register *reg;
  Register **regMap;
  Register *currentReg;
  Register *maxLoadedReg;
  Register *nextReg;
  Register *recycleReg;
  byte currentBit;
  byte nRepeat;
  byte debugcount;
  byte *speedTable;
  static byte idlePacket[];
  static byte resetPacket[];
  static byte bitMask[];
  RegisterList(int);
  byte ackdetect(unsigned int) volatile;
  byte poweron() volatile;
  unsigned int readBaseCurrent() volatile;
  void loadPacket(int, byte *, int, int, int=0) volatile;
  void setThrottle(char *) volatile;
  void setFunction(char *) volatile;  
  void setAccessory(char *) volatile;
  void writeTextPacket(char *) volatile;
  void readCV(char *) volatile;
  void writeCVByte(char *) volatile;
  void writeCVBit(char *) volatile;
  void writeCVByteMain(char *) volatile;
  void writeCVBitMain(char *s) volatile;  
  void printPacket(int, byte *, int, int) volatile;
  void printMaxNumRegs() volatile;
};

#endif
