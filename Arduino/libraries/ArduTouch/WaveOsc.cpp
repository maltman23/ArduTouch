/*
    WaveOsc.cpp  

    Implementation of the WaveOsc class (wavetable-based oscillator).

    ---------------------------------------------------------------------------
 
    Copyright (C) 2016, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
   ---------------------------------------------------------------------------
*/

#include "WaveOsc.h"
#include "WaveBank.h"

/******************************************************************************
 *
 *                                  WaveOsc 
 *
 ******************************************************************************/

/*----------------------------------------------------------------------------*
 *
 *  Name:  WaveOsc::charEv
 *
 *  Desc:  Process a character event.
 *
 *  Args:  code             - character to process
 *
 *  Memb:  name             - wavetable name
 *  
 *  Glob:  wavebank         - bank of wave tables.
 *
 *  Rets:  status           - true if character was handled
 *
 *----------------------------------------------------------------------------*/      

boolean WaveOsc::charEv( char code )
{
   switch ( code )
   {
      #ifdef INTERN_CONSOLE

      case 'w':                  // select a waveform

         if ( wavebank.choose() )
            setTableFromBank( wavebank.choice() );
         break;

      #endif

      #ifdef CONSOLE_OUTPUT

      case chrInfo:              // display object info to console
      case chrInLnfo:            // display object info inline to console

         super::charEv( code );
         if ( name )
            console.infoStr( CONSTR("waveform"), name );
         break;

      #endif

      default:

         return super::charEv( code );
   }
   return true;
}

/*-------------------------------------------------------------------------*
 *
 *  Name:  WaveOsc::evaluate
 *
 *  Desc:  Interpolate an audio value at the wave table index.
 *
 *  Args:  -- none --
 *
 *  Rets:  value            - interpolated value
 *
 *  Memb:  idx              - accumulated index into wave table 
 *         table            - ptr to wave table 
 *
 *  Note:  Linear interpolation is used.
 *
 *-------------------------------------------------------------------------*/      

byte WaveOsc::evaluate() 
{
   Int  aft;               // value of sample at table[(integral) idx]
   Int  fore;              // value of sample at table[(integral) idx+1]
   Int  interp;            // interpolated value

   word tabptr = (word )table + idx._.msw.val;  

   /* ----- interpolate an audio value based on neighboring samples ----- */

   /* fetch aft and fore values from wave table array */

   aft.val  = (char )pgm_read_byte_near( tabptr++ );
   fore.val = (char )pgm_read_byte_near( tabptr );

   /* use high byte of fractional component of idx as a proxy (x parts in 
      255) for the interpolation coeff (this is the distance between the 
      actual index and it's integral component). 
   */

   aft.val   *= 255 - idx._.lsw._.msb;
   fore.val  *= idx._.lsw._.msb;
   interp.val = aft.val + fore.val;

   if ( interp._.lsb & 0x80 )    // round interpolated value
      ++interp._.msb;

   return interp._.msb;    
}

#ifdef KEYBRD_MENUS

/*----------------------------------------------------------------------------*
 *
 *  Name:  WaveOsc::menu
 *
 *  Desc:  Given a key, return a character (to be processed via charEv()). 
 *
 *  Args:  k                - key
 *
 *  Rets:  c                - character (0 means "no character")
 *
 *  Note:  If a sketch is compiled with KEYBRD_MENUS defined, then this method 
 *         can be used to map the onboard keys to characters which the system 
 *         will automatically feed to the charEv() method.
 *
 *         This method is only called by the system if the MENU flag in this
 *         object is set (in the ::flags byte inherited from Mode), or if the
 *         keyboard is in a "oneShot menu selection" state.
 *
 *----------------------------------------------------------------------------*/      

char WaveOsc::menu( key k )
{
   switch ( k.position() )
   {
      case  2: return 'w';
      default: return super::menu( k );
   }
}
#endif

/*----------------------------------------------------------------------------*
 *
 *  Name:  WaveOsc::onFreq
 *
 *  Desc:  Compute frequency-dependent state vars.
 *
 *  Memb:  coeff            - step = frequency * coeff
 *         effFreq          - effective frequency (includes internal detuning)
 *         extFactor        - external detuning factor
 *        +step             - amount to increment idx per tick
 *
 *----------------------------------------------------------------------------*/      

void WaveOsc::onFreq()
{
   step.val = effFreq * extFactor * coeff;
}

/*-------------------------------------------------------------------------*
 *
 *  Name:  WaveOsc::output
 *
 *  Desc:  Write output to an audio buffer.
 *
 *  Args:  buf              - ptr to audio buffer  
 *
 *  Glob:  audioBufSz       - size of system audio buffers
 *
 *  Memb: +idx              - accumulated idx into wave table 
 *         length           - # of samples in wave table 
 *         step             - amount to increment idx per tick
 *
 *-------------------------------------------------------------------------*/      

void WaveOsc::output( char *buf ) 
{
   byte icnt = audioBufSz;       // write this many ticks of output

   while ( icnt-- )
   {
      idx.val += step.val;                      
      if ( idx._.msw.val >= length )
         idx._.msw.val -= length;
   
      *buf++ = evaluate();
   }
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  WaveOsc::setTable
 *
 *  Desc:  Set the wavetable.
 *
 *  Args:  descriptor       - wavetable descriptor
 *
 *----------------------------------------------------------------------------*/      

void WaveOsc::setTable( const desWavTab *d ) 
{
   setTable( d, NULL );
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  WaveOsc::setTable
 *
 *  Desc:  Set the wavetable.
 *
 *  Args:  descriptor       - wavetable descriptor
 *         name             - wavetable name
 *
 *  Memb: +coeff            - step = freq * coeff
 *        +idx              - current accumulated idx into table
 *        +length           - # of samples in table 
 *        +name             - wacetable name
 *        +period           - # of samples per wavelength
 *        +table            - ptr to table of samples
 * 
 *----------------------------------------------------------------------------*/      

void WaveOsc::setTable( const desWavTab *d, const char *n ) 
{
   desWavTab desc;

   /* copy descriptor from progmem */

   byte* rombyt = (byte *)d;
   byte* rambyt = (byte *)&desc;

   for ( byte z = 0; z < sizeof( desWavTab ); z++ )
      *rambyt++ = pgm_read_byte_near( rombyt++ );

   /* set local fields */

   table   = desc.table;
   period  = desc.period;
   length  = desc.length;

   name    = n;
   idx.val = 0;

   /* compute dependent fields */

   coeff   = pow(2, 16) * (period / audioRate);
   onFreq();
}


/*----------------------------------------------------------------------------*
 *
 *  Name:  WaveOsc::setTable
 *
 *  Desc:  Set table to a given member of the wavebank.
 *
 *  Args:  nth              - index of wavebank member
 *
 *  Glob:  wavebank         - bank of wave tables.
 *
 *----------------------------------------------------------------------------*/      

void WaveOsc::setTableFromBank( byte i ) 
{
   setTable( (const desWavTab* )wavebank.dataPtr(i), wavebank.name(i) );
}

/******************************************************************************
 *
 *                                FastWaveOsc 
 *
 ******************************************************************************/

/*----------------------------------------------------------------------------*
 *
 *  Name:  FastWaveOsc::onFreq
 *
 *  Desc:  Compute frequency-dependent state vars.
 *
 *  Args:  factor           - factor to apply
 *
 *  Glob:  audioBufSz       - size of system audio buffers
 *
 *  Memb: +aggEnd           - length - (step * audioBufSz)
 *         coeff            - step = frequency * coeff
 *         effFreq          - effective frequency (includes internal detuning)
 *         extFactor        - external detuning factor
 *        +step             - amount to increment idx per tick
 *
 *----------------------------------------------------------------------------*/      

void FastWaveOsc::onFreq()
{
   step.val = effFreq * extFactor * coeff;

   DWord longAggStep;

   longAggStep.val  = step.val;
   longAggStep.val *= audioBufSz;
   aggEnd = length - longAggStep._.msw.val - 1;
}

/*-------------------------------------------------------------------------*
 *
 *  Name:  FastWaveOsc::output
 *
 *  Desc:  Write output to an audio buffer.
 *
 *  Args:  buf              - ptr to audio buffer  
 *
 *  Glob:  audioBufSz       - size of system audio buffers
 *
 *  Memb:  aggEnd           - length - (step * audioBufSz)
 *        +idx              - accumulated idx into wave table 
 *         length           - # of samples in wave table 
 *         step             - amount to increment idx per tick
 *
 *-------------------------------------------------------------------------*/      

void FastWaveOsc::output( char *buf ) 
{
   byte icnt = audioBufSz;       // write this many ticks of output

   if ( idx._.msw.val >= aggEnd )
   {
      while ( icnt-- )
      {
         idx.val += step.val;                      
         if ( idx._.msw.val >= length )
            idx._.msw.val -= length;
         *buf++ = evaluate();
      }
   }
   else
   {
      while ( icnt-- )
      {
         idx.val += step.val;                      
         *buf++ = evaluate();
      }
   }

}

/******************************************************************************
 *
 *                                 SampleOsc 
 *
 ******************************************************************************/

/*----------------------------------------------------------------------------*
 *
 *  Name:  SampleOsc::charEv
 *
 *  Desc:  Process a character event.
 *
 *  Args:  code             - character to process
 *
 *  Memb: +flags.DONE       - output of wavetable has been completed
 *         name             - wavetable name
 *        +samples_to_go    - # of samples remaining to output
 *        +tabptr           - current location in sample table
 *  
 *  Glob:  wavebank         - bank of wave tables.
 *
 *  Rets:  status           - true if character was handled
 *
 *----------------------------------------------------------------------------*/      

boolean SampleOsc::charEv( char key )
{
   switch ( key )
   {
      case chrTrigger:                    // trigger the sample

         flags &= ~DONE;
         samples_to_go = length;
         tabptr  = (word )table;
         oddTick = false;
         break;

      case '!':                           // reset

         super::charEv( key );
         samples_to_go = 0;
         setMute( false );
         break;

      default:

         return super::charEv( key );
   }
   return true;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  SampleOsc::onFreq
 *
 *  Desc:  Compute frequency-dependent state vars.
 *
 *  Args:  factor           - factor to apply
 *
 *----------------------------------------------------------------------------*/      

void SampleOsc::onFreq()
{
}

/*-------------------------------------------------------------------------*
 *
 *  Name:  SampleOsc::output
 *
 *  Desc:  Write output to an audio buffer.
 *
 *  Args:  buf              - ptr to audio buffer  
 *
 *  Glob:  audioBufSz       - size of system audio buffers
 *
 *  Memb:  aggEnd           - length - audioBufSz
 *        +idx              - accumulated idx into wave table 
 *         length           - # of samples in wave table 
 *        +samples_to_go    - # of samples remaining to output
 *         step             - amount to increment idx per tick
 *        +tabptr           - current location in sample table
 *
 *-------------------------------------------------------------------------*/      

void SampleOsc::output( char *buf ) 
{
   byte icnt   = audioBufSz;       // write this many ticks of output

   if ( loFi )                     // play 1 sample for 2 audio ticks
   {
      while ( icnt-- )
      {
         if ( samples_to_go )                      
         {
            *buf++ = (char )pgm_read_byte_near( tabptr );
            if ( oddTick )
            {
               ++tabptr;
               --samples_to_go;
            }
            oddTick = ! oddTick;
         }
         else
         {
            *buf++ = 0;
         }
      }
   }
   else                            // "hiFi" - play 1 sample per tick
   {
      while ( icnt-- )
      {
         if ( samples_to_go )                      
         {
            *buf++ = (char )pgm_read_byte_near( tabptr++ );
            --samples_to_go;
         }
         else
         {
            *buf++ = 0;
         }
      }
   }
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  SampleOsc::setSample
 *
 *  Desc:  Set the wavetable to be output.
 *
 *  Args:  descriptor       - wavetable descriptor
 *
 *  Memb: +samples_to_go    - # of samples remaining to output
 *
 *----------------------------------------------------------------------------*/      

void SampleOsc::setSample( const desWavTab *d ) 
{
   setTable( d );
   samples_to_go = 0;
   loFi = ( period == 0.0 );
}

