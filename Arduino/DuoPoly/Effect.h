/*
    Effect.h  

    Declaration of the Effect and derived classes.

    ---------------------------------------------------------------------------
 
    Copyright (C) 2016, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
   ---------------------------------------------------------------------------
*/

#ifndef EFFECT_H_INCLUDED
#define EFFECT_H_INCLUDED

#include "Control.h"

/* -------------------------------------------------------------------------- */

class Effect : public Control
{
   public:

   char    shortcut;                // shortcut key to access object's mode
   Effect  *next;                   // ptr to next effect in loop

   virtual void process( char* ){}; // process an input buffer 

} ;  


class EffectsLoop : public Mode
{
   private:

   Effect* loop;                    // pts to 1st effect in loop

   public:

   void    append( Effect* );       // append an effect to loop
   void    charEv( char );          // process a character event 
   void    info();                  // display object info to console
   char    menu( key );             // map key event to character 
   void    process( char* );        // process audio buffer through loop
   char*   prompt();                // return object's prompt string

} ;

#endif   // ifndef EFFECT_H_INCLUDED
