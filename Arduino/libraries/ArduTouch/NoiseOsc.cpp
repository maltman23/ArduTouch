/*
    NoiseOsc.cpp  

    Implementation of the NoiseOsc and related classes.

    ---------------------------------------------------------------------------
 
    Copyright (C) 2017, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
   ---------------------------------------------------------------------------
*/

#include "NoiseOsc.h"

/******************************************************************************
 *
 *                                NoiseOsc 
 *
 ******************************************************************************/

/*----------------------------------------------------------------------------*
 *
 *  Name:  NoiseOsc::NoiseOsc
 *
 *  Desc:  WhiteNoise constructor.
 *
 *  Glob:  audioRate        - audio playback rate
 *
 *  Memb: +coeff            - this multiplied by osc frequency gives delta
 *
 *----------------------------------------------------------------------------*/      

NoiseOsc::NoiseOsc()
{
   coeff = pow(2, 24) * (32 / audioRate);
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  NoiseOsc::onFreq
 *
 *  Desc:  Compute frequency-dependent state vars.
 *
 *  Memb:  coeff            - multiply this by osc frequency to get delta
 *         effFreq          - effective frequency (includes internal detuning)
 *         extFactor        - external detuning factor
 *        +delta            - amount to increment index per tick
 *
 *  Note:  onFreq() is a virtual method of Osc, and is automatically called
 *         whenever there is a change in frequency for the oscillator. This change
 *         in frequency can come from one of 3 sources: the oscillator is locally
 *         detuned, the "ideal" frequency of the oscillator is set, or an external
 *         (transient) detuning is applied (such as from an ongoing vibrato).
 *
 *         There are 2 implicit inputs to onFreq(), Osc::effFreq and 
 *         Osc::extFactor. Multiplying these together gives the current 
 *         frequency of the oscillator.
 * 
 *----------------------------------------------------------------------------*/      

void NoiseOsc::onFreq()
{
   delta = effFreq * extFactor * coeff;
}

#ifdef CONSOLE_OUTPUT
const char *NoiseOsc::prompt()                     
{
   return CONSTR("noise");
}
#endif

/******************************************************************************
 *
 *                               WhiteNoise 
 *
 ******************************************************************************/

/*----------------------------------------------------------------------------*
 *
 *  Name:  WhiteNoise::output
 *
 *  Desc:  Write one buffer of output.
 *
 *  Glob:  audioBufSz       - size of system audio buffers
 *
 *  Memb:  outVal           - current output value
 *        +delta            - amount to increment outVal per tick
 *
 *----------------------------------------------------------------------------*/      

void WhiteNoise::output( char *buf )
{
   byte icnt = audioBufSz;                // write this many ticks of output

   byte lastIdx = index._.msw._.msb;
   while ( icnt-- )
   {
      index.val += delta;                 // update index
      if ( index._.msw._.msb != lastIdx )
      {
         random.next();
         outVal = random.output._.lsb;
         lastIdx = index._.msw._.msb;
      }
      *buf++ = outVal;                    
   }
}

#ifdef CONSOLE_OUTPUT
const char *WhiteNoise::prompt()                     
{
   return CONSTR("white");
}
#endif

