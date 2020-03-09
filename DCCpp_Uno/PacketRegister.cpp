/**********************************************************************

PacketRegister.cpp
COPYRIGHT (c) 2013-2016 Gregg E. Berman
              2016-2020 Harald Barth

Part of DCC++ BASE STATION for the Arduino

**********************************************************************/

#include "DCCpp_Uno.h"
#include "PacketRegister.h"
#include "Comm.h"

///////////////////////////////////////////////////////////////////////////////
    
RegisterList::RegisterList(int maxNumRegs, byte isProgReg){
  this->isProgReg=isProgReg;
  this->maxNumRegs=maxNumRegs;
  packetsTransmitted = 0;
  reg=(Register *)calloc((maxNumRegs+1),sizeof(Register));
  regMap=(Register **)calloc((maxNumRegs+1),sizeof(Register *));
  speedTable=(byte *)calloc((maxNumRegs+1),sizeof(byte));
  currentReg=reg;
  regMap[0]=reg;
  maxLoadedReg=reg;
  nextReg=NULL;
  recycleReg = NULL;
  currentBit=0;
  nRepeat=0;
  debugcount=0;
} // RegisterList::RegisterList
  
///////////////////////////////////////////////////////////////////////////////

// LOAD DCC PACKET INTO TEMPORARY REGISTER 0, OR PERMANENT REGISTERS 1 THROUGH DCC_PACKET_QUEUE_MAX (INCLUSIVE)
// CONVERTS 2, 3, 4, OR 5 BYTES INTO A DCC BIT STREAM WITH PREAMBLE, CHECKSUM, AND PROPER BYTE SEPARATORS
// BITSTREAM IS STORED IN UP TO A 9-BYTE ARRAY (USING AT MOST 69 OF 72 BITS)

void RegisterList::loadPacket(int nReg, byte *b, int nBytes, int nRepeat, int printFlag) volatile {
  Register *loopReg = NULL;
  Register *newReg = NULL;
  byte of=0;
  
  nReg=nReg%((maxNumRegs+1));          // force nReg to be between 0 and maxNumRegs, inclusive

  if (nReg != 0) {                     // nReg = 0 is the special "direct out" register
    newReg=maxLoadedReg+1;
    for(loopReg=reg; loopReg <=maxLoadedReg; loopReg++) {
        if (loopReg == recycleReg) {
          newReg = recycleReg;
	  break;
	}
    }
    if(regMap[nReg]==NULL)
	recycleReg = NULL;
    else
	recycleReg = regMap[nReg];    // remember where the regMap[nReg] that will be invalidated was stored
    regMap[nReg]=newReg;              // set the regMap[nReg] to be updated
  } else                              // if nReg is 0 then we have to wait here, otherwise we can wait later
    while(nextReg!=NULL);             // busy wait while there is a Register already waiting to be updated
                                      // nextReg will be reset to NULL by interrupt when prior Register updated fully processed
 
  Register *p=regMap[nReg];           // set Register to be updated
  byte *buf=p->buf;                   // set byte buffer in the Packet to be updated
          
  b[nBytes]=b[0];                        // copy first byte into what will become the checksum byte  
  for(int i=1;i<nBytes;i++)              // XOR remaining bytes into checksum byte
    b[nBytes]^=b[i];
  nBytes++;                              // increment number of bytes in packet to include checksum byte
      
  if (isProgReg && nBytes <= 5) {        // service mode programming (progRegs) need long preamble 
    of=1;                                // all service mode commands _should_ be <= 5 bytes
    buf[0]=0xFF;                         // first  8 bit of 22-bit preamble
  }
  buf[0+of]=0xFF;                        // first  8 bit of 14-bit preamble or second 8 bit of 22-bit preamble
  buf[1+of]=0xFC + bitRead(b[0],7);      // last   6 bit of preamble + data start bit + b[0], bit 7
  buf[2+of]=b[0]<<1;                     // b[0], bits 6-0 + data start bit
  buf[3+of]=b[1];                        // b[1], all bits
  buf[4+of]=b[2]>>1;                     // b[2], bits 7-1
  buf[5+of]=b[2]<<7;                     // b[2], bit 0
  
  if(nBytes==3){
    bitSet(buf[5+of],6);                 // endbit
    p->nBits=42+8*of;
  } else{
    buf[5+of]+=b[3]>>2;                  // b[3], bits 7-2
    buf[6+of]=b[3]<<6;                   // b[3], bit 1-0
    if(nBytes==4){
      bitSet(buf[6+of],5);               // endbit
      p->nBits=51+8*of;
    } else{
      buf[6+of]+=b[4]>>3;                // b[4], bits 7-3
      buf[7+of]=b[4]<<5;                 // b[4], bits 2-0
      if(nBytes==5){
	bitSet(buf[7+of],4);             // endbit
        p->nBits=60+8*of;
      } else{
        buf[7+of]+=b[5]>>4;              // b[5], bits 7-4
        buf[8]=b[5]<<4;               // b[5], bits 3-0
	bitSet(buf[8],3);             // endbit
        p->nBits=69;
      } // >5 bytes
    } // >4 bytes
  } // >3 bytes
  buf[8] &= 0xFE;                     // clear invalid flag on this register/packet content
  
  if (nReg != 0 && recycleReg!=NULL)
      (recycleReg->buf)[8] |= 0x01;   // set invalid flag on recycleReg packet content

  if (nReg != 0)                    // if nReg was 0 then we waited above
    while(nextReg!=NULL);           // busy wait while there is a Register already waiting to be updated
                                    // nextReg will be reset to NULL by interrupt when prior Register updated fully processed
  nextReg=p;

  this->nRepeat=nRepeat;
  maxLoadedReg=max(maxLoadedReg,nextReg);
  
  if(printFlag && SHOW_PACKETS)       // for debugging purposes
    printPacket(nReg,b,nBytes,nRepeat);  

} // RegisterList::loadPacket

///////////////////////////////////////////////////////////////////////////////

void RegisterList::setThrottle(char *s) volatile{
  byte b[5];                      // save space for checksum byte
  int nReg;
  int cab;
  int tSpeed;
  int tDirection;
  byte nB=0;
  
  if(sscanf(s,"%d %d %d %d",&nReg,&cab,&tSpeed,&tDirection)!=4)
    return;

  if(nReg<1 || nReg>maxNumRegs)
    return;  

  if(cab>127)
    b[nB++]=highByte(cab) | 0xC0;      // convert train number into a two-byte address

  if(tSpeed > 126)                     // Cap speed at max value 126
      tSpeed = 126;

  tDirection &= 0x01;                  // Only look at direction bit
    
  b[nB++]=lowByte(cab);
  b[nB++]=0x3F;                        // 128-step speed control byte
  if(tSpeed>=0) 
    b[nB++]=tSpeed+(tSpeed>0)+tDirection*128;   // max speed is 126, but speed codes range from 2-127 (0=stop, 1=emergency stop)
  else{
    b[nB++]=1;
    tSpeed=0;
  }
       
  loadPacket(nReg,b,nB,0,1);
  
  INTERFACE.print(F("<T"));
  INTERFACE.print(nReg); INTERFACE.print(F(" "));
  INTERFACE.print(tSpeed); INTERFACE.print(F(" "));
  INTERFACE.print(tDirection);
  INTERFACE.print(F(">"));
  
  speedTable[nReg]=tSpeed+tDirection*128;
    
} // RegisterList::setThrottle()

///////////////////////////////////////////////////////////////////////////////

void RegisterList::setFunction(char *s) volatile{
  byte b[5];                      // save space for checksum byte
  int cab;
  int fByte, eByte;
  int nParams;
  byte nB=0;
  
  nParams=sscanf(s,"%d %d %d",&cab,&fByte,&eByte);
  
  if(nParams<2)
    return;

  if(cab>127)
    b[nB++]=highByte(cab) | 0xC0;      // convert train number into a two-byte address
    
  b[nB++]=lowByte(cab);

  if(nParams==2){                      // this is a request for functions FL,F1-F12  
    b[nB++]=(fByte | 0x80) & 0xBF;     // for safety this guarantees that first nibble of function byte will always be of binary form 10XX which should always be the case for FL,F1-F12  
  } else {                             // this is a request for functions F13-F28
    b[nB++]=(fByte | 0xDE) & 0xDF;     // for safety this guarantees that first byte will either be 0xDE (for F13-F20) or 0xDF (for F21-F28)
    b[nB++]=eByte;
  }
    
  loadPacket(0,b,nB,4,1);
    
} // RegisterList::setFunction()

///////////////////////////////////////////////////////////////////////////////

void RegisterList::setAccessory(char *s) volatile{
  byte b[3];                      // save space for checksum byte
  int aAdd;                       // the accessory address (0-511 = 9 bits) 
  int aNum;                       // the accessory number within that address (0-3)
  int activate;                   // flag indicated whether accessory should be activated (1) or deactivated (0) following NMRA recommended convention
  
  if(sscanf(s,"%d %d %d",&aAdd,&aNum,&activate)!=3)
    return;
    
  b[0]=aAdd%64+128;                                           // first byte is of the form 10AAAAAA, where AAAAAA represent 6 least signifcant bits of accessory address  
  b[1]=((((aAdd/64)%8)<<4) + (aNum%4<<1) + activate%2) ^ 0xF8;      // second byte is of the form 1AAACDDD, where C should be 1, and the least significant D represent activate/deactivate
      
  loadPacket(0,b,2,4,1);
      
} // RegisterList::setAccessory()

///////////////////////////////////////////////////////////////////////////////

void RegisterList::writeTextPacket(char *s) volatile{
  
  int nReg;
  byte b[6];
  int nBytes;
  volatile RegisterList *regs;
    
  nBytes=sscanf(s,"%d %x %x %x %x %x",&nReg,b,b+1,b+2,b+3,b+4)-1;
  
  if(nBytes<2 || nBytes>5){    // invalid valid packet
    INTERFACE.print(F("<mInvalid Packet>"));
    return;
  }
         
  loadPacket(nReg,b,nBytes,0,1);
    
} // RegisterList::writeTextPacket()

///////////////////////////////////////////////////////////////////////////////

/* ackdetect side-effect: Will restore resetPacket to slot 1 */

byte RegisterList::ackdetect(int base) volatile{
    byte upflankFound = 0;
    byte ackFound = 0;
    int c = 0;
    byte searchLowflank = 1;
    int current;
    unsigned long acktime, lowflankMicros, upflankMicros;
    unsigned long oldPacketCounter;

    oldPacketCounter = packetsTransmitted; // remember time when we started
/*    for(int j=0;j<ACK_SAMPLE_COUNT;j++){  XXX remove ACK_SAMPLE_COUNT ?? */
    for(;;){
      int current = analogRead(CURRENT_MONITOR_PIN_PROG);

      /*INTERFACE.print(current-base); INTERFACE.print(".");*/
      c=(current-base)*ACK_SAMPLE_SMOOTHING+c*(1.0-ACK_SAMPLE_SMOOTHING); /* XXX this does not do what the standard says */
      if(upflankFound != 1 ) {
        if (c>ACK_SAMPLE_THRESHOLD) {
	  upflankFound=1;                                  // upflank found, set flag
	  upflankMicros=micros();                          // remember time when we got the upflank
	  /*INTERFACE.print("^");*/
	}
      } else {                                             // upflankFound == 1
        if (searchLowflank && c<ACK_SAMPLE_THRESHOLD) {    // lowflank found
	  lowflankMicros = micros();
	  searchLowflank= 0;
	  acktime = (unsigned long)(lowflankMicros - upflankMicros);
	  /*INTERFACE.print("v"); INTERFACE.print(acktime); INTERFACE.print("v");*/
	  if (acktime < 30000 || acktime > 56000) {        // this is way to wide but let go for now
	    upflankFound = 0;
	    searchLowflank = 1;
	  } else {
	    ackFound = 1;
	    loadPacket(1,resetPacket,2,1);                   // go back to transmitting reset packets
	    oldPacketCounter = packetsTransmitted;           // remember time when we got the Ack, leave loop below later
	  }
	}
      }
      if(ackFound && (unsigned long)(packetsTransmitted - oldPacketCounter) >= 3) { // wait for at least 3 packets after detected Ack
	  /*INTERFACE.print(packetsTransmitted);INTERFACE.print("!");*/
	return 1;                                       // We had an Ack 3 pkt ago, we can leave the detection loop
      }
      if ((unsigned long)(packetsTransmitted - oldPacketCounter) >= 9) { // Timeout: Wait for 3 reset, 5 vrfy and one extra packet time
        loadPacket(1,resetPacket,2,1);         // go back to transmitting reset packets
	/*INTERFACE.print(packetsTransmitted); INTERFACE.print("X");*/
	return ackFound;                              // timeout, maybe no Ack found
      }
    }
    /* should never reach here as there is a for(;;) above */
} // RegisterList::ackdetect(int)
  
///////////////////////////////////////////////////////////////////////////////

/* power up sequence: Check if we need to turn on rail power and if we do */
/* tell caller so that caller can turn off rail power later               */

byte RegisterList::poweron() volatile {
  byte turnoff = 0;
  byte numpackets = 3;                                   // 3 packets default wait
  unsigned long oldPacketCounter;

  if (digitalRead(SIGNAL_ENABLE_PIN_PROG) == LOW) {
    turnoff = 1;
    numpackets = 20;                                     // 20 packets poweron wait
    digitalWrite(SIGNAL_ENABLE_PIN_PROG,HIGH);
  }
  oldPacketCounter=packetsTransmitted;
  loadPacket(1,resetPacket,2,1);
  while ((unsigned long)(packetsTransmitted - oldPacketCounter) < numpackets); // busy wait
  return turnoff;
} // RegisterList::poweron()

///////////////////////////////////////////////////////////////////////////////

int RegisterList::readBaseCurrent() volatile {
  int base=0;
  /* read base current */
  for(int j=0;j<ACK_BASE_COUNT;j++)
    base+=analogRead(CURRENT_MONITOR_PIN_PROG);
  base/=ACK_BASE_COUNT;
  return base;
} // RegisterList::readBaseCurrent()

void RegisterList::readCV(char *s) volatile{
  byte bRead[4];
  int bValue;
  byte d;                            // tmp var for holding ackdetect answer
  int base;                          // measured base current before ack
  int cv, callBack, callBackSub;
  byte turnoff;                      // need to turn off power again

  if(sscanf(s,"%d %d %d",&cv,&callBack,&callBackSub)!=3)          // cv = 1-1024
    return;    
  cv--;                              // actual CV addresses are cv-1 (0-1023)
  
  bRead[0]=0x78+(highByte(cv)&0x03);   // any CV>1023 will become modulus(1024) due to bit-mask of 0x03
  bRead[1]=lowByte(cv);
  
  bValue=0;

  turnoff = poweron();
  base=readBaseCurrent();

  for(int i=0;i<8;i++){                     // check all 8 bits
    bRead[2]=0xE8+i;  

    loadPacket(0,resetPacket,2,3);          // NMRA recommends starting with 3 reset packets
    loadPacket(1,bRead,3,1);                // Start transmitting verify packets (according to NMRA at least 5 )
                                            // but we do it continiously until Ack or timeout
    d = ackdetect(base);
    bitWrite(bValue,i,d);                   // write the found bit into bValue
  }                                         // end loop over bits

  bRead[0]=0x74+(highByte(cv)&0x03);      // set-up to re-verify entire byte
  bRead[2]=bValue;  
  loadPacket(0,resetPacket,2,3);          // NMRA recommends starting with 3 reset packets
  loadPacket(1,bRead,3,1);                // Start transmitting verify packets (according to NMRA at least 5 
                                          // but we do it continiously until Ack or timeout
  d = ackdetect(base);

  if(d==0)    // verify unsuccessful
    bValue=-1;
  INTERFACE.print(F("<r"));
  INTERFACE.print(callBack);
  INTERFACE.print(F("|"));
  INTERFACE.print(callBackSub);
  INTERFACE.print(F("|"));
  INTERFACE.print(cv+1);
  INTERFACE.print(F(" "));
  INTERFACE.print(bValue);
  INTERFACE.print(F(">"));
  if (turnoff)
    digitalWrite(SIGNAL_ENABLE_PIN_PROG,LOW);
        
} // RegisterList::readCV()

///////////////////////////////////////////////////////////////////////////////

void RegisterList::writeCVByte(char *s) volatile{
  byte bWrite[4];
  byte turnoff;
  int bValue;
  byte d;
  int base;
  int cv, callBack, callBackSub;

  if(sscanf(s,"%d %d %d %d",&cv,&bValue,&callBack,&callBackSub)!=4)          // cv = 1-1024
    return;    
  cv--;                              // actual CV addresses are cv-1 (0-1023)

  turnoff = poweron();
  base=readBaseCurrent();
  
  bWrite[0]=0x7C+(highByte(cv)&0x03);   // any CV>1023 will become modulus(1024) due to bit-mask of 0x03
  bWrite[1]=lowByte(cv);
  bWrite[2]=bValue;
  loadPacket(1,bWrite,3,1);
  d = ackdetect(base);

  if (d == 0) { // we did not get ack on the write, try do do a traditional verify
    bWrite[0]=0x74+(highByte(cv)&0x03);   // set-up to re-verify entire byte
    loadPacket(1,bWrite,3,1);
    d = ackdetect(base);
  }

  if(d==0)    // verify unsuccessful
    bValue=-1;

  INTERFACE.print(F("<r"));
  INTERFACE.print(callBack);
  INTERFACE.print(F("|"));
  INTERFACE.print(callBackSub);
  INTERFACE.print(F("|"));
  INTERFACE.print(cv+1);
  INTERFACE.print(F(" "));
  INTERFACE.print(bValue);
  INTERFACE.print(F(">"));
  if (turnoff)
    digitalWrite(SIGNAL_ENABLE_PIN_PROG,LOW);

} // RegisterList::writeCVByte()
  
///////////////////////////////////////////////////////////////////////////////

void RegisterList::writeCVBit(char *s) volatile{
  byte bWrite[4];
  byte turnoff;
  int bNum,bValue;
  byte d;
  int base;
  int cv, callBack, callBackSub;

  if(sscanf(s,"%d %d %d %d %d",&cv,&bNum,&bValue,&callBack,&callBackSub)!=5)          // cv = 1-1024
    return;    
  cv--;                              // actual CV addresses are cv-1 (0-1023)

  turnoff = poweron();
  base=readBaseCurrent();

  bValue=bValue%2;
  bNum=bNum%8;
  bWrite[0]=0x78+(highByte(cv)&0x03);    // any CV>1023 will become modulus(1024) due to bit-mask of 0x03
  bWrite[1]=lowByte(cv);  
  bWrite[2]=0xF0+bValue*8+bNum;
  loadPacket(1,bWrite,3,1);
  d = ackdetect(base);

  if (d == 0) {                          // did not get ack from write and need to verify
  
    bitClear(bWrite[2],4);               // change instruction code from Write Bit to Verify Bit

    loadPacket(1,bWrite,3,1);
    d = ackdetect(base);
  }
    
  if(d==0)    // verify unsuccessful
    bValue=-1;
  
  INTERFACE.print(F("<r"));
  INTERFACE.print(callBack);
  INTERFACE.print(F("|"));
  INTERFACE.print(callBackSub);
  INTERFACE.print(F("|"));
  INTERFACE.print(cv+1);
  INTERFACE.print(F(" "));
  INTERFACE.print(bNum);
  INTERFACE.print(F(" "));
  INTERFACE.print(bValue);
  INTERFACE.print(F(">"));
  if (turnoff)
    digitalWrite(SIGNAL_ENABLE_PIN_PROG,LOW);

} // RegisterList::writeCVBit()
  
///////////////////////////////////////////////////////////////////////////////

void RegisterList::writeCVByteMain(char *s) volatile{
  byte b[6];                      // save space for checksum byte
  int cab;
  int cv;
  int bValue;
  byte nB=0;
  
  if(sscanf(s,"%d %d %d",&cab,&cv,&bValue)!=3)
    return;
  cv--;

  if(cab>127)    
    b[nB++]=highByte(cab) | 0xC0;      // convert train number into a two-byte address
    
  b[nB++]=lowByte(cab);
  b[nB++]=0xEC+(highByte(cv)&0x03);   // any CV>1023 will become modulus(1024) due to bit-mask of 0x03
  b[nB++]=lowByte(cv);
  b[nB++]=bValue;
    
  loadPacket(0,b,nB,4);

} // RegisterList::writeCVByteMain()
  
///////////////////////////////////////////////////////////////////////////////

void RegisterList::writeCVBitMain(char *s) volatile{
  byte b[6];                      // save space for checksum byte
  int cab;
  int cv;
  int bNum;
  int bValue;
  byte nB=0;
  
  if(sscanf(s,"%d %d %d %d",&cab,&cv,&bNum,&bValue)!=4)
    return;
  cv--;
    
  bValue=bValue%2;
  bNum=bNum%8; 

  if(cab>127)    
    b[nB++]=highByte(cab) | 0xC0;      // convert train number into a two-byte address
  
  b[nB++]=lowByte(cab);
  b[nB++]=0xE8+(highByte(cv)&0x03);   // any CV>1023 will become modulus(1024) due to bit-mask of 0x03
  b[nB++]=lowByte(cv);
  b[nB++]=0xF0+bValue*8+bNum;
    
  loadPacket(0,b,nB,4);
  
} // RegisterList::writeCVBitMain()

///////////////////////////////////////////////////////////////////////////////

void RegisterList::printPacket(int nReg, byte *b, int nBytes, int nRepeat) volatile {
  
  INTERFACE.print(F("<*"));
  INTERFACE.print(nReg);
  INTERFACE.print(F(":"));
  for(int i=0;i<nBytes;i++){
    INTERFACE.print(F(" "));
    INTERFACE.print(b[i],HEX);
  }
  INTERFACE.print(F(" / "));
  INTERFACE.print(nRepeat);
  INTERFACE.print(F(">"));
} // RegisterList::printPacket()

///////////////////////////////////////////////////////////////////////////////

void RegisterList::printMaxNumRegs() volatile {
      INTERFACE.print(F("<#"));
      INTERFACE.print(maxNumRegs);
      INTERFACE.print(F(">"));

}
///////////////////////////////////////////////////////////////////////////////

byte RegisterList::idlePacket[3]={0xFF,0x00,0};                 // always leave extra byte for checksum computation
byte RegisterList::resetPacket[3]={0x00,0x00,0};

byte RegisterList::bitMask[]={0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};         // masks used in interrupt routine to speed the query of a single bit in a Packet
