/*
    DualOsc.cpp  

    Implementation of the DualOsc and derived classes.

    ---------------------------------------------------------------------------
 
    Copyright (C) 2016, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
   ---------------------------------------------------------------------------
*/

#include "types.h"
#include "Console_.h"
#include "Audio.h"
#include "DualOsc.h"

/******************************************************************************
 *
 *                                 DualOsc 
 *
 ******************************************************************************/

/*----------------------------------------------------------------------------*
 *
 *  Name:  DualOsc::charEv
 *
 *  Desc:  Process a character event.
 *
 *  Args:  code             - character to process
 *
 *  Memb: +freqRatio        - frequency ratio between osc1 and osc0
 *         osc0             - ptr to oscillator 0
 *         osc1             - ptr to oscillator 1
 *
 *  Rets:  status           - true if character was handled
 *
 *----------------------------------------------------------------------------*/      

bool DualOsc::charEv( char code )
{
   switch ( code )
   {
      case chrTrigger:

         osc0->trigger();
         osc1->trigger();
         break;

      #ifdef INTERN_CONSOLE

      case '0':

         console.pushMode( osc0 );
         break;

      case '1':

         console.pushMode( osc1 );
         break;

      case 'r':

         console.getDouble( CONSTR("ratio"), &freqRatio );
         break;

      #endif

      #ifdef CONSOLE_OUTPUT

      case chrInfo:

         super::charEv( code );
         console.newlntab();
         console.infoDouble( CONSTR("ratio"), freqRatio );
         break;

      #endif

      case '!':                     // perform a reset

         osc0->reset();
         osc1->reset();
         freqRatio = 1.00;           

      default:

         return super::charEv( code );
   }
   return true;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  DualOsc::dynamics
 *
 *  Desc:  Perform a dynamic update.
 *
 *  Memb:  osc0             - ptr to oscillator 0
 *         osc1             - ptr to oscillator 1
 *
 *----------------------------------------------------------------------------*/      

void DualOsc::dynamics()
{
   osc0->dynamics();
   osc1->dynamics();
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  DualOsc::onFreq
 *
 *  Desc:  Compute frequency-dependent state vars.
 *
 *  Memb:  effFreq          - effective frequency (includes internal detuning)
 *         extFactor        - external detuning factor
 *         freqRatio        - frequency ratio between osc1 and osc0
 *         osc0             - ptr to oscillator 0
 *         osc1             - ptr to oscillator 1
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

void DualOsc::onFreq()
{
   double f = effFreq * extFactor;
   osc0->setFreq( f );
   osc1->setFreq( f * freqRatio );
}

/*-------------------------------------------------------------------------*
 *
 *  Name:  DualOsc::output
 *
 *  Desc:  Write output to an audio buffer.
 *
 *  Args:  buf              - ptr to audio buffer  
 *
 *  Glob:  audioBufSz       - size of system audio buffers
 *
 *  Memb:  osc0             - ptr to oscillator 0
 *         osc1             - ptr to oscillator 1
 *
 *-------------------------------------------------------------------------*/      

void DualOsc::output( char *buf ) 
{
   char  buf1[ audioBufSz ];     // temp buffer for for holding osc1 output

   osc0->output( buf );
   osc1->output( &buf1[0] );

   int sum;
   for ( byte i = 0 ; i < audioBufSz; i++ )
   {
      sum    = buf[i] + buf1[i];
      buf[i] = sum >> 1;
   }
}

/******************************************************************************
 *
 *                                 MixOsc 
 *
 ******************************************************************************/

/*----------------------------------------------------------------------------*
 *
 *  Name:  MixOsc::charEv
 *
 *  Desc:  Process a character event.
 *
 *  Args:  code             - character to process
 *
 *  Memb: +freqRatio        - frequency ratio between osc1 and osc0
 *         mix              - mix between osc0/1 
 *         osc0             - ptr to oscillator 0
 *         osc1             - ptr to oscillator 1
 *
 *  Rets:  status           - true if character was handled
 *
 *----------------------------------------------------------------------------*/      

bool MixOsc::charEv( char code )
{
   switch ( code )
   {
      #ifdef INTERN_CONSOLE

      case 'm':
      {
         byte userMix;
         if ( console.getByte( CONSTR("mix"), &userMix ) )
            setMix( userMix );
         break;
      }

      case 's':                        // set spectrum
      {
         byte userInput;
         if ( console.getByte( CONSTR( "spectrum" ), &userInput ) )
            setSpectrum( userInput );
         break;
      }

      #endif

      #ifdef CONSOLE_OUTPUT

      case chrInfo:

         super::charEv( code );
         console.infoByte( CONSTR("mix"), mix );
         console.infoByte( CONSTR("spectrum"), spectrum );
         break;

      #endif

      case '!':                     // perform a reset

         spectrum = 1;

         setMix( 64 );              // even mix between osc0 and osc1

      default:

         return super::charEv( code );
   }
   return true;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  MixOsc::evHandler
 *
 *  Desc:  Handle an onboard event.
 *
 *  Args:  ev               - onboard event
 *
 *  Memb: +freqRatio        - frequency ratio between osc1 and osc0
 *
 *  Rets:  status           - true if the event was handled
 *
 *----------------------------------------------------------------------------*/      

boolean MixOsc::evHandler( obEvent ev )
{
   switch ( ev.type() )
   {
      case BUT0_TPRESS:                // toggle mute status 

         setMute( ! muted() );
         break;

      case BUT1_TPRESS:                // bump frequency spectrum

         setSpectrum( spectrum+1 );
         break;

      case POT0:                       // set oscillator mix
      {
         byte val = ev.getPotVal();
         if ( val < 255 )
            val >>= 1;
         else
            val = 128;
         setMix( val );
         break;
      }
                                       // select frequency (from spectrum)
      case POT1:       

         freqRatio = getSpecFreq( ev.getPotVal() );
         break;

      default:

         return super::evHandler( ev );
   }
   return true;
}

/*-------------------------------------------------------------------------*
 *
 *  Name:  MixOsc::getSpecFreq
 *
 *  Desc:  Return the nth frequency in the current frequency spectrum.
 *
 *  Args:  nth              - index of frequency within spectrum 
 *
 *  Rets:  freq             - nth frequency within spectrum 
 *
 *  Note:  Each frequency spectrum consists of (up to) 256 frequencies.
 *
 *-------------------------------------------------------------------------*/      

double MixOsc::getSpecFreq( byte nth )
{
   double freq;                           // return frequency

   switch ( spectrum )
   {

      // 1st frequency spectrum consists of diatonic intervals starting
      // an octave below unity

      case 1:
      {
         #define RATIO_SEMITONE 1.059463  // ratio between diatonic pitches

         nth >>= 2;                       // 64 frequencies in this spectrum

         byte octave = 1 + (nth / 12);
         byte modula = nth % 12;

         freq = .5;
         for ( byte i = 1; i < octave; i++ )
            freq *= 2.0;
         for ( byte i = 0; i < modula; i++ )
            freq *= RATIO_SEMITONE;

         break;
      }

      // 2nd frequency spectrum consists of multiples of .250 and .333 

      case 2:
      {
         nth >>= 2;                       // 64 frequencies in this spectrum

         byte base   = nth / 6;
         byte modula = nth % 6;

         switch ( modula )
         {
            case 0:  freq = 0.000;  break;
            case 1:  freq = 0.250;  break;
            case 2:  freq = 0.333;  break;
            case 3:  freq = 0.500;  break;
            case 4:  freq = 0.667;  break;
            case 5:  freq = 0.750;  break;
         }

         freq += base;

         if ( freq == 0.0 )               // handles case of base = modula = 0
            freq = .125;

         break;
      }
   }

   return freq;
}

/*-------------------------------------------------------------------------*
 *
 *  Name:  MixOsc::numSpectrums
 *
 *  Desc:  Return the number of supported frequency spectrums.
 *
 *  Rets:  num              - number of supported frequency spectrums.  
 *
 *-------------------------------------------------------------------------*/      

byte MixOsc::numSpectrums()
{
   return 2;                              // there are 2 built-in spectrums
}

/*-------------------------------------------------------------------------*
 *
 *  Name:  MixOsc::output
 *
 *  Desc:  Write output to an audio buffer.
 *
 *  Args:  buf              - ptr to audio buffer  
 *
 *  Glob:  audioBufSz       - size of system audio buffers
 *
 *  Memb:  osc0             - ptr to oscillator 0
 *         osc1             - ptr to oscillator 1
 *         scalar1          - scaling coefficient for osc1
 *
 *-------------------------------------------------------------------------*/      

void MixOsc::output( char *buf ) 
{
   char  buf1[ audioBufSz ];     // temp buffer for holding osc1 output

   osc0->output( buf );
   osc1->output( &buf1[0] );

   Int sum;

   word scalar0 = 256 - scalar1; // scaling coefficient for osc0

   for ( byte i = 0 ; i < audioBufSz; i++ )
   {
      sum.val = scalar0*buf[i] + scalar1*buf1[i];
      buf[i] = sum._.msb;
   }
}

/*-------------------------------------------------------------------------*
 *
 *  Name:  MixOsc::setMix
 *
 *  Desc:  Set mix between osc0 and osc1.
 *
 *  Args:  mix              - (0=all osc0; 64=even; 128=all osc1)  
 *
 *  Memb: +mix              - mix between osc0/1 
 *        +scalar1          - scaling coefficient for osc1
 *
 *-------------------------------------------------------------------------*/      

void MixOsc::setMix( byte m )
{
   if ( m > 128 ) 
      m = 128;
   mix = m;

   scalar1   = m;
   scalar1 <<= 1;
} 


/*-------------------------------------------------------------------------*
 *
 *  Name:  MixOsc::setSpectrum
 *
 *  Desc:  Set the frequency spectrum (by ordinal number).
 *
 *  Args:  nth              - frequency spectrum 
 *
 *-------------------------------------------------------------------------*/      

void MixOsc::setSpectrum( byte nth )
{
   if ( nth > numSpectrums() )
      nth = 1;

   if ( nth < 1 )
      nth = 1;

   spectrum = nth;
}

/******************************************************************************
 *
 *                                  XorOsc 
 *
 ******************************************************************************/

/*----------------------------------------------------------------------------*
 *
 *  Name:  XorOsc::charEv
 *
 *  Desc:  Process a character event.
 *
 *  Args:  code             - character to process
 *
 *  Memb: +freqDiff         - add this to osc1 frequency after freqRatio 
 *
 *  Rets:  status           - true if character was handled
 *
 *----------------------------------------------------------------------------*/      

bool XorOsc::charEv( char code )
{
   switch ( code )
   {
      #ifdef INTERN_CONSOLE
      case 'D':                        // set freq Differ
      {
         console.getDouble( CONSTR( "Diff" ), &freqDiff );
         break;
      }
      #endif

      #ifdef CONSOLE_OUTPUT
      case chrInfo:
         super::charEv( code );
         console.infoDouble( CONSTR( "Diff" ), freqDiff );
         break;
      #endif

      case '!':                        // perform a reset

         freqDiff  = 0.0;

      default:

         return super::charEv( code );
   }
   return true;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  XorOsc::onFreq
 *
 *  Desc:  Compute frequency-dependent state vars.
 *
 *  Memb:  effFreq          - effective frequency (includes internal detuning)
 *         extFactor        - external detuning factor
 *         freqDiff         - add this to osc1 frequency after freqRatio 
 *         freqRatio        - frequency ratio between osc1 and osc0
 *         osc0             - ptr to oscillator 0
 *         osc1             - ptr to oscillator 1
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

void XorOsc::onFreq()
{
   double f = effFreq * extFactor;
   osc0->setFreq( f );

   if ( f == 0.0 )
      osc1->setFreq( f );
   else
   {
      f = f * freqRatio + freqDiff;
      if ( f < 0.0 )
         f = 0.0;
      osc1->setFreq( f );
   }
}

/*-------------------------------------------------------------------------*
 *
 *  Name:  XorOsc::output
 *
 *  Desc:  Write output to an audio buffer.
 *
 *  Args:  buf              - ptr to audio buffer  
 *
 *  Glob:  audioBufSz       - size of system audio buffers
 *
 *  Memb:  osc0             - ptr to oscillator 0
 *         osc1             - ptr to oscillator 1
 *         scalar1          - scaling coefficient for osc1
 *
 *-------------------------------------------------------------------------*/      

void XorOsc::output( char *buf ) 
{
   if ( muted() )                 // pass-thru output if XORing is OFF
   {
      super::output( buf );
      return;
   }

   // XORing is ON

   char  buf1[ audioBufSz ];     // temp buffer for holding osc1 output

   osc0->output( buf );
   osc1->output( &buf1[0] );

   Int  sum;                     // temp register for XORed outputs
   word scalar0 = 256 - scalar1; // scaling coefficient for osc0

   for ( byte i = 0 ; i < audioBufSz; i++ )
   {
      sum.val = scalar0*buf[i] ^ scalar1*buf1[i];
      buf[i] = sum._.msb;
   }
}

/******************************************************************************
 *
 *                                  RingMod 
 *
 ******************************************************************************/

/*-------------------------------------------------------------------------*
 *
 *  Name:  RingMod::output
 *
 *  Desc:  Write output to an audio buffer.
 *
 *  Args:  buf              - ptr to audio buffer  
 *
 *  Glob:  audioBufSz       - size of system audio buffers
 *
 *  Memb:  osc0             - ptr to oscillator 0
 *         osc1             - ptr to oscillator 1
 *
 *-------------------------------------------------------------------------*/      

void RingMod::output( char *buf ) 
{

   if ( muted() )                 // pass-thru if ring modulation is OFF
   {
      super::output( buf );
      return;
   }

   // ring modulation is ON

   char  buf1[ audioBufSz ];     // temp buffer for holding osc1 output

   osc0->output( buf );
   osc1->output( &buf1[0] );

   Int   sum;                    // temp register for holding multiplied outputs

   for ( byte i = 0 ; i < audioBufSz; i++ )
   {
      sum.val = buf[i] * buf1[i];
      buf[i] = sum._.msb;
   }
}

