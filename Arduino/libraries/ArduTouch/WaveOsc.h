/*
    WaveOsc.h  

    Declaration of WaveOsc and derived classes.

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

/******************************************************************************
 *
 *                                  WaveOsc 
 *
 ******************************************************************************/

class WaveOsc : public Osc       // wavetable oscillator
{
   typedef Osc super;            // superclass is Osc

   protected:

   const signed char* table;     // ptr to table of samples
   
   word    length;               // # of samples in table 
   double  period;               // # of samples per wavelength

   DWord   step;                 // amount to increment idx per tick
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
   char   menu( key );           // given a key, return a character 
   #endif

   void   setTable( const desWavTab* );              // set wave table 
   void   setTable( const desWavTab*, const char* ); // set wave table & name
   void   setTableFromBank( byte ); // set table to nth member of wavebank

} ;  

/******************************************************************************
 *
 *                                FastWaveOsc 
 *
 ******************************************************************************/

class FastWaveOsc : public WaveOsc  // speed-optimized wavetable oscillator
{
   private:

   word    aggEnd;              // length - (step * audioBufSz)

   public:

   void   onFreq();              // compute frequency dependent state vars
   void   output( char* );       // write output to a buffer 

} ;  

/******************************************************************************
 *
 *                                 SampleOsc 
 *
 ******************************************************************************/

class SampleOsc : public WaveOsc          // plays a wavetable once, one sample per tick
{
   typedef WaveOsc super;                 // superclass is WaveOsc

   protected:

   word   samples_to_go;                  // # of samples remaining to output
   word   tabptr;                         // current location in sample table

   bool   loFi;                           // sample is "lofi" (half the audio rate)
   bool   oddTick;                        // if loFi, # audio ticks since trigger is odd

   public:

   boolean charEv( char );                // process a character event
   void   onFreq();                       // compute frequency dependent state vars
   void   output( char* );                // write output to a buffer 
   void   setSample( const desWavTab* );  // set wave table 

} ;  

#endif   // ifndef WAVEOSC_H_INCLUDED
