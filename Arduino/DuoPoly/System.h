/*
    Systems.h  

    Declarations for the Systems module

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

#include "Audio.h"
#include "Synth.h"

//#define PCB_REV_1                      // comment out for prototype boards

#define RED_LED   0                    // red LED  #
#define BLUE_LED  1                    // blue LED #

#define LEFT_LED  0                    // left LED #
#define RIGHT_LED 1                    // right LED #

/* ----------------------     public functions     ------------------------- */

void device_io();                      // manage device I/O
void system_loop();                    // perform ongoing system tasks
void system_setup( Synth * );          // initialize system resources

namespace system 
{
   extern void bumpOctave( char );     // bump keyboard's octave # up/down 
   extern byte getOctave();            // return keyboard's octave #
   extern void setOctave( byte );      // set keyboard's octave #

   extern void onLED( byte );          // turn an LED on
   extern void offLED( byte );         // turn an LED off
   extern void blinkLED( byte );       // blink an LED 
   extern byte getBlinkRate();         // return LED blink rate
   extern void setBlinkRate( byte );   // set LED blink rate
}

#endif   // ifndef SYSTEM_H_INCLUDED
