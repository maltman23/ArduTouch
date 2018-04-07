/*
    Osc.h  

    Declaration of abstract Oscillator base classes.

    ---------------------------------------------------------------------------
 
    Copyright (C) 2016, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
   ---------------------------------------------------------------------------
*/

#ifndef OSC_H_INCLUDED
#define OSC_H_INCLUDED

#include "Control.h"

/******************************************************************************
 *
 *                                   Osc 
 *
 ******************************************************************************/

class Osc : public TControl
{
   typedef TControl super;          // superclass is TControl

   private:

   char    detune;                  // local detuning (total range = 1/4 tone)
   void    calcEffFreq();           // calculate effective frequency

   protected:

   double  idealFreq;               // osc frequency sans all detuning
   double  effFreq;                 // osc frequency including local detuning
   double  extFactor;               // external detuning factor 
 
   public:

   virtual void onFreq() {};        // compute frequency dependent state vars
   virtual void output( char* ) {}; // write one buffer of output

   boolean charEv( char );          // process a character event
   double  getFreq();               // return the ideal frequency
   void    modFreq( double );       // modify frequency by external factor
   void    setDetune( char );       // set the amount of local detuning
   void    setFreq( double );       // set the ideal frequency

   #ifdef CONSOLE_OUTPUT
   const char *prompt();            // return object's prompt string
   #endif

} ;  

/******************************************************************************
 *
 *                                 DualOsc 
 *
 ******************************************************************************/

class DualOsc : public Osc
{
   typedef Osc super;                     // superclass is Osc

   public:

   Osc *osc0;                             // ptr to oscillator 0
   Osc *osc1;                             // ptr to oscillator 1

   DualOsc( Osc *o0, Osc *o1 )
   {
      osc0 = o0;
      osc1 = o1;
   }

   bool  charEv( char );                  // process a character event
   void  dynamics();                      // update dynamics
   void  onFreq();                        // compute frequency dependent state vars
   void  output( char* );                 // write one buffer of output

   #ifdef CONSOLE_OUTPUT
   const char *prompt();            // return object's prompt string
   #endif

} ;

#endif   // ifndef OSC_H_INCLUDED
