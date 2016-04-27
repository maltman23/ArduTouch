/*
    Control.h  

    Declaration of the Control class.

    ---------------------------------------------------------------------------
 
    Copyright (C) 2016, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
   ---------------------------------------------------------------------------
*/

#ifndef CONTROL_H_INCLUDED
#define CONTROL_H_INCLUDED

#include "Mode.h"

class Control : public Mode
{
   public:

   enum { MUTE = _RESERVE0 };          // flags.MUTE = 1 if object muted

   virtual void  dynamics() {};        // perform a dynamic update

   inline boolean amMute()             // return mute status
   {
      return flags & MUTE;
   }

   void    charEv( char );             // process a character event
   void    info();                     // display object info to console
   char    menu( key );                // map key event to character 
   void    reset();                    // reset object state
   void    setMute( boolean );         // mute-unmute object
   void    trigger();                  // trigger the control

} ;

/* character codes for Control-specific commands (processed in charEv()) */

#define cmdTrigger      -2             

#endif   // ifndef CONTROL_H_INCLUDED
