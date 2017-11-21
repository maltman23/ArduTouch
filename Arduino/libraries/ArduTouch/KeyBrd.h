/*
    KeyBrd.h  

    Dclaration of the KeyBrd class.

    ---------------------------------------------------------------------------
 
    Copyright (C) 2016, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
   ---------------------------------------------------------------------------
*/

#ifndef KEYBRD_H_INCLUDED
#define KEYBRD_H_INCLUDED

#include "Arduino.h"
#include "Control.h"

class KeyBrd : public Control
{
   typedef Control super;                       // superclass is Control

   byte    botOctave;                           // bottom octave # 
   byte    defOctave;                           // default octave # 
   byte    topOctave;                           // top octave # 

   static const byte   MaxOctave = 15;          // maximum possible octave #

   static const byte   NumNotes = 13;           // number of notes in an octave (+1)

   static const char   keycode_[ NumNotes ];    // ascii keycodes mapped to notes
   static const char   letter_[ NumNotes ];     // letter for note position
   static const char   adental_[ NumNotes ];    // accidental for note position

   public:

   byte    octave;                              // current octave # for keyboard

   KeyBrd()
   {
      flags &= ~ECHO;

      botOctave = 0;
      topOctave = 7;
      defOctave = 4;
   }

   boolean  charEv( char );                     // handle a character event
   boolean  evHandler( obEvent );               // handle an onboard event

   void  downOctave();                          // lower keyboard by 1 octave 
   byte  getOctave();                           // return keyboard's octave 
   void  setBotOct( byte );                     // set bottom octave 
   void  setDefOct( byte );                     // set default octave 
   void  setTopOct( byte );                     // set top octave 
   void  setOctave( byte );                     // set current octave 
   void  upOctave();                            // raise keyboard by 1 octave 

   #ifdef CONSOLE_OUTPUT
   const    char *prompt();                     // return object's prompt string
   #endif

} ;

#endif   // ifndef KEYBRD_H_INCLUDED