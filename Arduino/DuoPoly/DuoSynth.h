/*
    DuoSynth.h  

    Declaration of the DuoSynth class.

    ---------------------------------------------------------------------------
 
    Copyright (C) 2016, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
   ---------------------------------------------------------------------------
*/

#ifndef DUOSYNTH_H_INCLUDED
#define DUOSYNTH_H_INCLUDED

#include "Synth.h"

class DuoSynth : public Synth
{
   public:

   DuoSynth();
    
   void     butEv( but );              // handle a button event
   void     charEv( char );            // handle a character event
   void     dynamics();                // perform a dynamic update
   void     generate(char*, char*);    // generate one stereo buffer of audio
   void     info();                    // display object info to console
   char     menu( key );               // map key event to character 
   char    *prompt();                  // return object's prompt string

   private:

   word     secDC = ticksPerSec;       // downcounter to next second

} ;

#endif   // ifndef DUOSYNTH_H_INCLUDED
