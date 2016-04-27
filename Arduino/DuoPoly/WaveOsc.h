/*
    WaveOsc.h  

    Declaration of the WaveOsc class, along with Tremolo and Vibrato classes.

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
#include "Phonic.h"
#include "LFO.h"

class Tremolo : public TrigLFO
{
   public:

   void   charEv( char );        // process a character event
   void   evaluate();            // set output val (as a function of LFO pos)
   char*  prompt();              // return object's prompt string

} ;

class Vibrato : public FadeLFO
{
   public:

   void   evaluate();            // set output val (as a function of LFO pos)
   void   iniPos();              // set initial osc position
   char*  prompt();              // return object's prompt string

} ;

class WaveOsc : public MonoPhonic
{
   protected:

   Tremolo       tremolo;        // built-in tremolo effect
   Vibrato       vibrato;        // built-in vibrato effect

   private:

   const signed char* table;     // ptr to table of waveform pts
   
   double        freq;           // ideal (unmodulated) playback freq
   word          length;         // # of samples in table 
   double        period;         // # of samples per wavelen

   unsigned long step;           // amount to increment idx per tick
                                 // (msw/lsw : integral/fractional parts)
   DWord         idx;            // current accumulated idx into table
                                 // (msw/lsw : integral/fractional parts)

   double        coeff;          // step = freq * coeff

   double        vol;            // volume level (0.0 - 1.0)
   Word          instVol;        // instantaneous volume (msb: integral part,
                                 //                       lsb: fractional part)
                                 // (pre-divided by 2 for use in evaluate())

   const char*   name;           // name (for Console display)

   public:

   /*-------------------------------------------------------------------------*
    *
    *  Name:  evaluate
    *
    *  Desc:  Interpolate an audio value at the wave table index.
    *
    *  Args:  -- none --
    *
    *  Rets:  value            - interpolated value
    *
    *  Memb:  idx              - accumulated index into wave table 
    *         instVol          - instantaneous volume
    *         table            - ptr to wave table 
    *
    *  Note:  Linear interpolation is used.
    *
    *-------------------------------------------------------------------------*/      

   byte evaluate() __attribute__((always_inline))
   {
      Int  aft;               // value of sample at table[(integral) idx]
      Int  fore;              // value of sample at table[(integral) idx+1]
      Int  interp;            // interpolated value
      Word icoeff;            // interpolation coefficient

      word tabptr = (word )table + idx._.msw.val;  

      /* ----- interpolate an audio value based on neighboring samples ----- */

      /* fetch aft and fore values from wave table array */

      aft.val  = (char )pgm_read_byte_near( tabptr++ );
      fore.val = (char )pgm_read_byte_near( tabptr );

      /* use high byte of fractional component of idx as a proxy (x parts in 
         255) for the interpolation coeff (this is the distance between the 
         actual index and it's integral component). Pre-multiply coefficient
         by the instantaneous volume (this saves a separate multiply per term 
         after interpolation). Also, note that instVol is by definition pre-
         divided by 2 which saves halving the sum of aft + fore when returning 
         the interpolated value :)
      */

      icoeff.val = idx._.lsw._.msb * instVol._.lsb;
      aft.val   *= instVol._.lsb - icoeff._.msb;
      fore.val  *= icoeff._.msb;
      interp.val = aft.val + fore.val;

      if ( interp._.lsb & 0x80 )    // round interpolated value
         ++interp._.msb;

      return interp._.msb;    
   }

   /*-------------------------------------------------------------------------*
    *
    *  Name:  WaveOsc::generate
    *
    *  Desc:  Generate output to an audio buffer.
    *
    *  Args:  buf              - ptr to audio buffer  
    *
    *  Glob:  audio::bufSz     - size of system audio buffers
    *
    *  Memb: +idx              - accumulated idx into wave table 
    *         length           - # of samples in wave table 
    *         step             - amount to increment idx per tick
    *
    *-------------------------------------------------------------------------*/      

   void generate( char *buf ) __attribute__((always_inline))
   {
      byte icnt = audio::bufSz;     // generate this many ticks of output

      while ( icnt-- )
      {
         idx.val += step;                      
         if ( idx._.msw.val >= length )
            idx._.msw.val -= length;
   
         *buf++ = evaluate();
      }
   }

   void    butEv( but );         // handle a button event
   void    charEv( char );       // process a character event
   void    dynamics();           // update dynamics
   double  getFreq();            // get the playback frequency
   void    info();               // display object info to console
   void    printNameFreq();      // display name, freq to console
   void    setFreq( double );    // set the playback frequency
   void    setVol( double );     // set the volume level
   void    setWave( byte );      // set wave table 

} ;  

#endif   // ifndef WAVEOSC_H_INCLUDED
