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

   DWord   step;                 // amount to increment idx per tick
                                 // (msw/lsw : integral/fractional parts)
   DWord   idx;                  // current accumulated idx into table
                                 // (msw/lsw : integral/fractional parts)

   double  coeff;                // step = freq * coeff

   const char* name;             // wavetable name 

   byte  evaluate() __attribute__((always_inline)); // interpolate audio val

   public:

   bool   charEv( char );        // process a character event
   double getPeriod();           // returns number of samples per wavelength
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

#define SAMPLE_UNIT_STEP 65536            // step.val of "1.0"

class SampleOsc : public WaveOsc          // plays a wavetable once
{
   typedef WaveOsc super;                 // superclass is WaveOsc

   protected:

   word  ticks_to_go;                     // # of ticks of output remaining
   bool  lofi;                            // if true, sample is lofi

   public:

   bool  charEv( char );                  // process a character event
   void  onFreq();                        // compute frequency dependent state vars
   void  output( char* );                 // write output to a buffer 
   void  setSample( const desWavTab* );   // set wave table 

} ;  

/******************************************************************************
 *
 *                               ModSampleOsc
 *
 *  ModSampleOsc is a SampleOsc which can be played forwards or backwards
 *  and whose duration can be stretched (by powers of 2).
 *
 ******************************************************************************/

class ModSampleOsc : public SampleOsc     // SampleOsc (modifiable playback)
{
   typedef SampleOsc super;               // superclass is WaveOsc

   protected:

   bool forwards;                         // if true, play sample forwards
   char shiftStep;                        // left-shift step this many bits

   public:

   bool charEv( char );                   // process a character event
   void backward();                       // set playback direction to backward
   void flip();                           // flip the direction of playback 
   void forward();                        // set playback direction to forward
   void stretch( int );                   // stretch playback duration (by power of 2)

   PROMPT_STR( modsampl ) 
} ;  

/******************************************************************************
 *
 *                             TunedSampleOsc
 *
 *  TunedSampleOsc is a WaveOsc whose wave table is played only once when 
 *  triggered (instead of looping continuously). Unlike a simple SampleOsc, 
 *  the sound represented by the wavetable for a TunedSampleOsc is considered  
 *  to be tonal, to have a definite wavelength, and so can be played back at 
 *  specific musical pitches like a normal oscillator.
 *
 ******************************************************************************/

class TunedSampleOsc : public WaveOsc     // plays a wavetable once
{
   typedef WaveOsc super;                 // superclass is WaveOsc

   protected:

   public:

   bool  charEv( char );                  // process a character event
   void  normalize( double );             // normalize step to a given freq
   void  output( char* );                 // write output to a buffer 
   void  setSample( const desWavTab* );   // set wave table 

   PROMPT_STR( tunsampl ) 
} ;  

#endif   // ifndef WAVEOSC_H_INCLUDED
