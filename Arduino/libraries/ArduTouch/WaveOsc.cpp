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

   /* ---- Note: rounding makes no discernible difference

   if ( interp._.lsb & 0x80 )    // round interpolated value
      ++interp._.msb;

   */

   return interp._.msb;    
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  WaveOsc::getPeriod
 *
 *  Desc:  Returns the number of samples per wavelength.
 *
 *  Glob:  audioRate        - audio playback rate
 *
 *  Memb:  coeff            - step = frequency * coeff
 *
 *----------------------------------------------------------------------------*/      

double WaveOsc::getPeriod()
{
   return  coeff * audioRate / pow(2, 16);
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
 *  Desc:  Set the wavetable + name.
 *
 *  Args:  descriptor       - wavetable descriptor
 *         name             - wavetable name
 *
 *  Glob:  audioRate        - audio playback rate
 *
 *  Memb: +coeff            - step = freq * coeff
 *        +idx              - current accumulated idx into table
 *        +length           - # of samples in table 
 *        +name             - wavetable name
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
   length  = desc.length;

   name    = n;
   idx.val = 0;

   /* compute dependent fields */

   coeff   = pow(2, 16) * (desc.period / audioRate);
   onFreq();
}


/*----------------------------------------------------------------------------*
 *
 *  Name:  WaveOsc::setTableFromBank
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

   word integralAggStep = longAggStep._.msw.val + 1;

   if ( length > integralAggStep )
      aggEnd = length - integralAggStep;
   else
      aggEnd = 0;
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
 *        +idx              - accumulated ptr into wave table 
 *         lofi             - if true, sample is lofi
 *        +step             - amount to increment idx per tick
 *         table            - ptr to wave table 
 *        +ticks_to_go      - # of ticks of output remaining
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

         idx._.lsw.val = 0;                     // idx = table element 0
         idx._.msw.val = (word )table;

         if ( lofi )
         {
            step.val = SAMPLE_UNIT_STEP >> 1;   // "0.5"
            ticks_to_go = length << 1;
         }
         else                                   
         {
            step.val = SAMPLE_UNIT_STEP;        // "1.0"
            ticks_to_go = length;
         }

         break;

      case '!':                           // reset

         super::charEv( key );
         ticks_to_go = 0;
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
 *  Memb: +idx              - accumulated ptr into wave table 
 *        +ticks_to_go      - # of ticks of output remaining
 *         step             - amount to increment idx per tick
 *
 *-------------------------------------------------------------------------*/      

void SampleOsc::output( char *buf ) 
{
   byte icnt   = audioBufSz;       // write this many ticks of output

   while ( icnt-- )
   {
      if ( ticks_to_go )                      
      {
         *buf++   = (char )pgm_read_byte_near( idx._.msw.val );
         idx.val += step.val;                      
         --ticks_to_go;
      }
      else
      {
         *buf++ = 0;
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
 *  Memb: +lofi             - if true, sample is lofi
 *        +ticks_to_go      - # of ticks of output remaining
 *
 *----------------------------------------------------------------------------*/      

void SampleOsc::setSample( const desWavTab *d ) 
{
   setTable( d );
   ticks_to_go = 0;                       // 0 until next trigger
   lofi = (getPeriod() < 0.01 );          // lofi samples have period = 0.0         
}

/******************************************************************************
 *
 *                               ModSampleOsc
 *
 ******************************************************************************/

/*----------------------------------------------------------------------------*
 *
 *  Name:  ModSampleOsc::charEv
 *
 *  Desc:  Process a character event.
 *
 *  Args:  code             - character to process
 *
 *  Memb: +flags.DONE       - output of wavetable has been completed
 *        +idx              - accumulated ptr into wave table 
 *         runlength        - # of ticks of output per trigger
 *        +shiftStep        - left-shift step this many bits
 *        +ticks_to_go      - # of ticks of output remaining
 *  
 *  Rets:  status           - true if character was handled
 *
 *----------------------------------------------------------------------------*/      

bool ModSampleOsc::charEv( char code )
{
   #define TWOS_COMP_MASK 0xFFFFFFFFFFFFFFFF

   switch ( code )
   {
      case chrTrigger:                    // trigger the sample
      {
         super::charEv( code );

         if ( shiftStep > 0 )             // apply bit shift ...
         {
            step.val <<= shiftStep;
            ticks_to_go >>= shiftStep;
         }
         else if ( shiftStep < 0 )
         {
            step.val >>= -shiftStep;
            ticks_to_go <<= -shiftStep;
         }
   
         if ( ! forwards )
         {
            step.val ^= TWOS_COMP_MASK;   // step = -step
            step.val++;                   // ""

            idx._.msw.val += length;      // idx = last element + 1 - step
            idx.val += step.val;          // ""
         }

         break;
      }

      #ifdef INTERN_CONSOLE

      case '+':                           // play forwards

         forward();
         break;

      case '-':                           // play backwards

         backward();
         break;

      case 'r':                           // reverse (flip) playback direction

         flip();
         break;

      case 's':                           // stretch the sample
      {
         int pwr2;
         if ( console.getInt( CONSTR("stretch"), &pwr2 ) )
            stretch( pwr2 );
         break;
      }
      #endif
                                          
      #ifdef CONSOLE_OUTPUT

      case chrInfo:

         super::charEv( code );
         console.newlntab();
         console.print( forwards ? '+' : '-' );
         console.space();
         console.infoInt( CONSTR("stretch"), shiftStep );
         break;

      #endif

      case '!':                           // reset

         super::charEv( code );
         forward();
         shiftStep = 0;
         break;

      default:

         return super::charEv( code );
   }
   return true;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  ModSampleOsc::flip
 *
 *  Desc:  Flip the current playback direction.
 *
 *  Memb:  forwards         - if true, play sample forwards
 *
 *----------------------------------------------------------------------------*/      

void ModSampleOsc::flip()
{
   if ( forwards )
      backward();
   else
      forward();
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  ModSampleOsc::forward
 *
 *  Desc:  Set the playback direction to forward.
 *
 *  Memb: +forwards         - if true, play sample forwards
 *
 *----------------------------------------------------------------------------*/      

void ModSampleOsc::forward()
{
   forwards = true;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  ModSampleOsc::backward
 *
 *  Desc:  Set the playback direction to backward.
 *
 *  Memb: +forwards         - if true, play sample forwards
 *
 *----------------------------------------------------------------------------*/      

void ModSampleOsc::backward()
{
   forwards = false;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  ModSampleOsc::stretch
 *
 *  Desc:  Stretch the sample's duration by a power of 2.
 *
 *  Args:  power2           - power of 2 by which to stretch the sample
 *
 *  Memb: +shiftStep        - left-shift step this many bits
 *
 *  Note:  The maximum stretch (positive or negative) is 3. 
 *
 *         For example:
 *
 *             stretch(1)  doubles the playback duration of the sample
 *             stretch(-2) quarters the playback duration of the sample
 *             stretch(0)  sets the playback duration to normal
 *
 *----------------------------------------------------------------------------*/      

void ModSampleOsc::stretch( int pwr2 )
{
   #define MAX_SHIFT 3                 // max shift in either direction is 3

   if ( pwr2 < 0 )
      shiftStep = pwr2 < MAX_SHIFT ? pwr2 : MAX_SHIFT;
   else if ( pwr2 > 0 )
      shiftStep = pwr2 > -MAX_SHIFT ? pwr2 : -MAX_SHIFT;
   else
      shiftStep = 0;
}

/******************************************************************************
 *
 *                             TunedSampleOsc
 *
 ******************************************************************************/

/*----------------------------------------------------------------------------*
 *
 *  Name:  TunedSampleOsc::charEv
 *
 *  Desc:  Process a character event.
 *
 *  Args:  code             - character to process
 *
 *  Memb: +flags.DONE       - output of wavetable has been completed
 *        +idx              - accumulated idx into wave table 
 *  
 *  Rets:  status           - true if character was handled
 *
 *----------------------------------------------------------------------------*/      

bool TunedSampleOsc::charEv( char key )
{
   switch ( key )
   {
      case chrTrigger:                    // trigger the sample
      
         flags  &= ~DONE;                 // playback is ongoing
         idx.val = 0;                     // reset accumulated idx
         break;

      case '!':                           // reset

         super::charEv( key );
         flags  &= ~(MUTE+LEGATO);        // unmute and use staccato triggering
         flags  |= DONE;                  // playback is NOT ongoing
         break;

      default:

         return super::charEv( key );
   }
   return true;
}

/*-------------------------------------------------------------------------*
 *
 *  Name:  TunedSampleOsc::normalize
 *
 *  Desc:  Normalize the step to a given frequency.
 *
 *  Args:  freq             - normalization frequency  
 *
 *  Memb: +coeff            - step = frequency * coeff
 *
 *  Note:  Normalizing the step means that the wavetable will play back
 *         at the rate of one sample per tick when the oscillator is set 
 *         to the normalization frequency. 
 *
 *-------------------------------------------------------------------------*/      

void TunedSampleOsc::normalize( double freq ) 
{
   coeff = pow(2, 16) / freq;    // note: pow(2, 16) = "a step of 1.0"
}

/*-------------------------------------------------------------------------*
 *
 *  Name:  TunedSampleOsc::output
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

void TunedSampleOsc::output( char *buf ) 
{
   byte icnt = audioBufSz;       // write this many ticks of output

   while ( icnt-- )
   {
      if ( flags & DONE )        // sample playback is ended
      {
         *buf++ = 0;
      }
      else                       // sample playback is ongoing
      {
         *buf++   = (char )pgm_read_byte_near( (word )table + idx._.msw.val );
         idx.val += step.val;                      
         if ( idx._.msw.val >= length )
            flags |= DONE;
      }
   }
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  TunedSampleOsc::setSample
 *
 *  Desc:  Set the wavetable to be output.
 *
 *  Args:  descriptor       - wavetable descriptor
 *
 *----------------------------------------------------------------------------*/      

void TunedSampleOsc::setSample( const desWavTab *d ) 
{
   setTable( d );

   // normalize non-tonal samples to 440 Hz 
   //
   // non-tonal samples come in 2 varieties: "hi-fi" and "lo-fi"
   //
   // hifi samples are meant to be played back at a rate of 1 sample per tick
   // lofi samples are meant to be played back at a rate of 1 sample per 2 ticks
   //
   // hifi samples are denoted in their descriptor by having a period of 1.0
   // lofi samples are denoted in their descriptor by having a period of 0.0

   double period = getPeriod();

   if ( period < 1.01 )                        // if sample is non-tonal
      normalize( period > .01 ? 440.0          // ... normalize hifi sample
                              : 880.0 );       // ... normalize lofi sample
}

