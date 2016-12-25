/*
    Osc.h  

    Declaration of the Oscillator class.

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

class Osc : public TControl
{
   private:

   char    detune;                  // local detuning (total range = 1/4 tone)
   void    calcEffFreq();           // calculate effective frequency

   protected:

   double  idealFreq;               // osc frequency sans all detuning
   double  effFreq;                 // osc frequency including local detuning
   double  extFactor;               // external detuning factor 
 
   public:

   Osc() { reset(); };

   virtual void onFreq() {};        // compute frequency dependent state vars
   virtual void output( char* ) {}; // write one buffer of output

   #ifdef CONSOLE_OUTPUT
   const char *prompt();            // return object's prompt string
   #endif

   boolean charEv( char );          // process a character event
   double  getFreq();               // return the ideal frequency
   void    modFreq( double );       // modify frequency by external factor
   void    setDetune( char );       // set the amount of local detuning
   void    setFreq( double );       // set the ideal frequency

} ;  

#endif   // ifndef OSC_H_INCLUDED
