/*
    StepSqnc.h  

    Declaration of the Step Sequencer class.

    ---------------------------------------------------------------------------
 
    Copyright (C) 2016, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
   ---------------------------------------------------------------------------
*/

#ifndef STEPSQNC_H_INCLUDED
#define STEPSQNC_H_INCLUDED

#include "Control.h"

class StepSqnc     
{
   public:
   
   Control *subject;             // entity that is being sequenced 

   bool   _charEv( char );       // process a character event
   void   info();                // display object info to console
   void   cont();                // continue playback for a buffer's length of time
   void   setTempo( double );    // set sequencer playback tempo
   void   start();               // start sequencer playback
   void   stop();                // stop sequencer playback 

   private:

   static  const byte pbOFF    = 0;  // playback status values
   static  const byte pbON     = 1; 
   static  const byte pbPAUSED = 2; 
   
   byte    playback;             // playback status
   bool    unMute;               // unMute subject when encountering tokenUnMute  

   word    ticksPerBeat;         // # ticks per beat
   word    beatDC;               // downcounter: # ticks to next beat
   byte    exeDC;                // downcounter: # beats to record execution
   double  tempo;                // beats per second

   static const byte MaxLen = 65;// max # bytes in a compiled sequence

   byte    sqnc[ MaxLen ];       // compiled sequence records
   byte    idx;                  // index of next record in sqnc[] 

   void    exeRec();             // execute next record in sqnc[]

} ;

#endif   // ifndef STEPSQNC_H_INCLUDED
