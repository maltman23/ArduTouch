/*
    Tuning.h  

    Declaration of Tuning classes.

    ---------------------------------------------------------------------------
 
    Copyright (C) 2016, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
   ---------------------------------------------------------------------------
*/
                                    
#ifndef TUNING_H_INCLUDED
#define TUNING_H_INCLUDED

#include "Onboard.h"

class Tuning 
{
   public:

   byte    minOctave;                     // minimum octave # (0-15)
   byte    maxOctave;                     // maximum octave # (0-15)
   byte    defOctave;                     // default octave # (0-15)

   Tuning()
   {
      defOctave = 4;
      maxOctave = 7;
   }

   virtual double pitch( key ) = 0;       // return frequency for a given key

} ;

class EqualTemperament : public Tuning
{
   private:

   static const byte   numNotes = 13;     // number of notes in an octave (+1)
   static const double freq[ numNotes ];  // frequencies for notes in octave 0

   public:

   double pitch( key );                   // return frequency for a given key

} ;

/* ------------------------      public vars      -------------------------- */

extern Tuning *masterTuning;

#endif   // ifndef TUNING_H_INCLUDED