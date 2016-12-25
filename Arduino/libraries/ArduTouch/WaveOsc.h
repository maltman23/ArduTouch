/*
    WaveOsc.h  

    Declaration of the WaveOsc class (wavetable-based oscillator).

    ---------------------------------------------------------------------------
 
    Copyright (C) 2016, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
   ---------------------------------------------------------------------------
*/

#ifndef WAVEOSC_H_INCLUDED
#define WAVEOSC_H_INCLUDED

#include "types.h"
#include "Audio.h"
#include "Osc.h"
#include "WaveTable.h"

class WaveOsc : public Osc
{
   private:

   const signed char* table;     // ptr to table of samples
   
   word    length;               // # of samples in table 
   double  period;               // # of samples per wavelength

   unsigned long step;           // amount to increment idx per tick
                                 // (msw/lsw : integral/fractional parts)
   DWord   idx;                  // current accumulated idx into table
                                 // (msw/lsw : integral/fractional parts)

   double  coeff;                // step = freq * coeff

   const char* name;             // wavetable name 

   byte  evaluate() __attribute__((always_inline)); // interpolate audio val

   public:

   boolean charEv( char );       // process a character event
   void   onFreq();              // compute frequency dependent state vars
   void   output( char* );       // write output to a buffer 

   #ifdef KEYBRD_MENUS
   char   menu( key );           // map key event to character 
   #endif

   void   setTable( const desWavTab* );              // set wave table 
   void   setTable( const desWavTab*, const char* ); // set wave table & name
   void   setTableFromBank( byte ); // set table to nth member of wavebank

} ;  

#endif   // ifndef WAVEOSC_H_INCLUDED
