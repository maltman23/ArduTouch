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

   Control();

   enum {  // use these bits in the flags byte: 

           MUTE   = _RESERVE0          // if flags.MUTE is set, object is muted

        } ;

   virtual void  dynamics() {};        // perform a dynamic update

   inline boolean amMute()             // return mute status
   {
      return flags & MUTE;
   }

   boolean charEv( char );             // process a character event
   void    reset();                    // reset object state
   void    setMute( boolean );         // mute-unmute object

   #ifdef KEYBRD_MENUS
   char    menu( key );                // map key event to character 
   #endif

} ;

class TControl : public Control        // a triggered control 
{
   public:

   enum {  // use these bits in the flags byte: 

           LEGATO = _RESERVE1          // control uses legato re-triggering

           // if flags.LEGATO is set, triggers are ignored until
           //    ready() returns true

        } ;

   boolean charEv( char );             // process a character event
   void    trigger();                  // trigger the control

   #ifdef KEYBRD_MENUS
   char    menu( key );                // map key event to character 
   #endif

   virtual boolean ready();            // trigger status

} ;

#endif   // ifndef CONTROL_H_INCLUDED
