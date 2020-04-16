/* 
 * digitalWritefast.h
 * 
 * Copyright (C) 2011-2012 reaDIYmate Pierre Bouchet
 * Copyright (C) 2020      Harald Barth
 *
 * This file was part of the reaDIYmate library.
 *
 * This library is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef DIGITAL_WRITE_FAST_H
#define DIGITAL_WRITE_FAST_H
/**
 * This is a custom version of DigitalWriteFast
 * Macros to use digitalWrite style syntax with most of the speed of PORT
 *
 * This library consists of a complex header file that translates
 * digitalWriteFast, pinModeFast, digitalReadFast into the corresponding PORT
 * commands.
 *
 * It provides syntax that is as novice-friendly as the arduino's pin
 * manipulation commands but an order of magnitude faster.
 *
 * It can speed things up when the pin number is known at compile time, so that
 * digitalWriteFast(9,HIGH); is speeded up. On the other hand a loop with
 * digitalWriteFast(i,HIGH); or a called function with the pin number as a passed
 * argument will throw a compiler error. This is intentional.
 *
 */
#include <Arduino.h>
//------------------------------------------------------------------------------
// bit operations
#define BIT_READ(value, bit) (((value) >> (bit)) & 0x01)
#define BIT_SET(value, bit) ((value) |= (1UL << (bit)))
#define BIT_CLEAR(value, bit) ((value) &= ~(1UL << (bit)))
#define BIT_WRITE(value, bit, bitvalue) \
(bitvalue ? BIT_SET(value, bit) : \BIT_CLEAR(value, bit))
//------------------------------------------------------------------------------
#if !defined(digitalPinToPortReg)
//------------------------------------------------------------------------------
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
// ATmega1280, ATmega2560
#define digitalPinToPortReg(P) \
(((P) >= 22 && (P) <= 29) ? &PORTA : \
((((P) >= 10 && (P) <= 13) || ((P) >= 50 && (P) <= 53)) ? &PORTB : \
(((P) >= 30 && (P) <= 37) ? &PORTC : \
((((P) >= 77 && (P) <= 79) || ((P) >= 18 && (P) <= 21) || (P) == 38) ? &PORTD : \
((((P) >= 0 && (P) <= 3) || (P) == 5 || (P) == 70) ? &PORTE : \
(((P) >= 54 && (P) <= 61) ? &PORTF : \
((((P) >= 39 && (P) <= 41) || (P) == 4) ? &PORTG : \
((((P) >= 6 && (P) <= 9) || (P) == 16 || (P) == 17) ? &PORTH : \
((((P) >= 71 && (P) <= 76) || (P) == 14 || (P) == 15) ? &PORTJ : \
(((P) >= 62 && (P) <= 69) ? &PORTK : &PORTL))))))))))
#define digitalPinToDDRReg(P) \
(((P) >= 22 && (P) <= 29) ? &DDRA : \
((((P) >= 10 && (P) <= 13) || ((P) >= 50 && (P) <= 53)) ? &DDRB : \
(((P) >= 30 && (P) <= 37) ? &DDRC : \
((((P) >= 77 && (P) <= 79) || ((P) >= 18 && (P) <= 21) || (P) == 38) ? &DDRD : \
((((P) >= 0 && (P) <= 3) || (P) == 5 || (P) == 70) ? &DDRE : \
(((P) >= 54 && (P) <= 61) ? &DDRF : \
((((P) >= 39 && (P) <= 41) || (P) == 4) ? &DDRG : \
((((P) >= 6 && (P) <= 9) || (P) == 16 || (P) == 17) ? &DDRH : \
((((P) >= 71 && (P) <= 76) || (P) == 14 || (P) == 15) ? &DDRJ : \
(((P) >= 62 && (P) <= 69) ? &DDRK : &DDRL))))))))))
#define digitalPinToPINReg(P) \
(((P) >= 22 && (P) <= 29) ? &PINA : \
((((P) >= 10 && (P) <= 13) || ((P) >= 50 && (P) <= 53)) ? &PINB : \
(((P) >= 30 && (P) <= 37) ? &PINC : \
((((P) >= 77 && (P) <= 79) || ((P) >= 18 && (P) <= 21) || (P) == 38) ? &PIND : \
((((P) >= 0 && (P) <= 3) || (P) == 5 || (P) == 70) ? &PINE : \
(((P) >= 54 && (P) <= 61) ? &PINF : \
((((P) >= 39 && (P) <= 41) || (P) == 4) ? &PING : \
((((P) >= 6 && (P) <= 9) || (P) == 16 || (P) == 17) ? &PINH : \
((((P) >= 71 && (P) <= 76) || (P) == 14 || (P) == 15) ? &PINJ : \
(((P) >= 62 && (P) <= 69) ? &PINK : &PINL))))))))))
#define __digitalPinToBit(P) \
(((P) >=  7 && (P) <=  9) ? (P) - 3 : \
(((P) >= 10 && (P) <= 13) ? (P) - 6 : \
(((P) >= 22 && (P) <= 29) ? (P) - 22 : \
(((P) >= 30 && (P) <= 37) ? 37 - (P) : \
(((P) >= 39 && (P) <= 41) ? 41 - (P) : \
(((P) >= 42 && (P) <= 49) ? 49 - (P) : \
(((P) >= 50 && (P) <= 53) ? 53 - (P) : \
(((P) >= 54 && (P) <= 61) ? (P) - 54 : \
(((P) >= 62 && (P) <= 69) ? (P) - 62 : \
(((P) >= 71 && (P) <= 76) ? (P) - 69 : \
(((P) == 0 || (P) == 15 || (P) == 17 || (P) == 21) ? 0 : \
(((P) == 1 || (P) == 14 || (P) == 16 || (P) == 20) ? 1 : \
(((P) == 19) ? 2 : \
(((P) == 5 || (P) == 6 || (P) == 18) ? 3 : \
(((P) == 2 || (P) == 79) ? 4 : \
(((P) == 3 || (P) == 4 || (P) == 78) ? 5 : \
(((P) == 70 || (P) == 77) ? 6 : 7)))))))))))))))))
#define __digitalPinToTimer(P) \
(((P) == 13 || (P) ==  4) ? &TCCR0A : \
(((P) == 11 || (P) == 12) ? &TCCR1A : \
(((P) == 10 || (P) ==  9) ? &TCCR2A : \
(((P) ==  5 || (P) ==  2 || (P) ==  3) ? &TCCR3A : \
(((P) ==  6 || (P) ==  7 || (P) ==  8) ? &TCCR4A : \
(((P) == 46 || (P) == 45 || (P) == 44) ? &TCCR5A : 0))))))
#define __digitalPinToTimerBit(P) \
(((P) == 13) ? COM0A1 : (((P) ==  4) ? COM0B1 : \
(((P) == 11) ? COM1A1 : (((P) == 12) ? COM1B1 : \
(((P) == 10) ? COM2A1 : (((P) ==  9) ? COM2B1 : \
(((P) ==  5) ? COM3A1 : (((P) ==  2) ? COM3B1 : (((P) ==  3) ? COM3C1 : \
(((P) ==  6) ? COM4A1 : (((P) ==  7) ? COM4B1 : (((P) ==  8) ? COM4C1 : \
(((P) == 46) ? COM5A1 : (((P) == 45) ? COM5B1 : COM5C1))))))))))))))
//------------------------------------------------------------------------------
#else	// ATmega8, ATmega168, ATmega328
#define digitalPinToPortReg(P) \
(((P) >= 0 && (P) <= 7) ? &PORTD : (((P) >= 8 && (P) <= 13) ? &PORTB : &PORTC))
#define digitalPinToDDRReg(P) \
(((P) >= 0 && (P) <= 7) ? &DDRD : (((P) >= 8 && (P) <= 13) ? &DDRB : &DDRC))
#define digitalPinToPINReg(P) \
(((P) >= 0 && (P) <= 7) ? &PIND : (((P) >= 8 && (P) <= 13) ? &PINB : &PINC))
#define __digitalPinToBit(P) \
(((P) >= 0 && (P) <= 7) ? (P) : (((P) >= 8 && (P) <= 13) ? (P) - 8 : (P) - 14))
//------------------------------------------------------------------------------
#if defined(__AVR_ATmega8__) // ATmega8
#define __digitalPinToTimer(P) \
(((P) ==  9 || (P) == 10) ? &TCCR1A : (((P) == 11) ? &TCCR2 : 0))
#define __digitalPinToTimerBit(P) \
(((P) ==  9) ? COM1A1 : (((P) == 10) ? COM1B1 : COM21))
//------------------------------------------------------------------------------
#else // ATmega168, ATmega328
#define __digitalPinToTimer(P) \
(((P) ==  6 || (P) ==  5) ? &TCCR0A : \
(((P) ==  9 || (P) == 10) ? &TCCR1A : \
(((P) == 11 || (P) ==  3) ? &TCCR2A : 0)))
#define __digitalPinToTimerBit(P) \
(((P) ==  6) ? COM0A1 : (((P) ==  5) ? COM0B1 : \
(((P) ==  9) ? COM1A1 : (((P) == 10) ? COM1B1 : \
(((P) == 11) ? COM2A1 : COM2B1)))))
#endif  // ATmega8
#endif  // defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
#endif  // !defined(digitalPinToPortReg)
//------------------------------------------------------------------------------
#define __atomicWrite__(A,P,V) \
if ( (int)(A) < 0x40) { \
bitWrite(*((volatile uint8_t*) A), __digitalPinToBit(P), (V) );} \
else { \
uint8_t register saveSreg = SREG; \
cli(); \
bitWrite(*((volatile uint8_t*)A), __digitalPinToBit(P), (V) ); \
SREG=saveSreg; \
}
//------------------------------------------------------------------------------

void NonConstantUsed( void )  __attribute__ (( error("") )); 

#ifndef digitalWriteFast
#define digitalWriteFast(P, V) \
do { \
if (__builtin_constant_p(P) && __builtin_constant_p(V)) \
__atomicWrite__(digitalPinToPortReg(P),P,V) \
else  NonConstantUsed();	\
} while (0)
#endif
//------------------------------------------------------------------------------
#if !defined(pinModeFast)
#define pinModeFast(P, V) \
do { \
if (__builtin_constant_p(P) && __builtin_constant_p(V)) \
__atomicWrite__(digitalPinToDDRReg(P),P,V) \
else NonConstantUsed(); \
} while (0)
#endif
//------------------------------------------------------------------------------
#ifndef noAnalogWrite
#define noAnalogWrite(P) \
do {if (__builtin_constant_p(P) )  __atomicWrite(__digitalPinToTimer(P),P,0) \
else NonConstantUsed(); \
} while (0)
#endif
//------------------------------------------------------------------------------
#ifndef digitalReadFast
#define digitalReadFast(P) ( (int) _digitalReadFast_((P)) )
#define _digitalReadFast_(P ) \
(__builtin_constant_p(P) ) ? ( \
( BIT_READ(*digitalPinToPINReg(P), __digitalPinToBit(P))) ) : \
NonConstantUsed())
#endif

#endif // DIGITAL_WRITE_FAST_H
