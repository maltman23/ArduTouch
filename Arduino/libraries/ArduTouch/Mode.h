/*
    Mode.h  

    Declaration of the Mode class.

    ---------------------------------------------------------------------------
 
    Copyright (C) 2016, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
   ---------------------------------------------------------------------------
*/

#ifndef MODE_H_INCLUDED
#define MODE_H_INCLUDED

#include "Arduino.h"
#include "Model.h"
#include "Onboard.h"

class Mode
{
   public:

   byte     flags;                     // flag bits denote:

   enum     { ECHO      = 0b00000001,  // echo character input to console
              MENU      = 0b00000010,  // charEv(menu()) pre-empts KEY_DOWN
              UIFRAME   = 0b00000100,  // enable embedded user interface frames
              _RESERVE0 = 0b00001000,  // reserved for use by derived classes
              _RESERVE1 = 0b00010000,  // reserved for use by derived classes
              _RESERVE2 = 0b00100000,  // reserved for use by derived classes
              _RESERVE3 = 0b01000000,  // reserved for use by derived classes
              _RESERVE4 = 0b10000000,  // reserved for use by derived classes
            };

   Mode()
   {
      flags = ECHO;
   }

   virtual bool charEv( char );        // handle a character event
   virtual bool evHandler( obEvent );  // handle an onboard event

   #ifdef KEYBRD_MENUS
   virtual char    menu( key );        // given a key, return a character
   #endif

   #ifdef CONSOLE_OUTPUT
   virtual const   char* prompt();     // return prompt string for this object
   #endif

   void    brief();                    // display brief object info 
   void    execute( const char * );    // execute a macro 
   void    inform();                   // display object info, then new prompt
   bool    potEv( obEvent );           // handle pot event, stripping frame # if necessary

} ;

#endif   // ifndef MODE_H_INCLUDED
