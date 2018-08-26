/*
    WaveGen.cpp  

    Implementation of waveform generator classes.

    ---------------------------------------------------------------------------
 
    Copyright (C) 2017, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
   ---------------------------------------------------------------------------
*/

#include "Audio.h"
#include "WaveGen.h"

/******************************************************************************
 *
 *                               WavGen256 
 *
 ******************************************************************************/

/*----------------------------------------------------------------------------*
 *
 *  Name:  WavGen256::WavGen256
 *
 *  Desc:  WavGen256 constructor.
 *
 *  Glob:  audioRate        - audio playback rate
 *
 *  Memb: +coeff            - this multiplied by osc frequency gives delta
 *
 *----------------------------------------------------------------------------*/      

WavGen256::WavGen256()
{
   coeff = pow(2, 24) * (256 / audioRate);
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  WavGen256::onFreq
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

void WavGen256::onFreq()
{
   delta = effFreq * extFactor * coeff;
}

/******************************************************************************
 *
 *                               TabOsc256 
 *
 ******************************************************************************/

/*-------------------------------------------------------------------------*
 *
 *  Name:  TabOsc256::output
 *
 *  Desc:  Write output to an audio buffer.
 *
 *  Args:  buf              - ptr to audio buffer  
 *
 *  Glob:  audioBufSz       - size of system audio buffers
 *
 *  Memb: +index            - current index into wave table 
 *         delta            - amount to increment index per tick
 *
 *-------------------------------------------------------------------------*/      

void TabOsc256::output( char* buf ) 
{
   Int  aft;               // value of sample at table[(integral) idx]
   Int  fore;              // value of sample at table[(integral) idx+1]
   Int  interp;            // interpolated value

   byte icnt = audioBufSz;             // write this many ticks of output
   while ( icnt-- )
   {
      word tabptr = (word )table + index._.msw._.msb;  

      /* ----- interpolate an audio value based on neighboring samples ----- */

      /* fetch aft and fore values from wave table array */

      aft.val  = (char )pgm_read_byte_near( tabptr++ );
      fore.val = (char )pgm_read_byte_near( tabptr );

      /* use high byte of fractional component of idx as a proxy (x parts in 
         255) for the interpolation coeff (this is the distance between the 
         actual index and it's integral component). 
      */

      aft.val   *= 255 - index._.msw._.lsb;
      fore.val  *= index._.msw._.lsb;
      interp.val = aft.val + fore.val;

      *buf++ = interp._.msb;
      index.val += delta;                      
   }
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  TabOsc256::setTable
 *
 *  Desc:  Assign a wave table to the oscillator.
 *
 *  Args:  desc             - addr of wavetable descriptor (in ROM)
 *
 *  Memb:  table            - ptr to table of samples
 *
 *  Note:  The table is assumed to have 256 entries which define a single 
 *         wavelength.
 *
 *----------------------------------------------------------------------------*/      

void TabOsc256::setTable( const desWavTab *d ) 
{
   desWavTab desc;

   // copy descriptor from progmem 

   byte* rombyt = (byte *)d;
   byte* rambyt = (byte *)&desc;

   for ( byte z = 0; z < sizeof( desWavTab ); z++ )
      *rambyt++ = pgm_read_byte_near( rombyt++ );

   // copy table addr entry from descriptor

   table = desc.table;
}

/******************************************************************************
 *
 *                                SawTooth 
 *
 ******************************************************************************/

/*----------------------------------------------------------------------------*
 *
 *  Name:  SawTooth::output
 *
 *  Desc:  Write one buffer of output.
 *
 *  Glob:  audioBufSz       - size of system audio buffers
 *
 *  Memb:  index            - current output value
 *        +delta            - amount to increment index per tick
 *
 *----------------------------------------------------------------------------*/      

void SawTooth::output( char *buf )
{
   byte icnt = audioBufSz;                // write this many ticks of output

   while ( icnt-- )
   {
      index.val += delta;                // update index       
      *buf++ = index._.msw._.msb;        // write integral portion to buffer
   }
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Square::charEv
 *
 *  Desc:  Process a character event.
 *
 *  Args:  code             - character to process
 *
 *  Memb:  lfoPW            - LFO controlling pulse width
 *        +uiPW             - pulse width in U/I {parts/256} 
 *
 *----------------------------------------------------------------------------*/      

boolean Square::charEv( char code )    
{
   switch ( code )
   {
      #ifdef INTERN_CONSOLE

      case 'c':                     // set new cutoff value 
      {
         byte usrInp;
         if ( console.getByte( CONSTR("cutoff"), &usrInp ) )
            setCutoff( usrInp );
         break;
      }
      case 'l':                     // push pw LFO

         console.pushMode( &lfoPW );
         break;

      case 'p':                     // set ideal pulse-width
      {
         byte uiPW;
         if ( console.getByte( CONSTR("pw"), &uiPW ) ) 
            setPW( uiPW );
         break;
      }

      #endif

      #ifdef CONSOLE_OUTPUT

      case chrInfo:                 // display object info to console

         Osc::charEv( chrInfo );
         console.newlntab();
         console.infoByte( CONSTR("cutoff"), cutoff );
         console.infoByte( CONSTR("pw"), idealPW._.msw._.msb );
         lfoPW.brief();
         break;

      #endif

      case '!':                     // perform a reset

         lfoPW.reset();

         setPW( 128 );
         setCutoff( 255 );

         // fall thru to general reset 
            
      default:

         return super::charEv( code );  // pass unhandled events to super class
   }
   return true;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Square::dynamics
 *
 *  Desc:  Perform a dynamic update by updating the pulse width lfo and 
 *         recalculating the instantaneous pulse width.
 *
 *  Memb:  idealPW          - ideal pulse width {(parts per 256)<<8}
 *        +instPW           - instantaneous pulse width
 *         lfoPW            - LFO controlling pulse width
 *
 *----------------------------------------------------------------------------*/

void Square::dynamics()
{
   lfoPW.dynamics();
   fallEdge = idealPW.val * lfoPW.value;
   normDelta   = 254.0 / delta;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Square:evHandler
 *
 *  Desc:  Handle an onboard event.
 *
 *  Args:  ev               - onboard event
 *
 *  Rets:  status           - true if the event was handled
 *
 *----------------------------------------------------------------------------*/      

boolean Square::evHandler( obEvent ev )
{
   switch ( ev.type() )
   {
      case POT0:                          // top pot controls pulse width (0-128)
      {
         word width = ev.getPotVal();     // 0 <= width <= 255
         ++width;                         // 1 <= width <= 256
         width >>= 1;                     // 0 <= width <= 128
         setPW( width );
         break;
      }
      case POT1:                          // bottom pot controls filter cutoff

         setCutoff( ev.getPotVal() );
         return true;                     

      default:

         return super::evHandler(ev);
   }
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Square::output
 *
 *  Desc:  Write one buffer of output.
 *
 *  Glob:  audioBufSz       - size of system audio buffers
 *
 *  Memb:  instPW           - instantaneous pulse width
 *        +index            - current index value
 *         delta            - amount to increment index per tick
 *
 *----------------------------------------------------------------------------*/      

void Square::output( char *buf )
{
   byte icnt = audioBufSz;                // write this many ticks of output
   while ( icnt-- )
   {
      Int b;                              // register for scaling buf value
      Int l;                              // register for scaling last value
      char raw;                           // raw (pre-lowpass) output value

      index.val += delta;
      if ( high )                         // was on high edge
         {
         if ( index.val <= fallEdge )     // remaining on high edge
         {
            raw = 127;
         }
         else // transitioning to low edge
         {
            high = false;
            double pct = 127 - (index.val - fallEdge) * normDelta;
            raw = pct;
         }
      }
      else // low edge
      {
         if ( index.val > fallEdge )      // remaining on low edge
         {
            raw = -127;
         }
         else // transitioning to high edge
         {
            high = true;
            double pct = -127 + index.val * normDelta;
            raw = pct;
         }
      }

      b.val = raw * weight;               // exponentially scale terms
      l.val = last * weight_;             // ""

      l.val += b.val;                     // sum scaled terms 

      if ( l._.lsb & 0b10000000 )         // round scaled sum ...
         ++l._.msb;

      last = l._.msb;
      *buf++ = last;
   }
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Square::setCutoff
 *
 *  Desc:  Set the cutoff coefficient of internal low pass filter.
 *
 *  Args:  c                - cutoff level (n/255)
 *
 *  Memb: +cutoff           - filter cutoff (n/255)
 *
 *----------------------------------------------------------------------------*/      

void Square::setCutoff( byte cutoff )
{
   this->cutoff  = cutoff;
   this->weight  = cutoff + 1;
   this->weight_ = 256 - weight;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Square::setPW
 *
 *  Desc:  Set the ideal pulse width.
 *
 *  Args:  pw               - new pulse width ( 0 - 128 )
 *
 *  Memb: +idealPW          - ideal pulse width 
 *
 *----------------------------------------------------------------------------*/      

void Square::setPW( byte pw )    
{
   if ( pw > 128 )
      pw = 128;

   idealPW.val         = 0;
   idealPW._.msw._.msb = pw;
}

/******************************************************************************
 *
 *                                PwLFO 
 *
 ******************************************************************************/

boolean PwLFO::charEv( char code )    
{
   switch ( code )
   {
      case '!':                     // perform a reset

         super::charEv('!');
         super::charEv('~');        // legato re-trigering
         setDepth( .62 );
         setFreq( .36 );
         break;

       default:

         return super::charEv( code );   
   }
   return true;
}

