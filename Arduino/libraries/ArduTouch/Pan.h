/*
    Pan.h  

    Declaration of the Panning control.

    ---------------------------------------------------------------------------
 
    Copyright (C) 2016, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
   ---------------------------------------------------------------------------
*/

#ifndef PAN_H_INCLUDED
#define PAN_H_INCLUDED

#include "LFO.h"

/* -------------------------------------------------------------------------- */

class PanControl : public LFO
{
   typedef  LFO super;                    // superclass is LFO

   protected:

   byte  *panPos;                         // current pan position 

   byte  restPos;                         // pan position when at rest
   byte  center;                          // panning center
   byte  diameter;                        // panning diameter

   boolean pinned;                        // pan is centered around restPos

   public:

   PanControl() {}                        // null constructor

   PanControl( byte *panPos )             // constructor 
   {
      this->panPos = panPos;
   }

   void    calcExtent();                  // calculate panning extent
   boolean charEv( char code );           // handle a character event
   void    evaluate();                    // compute panPos from osc position
   void    setPinned( boolean );          // set/unset pinned state
   void    setRestPos( byte );            // set the rest position

   #ifdef KEYBRD_MENUS
   char    menu( key );                   // given a key, return a character  
   #endif

   #ifdef CONSOLE_OUTPUT
   const char *prompt();                  // return object's prompt string
   #endif

} ;

#endif   // ifndef PAN_H_INCLUDED
