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

         console.infoStr( CONSTR("waveform"), name );
         Osc::charEv( chrInfo );
         break;
      #endif

      default:

         return Osc::charEv( code );
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
char WaveOsc::menu( key k )
{
   switch ( k.position() )
   {
      case  2: return 'w';
      default: return Osc::menu( k );
   }
}
#endif

/*----------------------------------------------------------------------------*
 *
 *  Name:  WaveOsc::modFreq
 *
 *  Desc:  Modify the instantaneous frequency by applying a factor to 
 *         the effective frequency.
 *
 *  Args:  factor           - factor to apply
 *
 *  Memb:  coeff            - step = freq * coeff
 *         effFreq          - effective frequency (includes detuning)
 *        +step             - amount to increment idx per tick
 *
 *----------------------------------------------------------------------------*/      

void WaveOsc::modFreq( double factor )
{
   step = effFreq * coeff * factor;
}

/*-------------------------------------------------------------------------*
 *
 *  Name:  WaveOsc::output
 *
 *  Desc:  Write output to an audio buffer.
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

void WaveOsc::output( char *buf ) 
{
   byte icnt = audio::bufSz;     // write this many ticks of output

   while ( icnt-- )
   {
      idx.val += step;                      
      if ( idx._.msw.val >= length )
         idx._.msw.val -= length;
   
      *buf++ = evaluate();
   }
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  WaveOsc::setFreq
 *
 *  Desc:  Set the (ideal) frequency.
 *
 *  Args:  freq             - new frequency
 *
 *  Memb: +step             - amount to increment idx per tick
 *
 *----------------------------------------------------------------------------*/      

void WaveOsc::setFreq( double newFreq )
{
   Osc::setFreq( newFreq );
   step = effFreq * coeff;
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
 *        +step             - amount to increment idx per tick
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
   step    = effFreq * coeff;
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

