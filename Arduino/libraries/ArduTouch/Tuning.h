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

   virtual double pitch( key ) = 0;       // return frequency for a given key

   protected:

   static const byte   numNotes = 13;     // number of notes in an octave (+1)

} ;

class EqualTemperament : public Tuning
{
   protected:

   static const double freq[ numNotes ];  // frequencies for notes in octave 0

   public:

   double pitch( key );                   // return frequency for a given key

} ;

class HarmonicTuning : public Tuning
{
   protected:

   static const double intervals[ numNotes ];  // freq ratios for notes 

   public:

   double   tonic0;                       // tonic frequency for C0

   HarmonicTuning()
   {
      tonic0 = 16.351601;
   }

   double pitch( key );                   // return frequency for a given key

} ;

/* ------------------------      public vars      -------------------------- */

extern Tuning *masterTuning;

#endif   // ifndef TUNING_H_INCLUDED