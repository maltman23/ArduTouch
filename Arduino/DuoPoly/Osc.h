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

#include "Mode.h"

class Osc : public Mode
{
   protected:

   double  idealFreq;               // frequency sans detuning
   double  effFreq;                 // frequency including detuning
 
   char    detune;                  // detuning amount

   void    calcEffFreq();           // calculate the effective frequency

   public:

   virtual void modFreq( double );  // apply factor to effective frequency 
   virtual void setFreq( double );  // set the ideal frequency

   boolean charEv( char );          // process a character event
   double  getFreq();               // return the ideal frequency
   void    setDetune( char );       // set the amount of detuning

   virtual void output( char* ) 
   {
      // write output to one (mono) audio buffer
   };     

} ;  

#endif   // ifndef OSC_H_INCLUDED
