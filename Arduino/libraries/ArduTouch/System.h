/*
    System.h  

    Declarations for the System module

    ---------------------------------------------------------------------------
 
    Copyright (C) 2016, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
   ---------------------------------------------------------------------------
*/

#ifndef SYSTEM_H_INCLUDED
#define SYSTEM_H_INCLUDED

#include "types.h"
#include "Audio.h"
#include "Synth.h"
#include "Model.h"

#define LIBRARY_VERSION "1.16"

/* ------------------------------------------------------------------------- */

// declare the PCB REV of your board by uncommenting one of the lines below:

  #define PCB_REV_C                      
//#define PCB_REV_B                      
//#define PCB_REV_A                      

/* ------------------------------------------------------------------------- */

#define NumButs   2                    // number of onboard buttons 
#define NumPots   2                    // number of onboard pots 
#define NumLEDs   2                    // number of onboard LEDs 
#define NumKeys  12                    // # of onboard keys

/* ------------------------      public vars      -------------------------- */

extern Synth *synth;                   // ptr to the runtime synth 

#ifdef MONITOR_CPU
extern byte  cpu;                      // % CPU used rendering audio (parts per 256)
#endif

/* ----------------------     public functions     ------------------------- */

void ardutouch_info();                 // display ArduTouch library info to console
void ardutouch_loop();                 // perform ongoing ardutouch tasks
void ardutouch_setup( Synth * );       // initialize ardutouch resources
void device_io();                      // manage device I/O
byte readPot( byte );                  // read and return a pot's value

extern void bufStats();                // prints audio buffer stats
extern int  freeRam();                 // returns space between heap & stack

extern void readMetrics( word addrNVS = AUTO_METRICS_ADDR );  // read metrics from NVS
extern void writeMetrics( word addrNVS = AUTO_METRICS_ADDR ); // write metrics to NVS

                  // LEDs

#define RED_LED   0                    // red LED  #
#define BLUE_LED  1                    // blue LED #

#define LEFT_LED  0                    // left LED #
#define RIGHT_LED 1                    // right LED #

#define NULL_LED  255                  // null LED # 

extern void onLED( byte );             // turn an LED on
extern void offLED( byte );            // turn an LED off
extern void blinkLED( byte, bool invert = false ); // blink an LED 
extern byte getBlinkTime();            // return LED blink time
extern void setBlinkTime( byte );      // set LED blink time


                  // Non-Volatile Storage (EEPROM) routines

extern byte readNVS( word addrNVS );                           // read one byte 
extern void readNVS( word addrNVS, void *addrRAM, word size ); // read multiple bytes

extern void writeNVS( word addrNVS, byte value );              // write one byte
extern void writeNVS( word addrNVS, void *addrRAM, word size );// write multiple bytes

/******************************************************************************
 *
 *                   Rand16 (Random 16-bit Number Generator)
 *
 ******************************************************************************/

class Rand16                           // 16-bit Linear FeedBack Shift Register
{
   public:

   Word output;                        // random 16-bit number

   void next();                        // generate next random value
   void reseed();                      // reseed the generator

} ;

#endif   // ifndef SYSTEM_H_INCLUDED
