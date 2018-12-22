/*
    Osc.h  

    Declaration of the Osc and NullOsc classes.

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
   virtual double realFreq();       // return the real frequency

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
 *                                 NullOsc 
 *
 ******************************************************************************/

class NullOsc : public Osc                // place-holder osc which outputs silence
{
   typedef Osc super;                     // superclass is Osc

   public:

   void  output( char* buf );             // write one buffer of output
} ;

#endif   // ifndef OSC_H_INCLUDED
