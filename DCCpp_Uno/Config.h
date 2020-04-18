/**********************************************************************

Config.h
COPYRIGHT (c) 2013-2016 Gregg E. Berman
              2016-2020 Harald Barth

Part of DCC++ BASE STATION for the Arduino

**********************************************************************/

/////////////////////////////////////////////////////////////////////////////////////
//
// DEFINE MOTOR_SHIELD_TYPE ACCORDING TO THE FOLLOWING TABLE:
//
//  0 = ARDUINO MOTOR SHIELD          (MAX 18V/2A PER CHANNEL)
//  1 = POLOLU MC33926 MOTOR SHIELD   (MAX 28V/3A PER CHANNEL)

#define MOTOR_SHIELD_TYPE   0

/////////////////////////////////////////////////////////////////////////////////////
//
// DEFINE NUMBER OF MAIN TRACK REGISTER

#define MAX_MAIN_REGISTERS 100

/////////////////////////////////////////////////////////////////////////////////////
//
// DEFINE COMMUNICATIONS INTERFACE
//
//  0 = Built-in Serial Port
//  1 = Arduino.cc Ethernet/SD-Card Shield
//  2 = Arduino.org Ethernet/SD-Card Shield
//  3 = Seeed Studio Ethernet/SD-Card Shield W5200

#define COMM_INTERFACE   0

/////////////////////////////////////////////////////////////////////////////////////
//
// DEFINE STATIC IP ADDRESS *OR* COMMENT OUT TO USE DHCP
//

//#define IP_ADDRESS { 192, 168, 1, 200 }

/////////////////////////////////////////////////////////////////////////////////////
//
// DEFINE PORT TO USE FOR ETHERNET COMMUNICATIONS INTERFACE
//

#define ETHERNET_PORT 2560

/////////////////////////////////////////////////////////////////////////////////////
//
// DEFINE MAC ADDRESS ARRAY FOR ETHERNET COMMUNICATIONS INTERFACE
//

#define MAC_ADDRESS {  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEF }

/////////////////////////////////////////////////////////////////////////////////////
//
// USE_TRIGGERPIN: Enable code that switches the trigger pin on and off at end
//                 of the preamble. This takes some clock cycles in the
//                 interrupt routine for the main track.
// USE_TRIGGERPIN_PER_BIT: As above but for every bit. This makes only sense
//                 if USE_TRIGGERPIN is set.
//
// The value of the TRIGGERPIN is defined in DCCpp_Uno.h because it might
// be board specific
//
//#define USE_TRIGGERPIN
//#define USE_TRIGGERPIN_PER_BIT

/////////////////////////////////////////////////////////////////////////////////////
//
// Define only of you need the store to EEPROM feature. This takes RAM and
// you may then use less MAX_MAIN_REGISTERS to compensate (at least on the UNO)
//#define EESTORE
//
// Define only of you need fancy config output in the beginning. This takes RAM and
// you may then use less MAX_MAIN_REGISTERS to compensate (at least on the UNO)
//#define SHOWCONFIG // to preserve SDRAM

/////////////////////////////////////////////////////////////////////////////////////
//
// PREAMBLE_MAIN: Length of the preamble on the main track. Per standard this should
//                be at least 14 bits but if some equipment wants to insert a RailCom
//                cutout this should be at least 16 bits.
// PERAMBLE_PROG: Length of the preamble on the programming track. Per standard this
//                should be at least 22 bits 
//
#define PREAMBLE_MAIN 16
#define PREAMBLE_PROG 22

/////////////////////////////////////////////////////////////////////////////////////
//
// SHORT_SENSITIVITY: Number of short current detections until rail is shut down
// time between tests ist apperox 20ms
// This can be increased at own risk if you have problems with slow auto reversing
// circuits. 1 means shutdown at first over current occurence.
//
#define SHORT_SENSITIVITY 2
/////////////////////////////////////////////////////////////////////////////////////
//
// RAILCOM_CUTOUT: If you want to generate a railcom cutout. Experimental!
//
#define RAILCOM_CUTOUT

