/*
    NoiseOsc.h  

    Declaration of NoiseOsc and derived classes.

    ---------------------------------------------------------------------------
 
    Copyright (C) 2017, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
   ---------------------------------------------------------------------------
*/

#ifndef NOISEOSC_H_INCLUDED
#define NOISEOSC_H_INCLUDED

#include "types.h"
#include "Osc.h"
#include "System.h"

/******************************************************************************
 *
 *                                NoiseOsc 
 *
 ******************************************************************************/

class NoiseOsc : public Osc                  // template for noise oscillator
{
   typedef Osc super;                        // superclass is Osc

   protected:

   DWord index;                              // current index value, kept as a
                                             // fixed pt, 32-bit number. High byte
                                             // is integral part, lower 3 bytes 
                                             // are the fractional part.

   signed char outVal;                       // current output value

   unsigned long delta;                      // bump index this much per tick
   double        coeff;                      // delta = frequency * coeff

   Rand16 random;                            // 16-bit rabdom number generator

   public:

   NoiseOsc();                               // initialize state vars
   void onFreq();                            // compute freq dependent state vars

   #ifdef CONSOLE_OUTPUT
   const char *prompt();                     // return object's prompt string
   #endif

} ;

/******************************************************************************
 *
 *                               WhiteNoise 
 *
 ******************************************************************************/

class WhiteNoise : public NoiseOsc           // a white noise oscillator
{
   typedef NoiseOsc super;                   // superclass is NoiseOsc

   public:

   void output( char *buf );                 // write one buffer of output

   #ifdef CONSOLE_OUTPUT
   const char *prompt();               // return object's prompt string
   #endif

} ;


#endif   // ifndef NOISEOSC_H_INCLUDED
