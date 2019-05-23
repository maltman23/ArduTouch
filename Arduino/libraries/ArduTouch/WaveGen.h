/*
    WaveGen.h  

    Declaration of waveform generator classes.

    ---------------------------------------------------------------------------
 
    Copyright (C) 2016, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
   ---------------------------------------------------------------------------
*/

#ifndef WAVEGEN_H_INCLUDED
#define WAVEGEN_H_INCLUDED

#include "types.h"
#include "LFO.h"
#include "Osc.h"
#include "WaveTable.h"
#include "WaveBank.h"

/******************************************************************************
 *
 *                               WavGen256 
 *
 ******************************************************************************/

class WavGen256 : public Osc                 // abstract wave length generator 
                                             // with a period of 256
{
   public:

   DWord index;                              // current index value, kept as a
                                             // fixed pt, 32-bit number. High byte
                                             // is integral part, lower 3 bytes 
                                             // are the fractional part.

   unsigned long delta;                      // bump index this much per tick

   WavGen256();                              // initialize state vars

   void onFreq();                            // compute freq dependent state vars

   protected:

   double        coeff;                      // delta = frequency * coeff

} ;

/******************************************************************************
 *
 *                               TabOsc256 
 *
 ******************************************************************************/

class TabOsc256 : public WavGen256           // wave length specified by a table  
                                             // of 256 samples in ROM
{
   typedef WavGen256 super;                  // superclass is WavGen256

   public:

   void   output( char* );                   // write output to a buffer 
   void   setTable( const desWavTab* );      // assign a wave table to oscillator

   protected:

   const signed char* table;                 // ptr to table of samples

} ;


/******************************************************************************
 *
 *                                  Sine 
 *
 ******************************************************************************/

class Sine : public TabOsc256             
{
   typedef TabOsc256 super;                  // superclass is TabOsc256

   public:

   Sine()
   {
      setTable( wavetable( Sine ) );         // the Sine table has 256 samples
   }

} ;


/******************************************************************************
 *
 *                                 SawTooth 
 *
 ******************************************************************************/

class SawTooth : public WavGen256            // a sawtooth oscillator
{
   public:

   void output( char *buf );                 // write one buffer of output

} ;

/******************************************************************************
 *
 *                                PwLFO 
 *
 ******************************************************************************/

class PwLFO : public LFO                     // pulse-width modulator
{
   typedef  LFO super;

   public:

   boolean charEv( char code );              // handle a character event

   PROMPT_STR( pwLFO ) 

} ;

/******************************************************************************
 *
 *                                Square 
 *
 ******************************************************************************/

class Square : public WavGen256              // a square-wave oscillator
{
   typedef  WavGen256 super;

   DWord    idealPW;                         // ideal pulse width
   unsigned long fallEdge;                   // index of falling edge 

   double  normDelta;                        // 254.0 / delta;

   boolean  high;                            // if true, currently on high edge

   char    last;                             // last output value   

   byte    cutoff;                           // lowpass cutoff level (n/255) 

   word    weight;                           // weight for input (n/256) 
   word    weight_;                          // weight for avg (n/256) 

   public:

   PwLFO   lfoPW;                            // LFO controlling pulse width

   boolean charEv( char code );              // handle a character event
   void    dynamics();                       // update dynamics
   boolean evHandler( obEvent e );           // handle an onboard event
   void    output( char *buf );              // write one buffer of output
   void    setCutoff( byte );                // set cuttoff coef 
   void    setPW( byte pw );                 // set the ideal pulse width 
} ;

#endif   // ifndef WAVEGEN_H_INCLUDED
