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

class DuoSynth : public StereoSynth
{
   public:

   DuoSynth();
    
   boolean charEv( char );          // handle a character event
   boolean evHandler( obEvent );    // handle an onboard event
   void    dynamics();              // perform a dynamic update
   void    onFocus( focus f );      // execute on change in console focus
   void    output(char*, char*);    // write one stereo buffer of audio
   void    setVol( byte );          // set the volume level
   void    startup();               // execute at end of system startup

   #ifdef KEYBRD_MENUS
   char    menu( key );             // map key event to character 
   #endif

   #ifdef CONSOLE_OUTPUT
   const char *prompt();            // return object's prompt string
   #endif
} ;

#endif   // ifndef DUOSYNTH_H_INCLUDED
