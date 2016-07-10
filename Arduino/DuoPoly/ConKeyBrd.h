/*
    ConKeyBrd.h  

    Dclaration of the ConKeyBrd class.

    ---------------------------------------------------------------------------
 
    Copyright (C) 2016, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
   ---------------------------------------------------------------------------
*/

#ifndef CONKEYBRD_H_INCLUDED
#define CONKEYBRD_H_INCLUDED

#include "Arduino.h"
#include "Mode.h"

class ConKeyBrd : public Mode
{
   byte     position;                           // note position in current octave
                                                // 0 = "C", 1 = "C#", etc..

   boolean  whichNote( char key );              // returns true if keycode maps
                                                // to note; sets position

   static const byte   numNotes = 13;           // number of notes in an octave (+1)

   static const char   keycode_[ numNotes ];    // ascii keycodes mapped to notes
   static const char   letter_[ numNotes ];     // letter for note position
   static const char   adental_[ numNotes ];    // accidental for note position

   public:

   ConKeyBrd();

   boolean  charEv( char );                     // handle a character event
   boolean  evHandler( obEvent );               // handle an onboard event

   #ifdef CONSOLE_OUTPUT
   const    char *prompt();                     // return object's prompt string
   #endif

} ;

extern ConKeyBrd keybrd;

#endif   // ifndef CONKEYBRD_H_INCLUDED