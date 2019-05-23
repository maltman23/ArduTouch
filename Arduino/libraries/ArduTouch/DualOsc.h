/*
    DualOsc.h  

    Declaration of the DualOsc and derived classes.

    ---------------------------------------------------------------------------
 
    Copyright (C) 2018, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
   ---------------------------------------------------------------------------
*/

#ifndef DUALOSC_H_INCLUDED
#define DUALOSC_H_INCLUDED

#include "Osc.h"

/******************************************************************************
 *
 *                                 DualOsc 
 *
 ******************************************************************************/

class DualOsc : public Osc                // container for 2 oscillators
{
   typedef Osc super;                     // superclass is Osc

   public:

   Osc *osc0;                             // ptr to oscillator 0
   Osc *osc1;                             // ptr to oscillator 1

   double freqRatio;                      // frequency ratio between osc1/0

   DualOsc() : super() {};

   DualOsc( Osc *o0, Osc *o1 )
   {
      osc0 = o0;
      osc1 = o1;
   }

   bool  charEv( char );                  // process a character event
   void  dynamics();                      // update dynamics
   void  onFreq();                        // compute frequency dependent state vars
   void  output( char* );                 // write one buffer of output

   PROMPT_STR( dual ) 

} ;

/******************************************************************************
 *
 *                                  MixOsc 
 *
 ******************************************************************************/

class MixOsc : public DualOsc             // Mixes 2 oscillators
{
   typedef DualOsc super;                 // superclass is DualOsc

   // provides a default mixer with U/I

   public:

   MixOsc() : super() {};
   MixOsc( Osc* o0, Osc* o1 ) : super( o0, o1 ) {};

   bool  charEv( char );                  // process a character event
   bool  evHandler( obEvent );            // handle an onboard event
   void  output( char* );                 // write one buffer of output
   void  setMix( byte m );                // specify mix between osc0 and osc1
   void  setSpectrum( byte );             // set frequency spectrum

   virtual double getSpecFreq( byte );    // returns nth frequency in current spectrum
   virtual byte   numSpectrums();         // returns number of frequency spectrums

   PROMPT_STR( mix ) 

   protected:

   byte mix;                              // mix between osc0/1 (0 = all osc0; 64 = even; 128 = all osc1)
   word scalar1;                          // scaling coefficient for osc1

   byte spectrum;                         // current frequency spectrum 

} ; 

/******************************************************************************
 *
 *                                  XorOsc 
 *
 ******************************************************************************/

class XorOsc : public MixOsc              // XORs mixed output of 2 oscillators
{
   typedef MixOsc super;                  // superclass is MixOsc

   public:

   double freqDiff;                       // apply this difference to osc1 (post-ratio)

   XorOsc( Osc* o0, Osc* o1 ) : super( o0, o1 ) {};

   bool  charEv( char );                  // process a character event
   void  onFreq();                        // compute frequency dependent state vars
   void  output( char* );                 // write one buffer of output

   PROMPT_STR( xor ) 

} ; 

/******************************************************************************
 *
 *                                  RingMod 
 *
 ******************************************************************************/

class RingMod : public MixOsc             // a ring modulator
{
   typedef MixOsc super;                  // superclass is MixOsc

   public:

   RingMod( Osc* o0, Osc* o1 ) : super( o0, o1 ) {};

   void  output( char* );                 // write one buffer of output

   PROMPT_STR( ring ) 

} ; 

#endif   // ifndef DUALOSC_H_INCLUDED
