/*
    Control.h  

    Declaration of Control and ControlChain classes and their derivatives.

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

/******************************************************************************
 *
 *                                  Control
 *
 ******************************************************************************/

//
// A note on terminology: 
//
//    to "mute" a control means to "turn it off"
//    to "unmute" a control means to "turn it on"
//

class Control : public Mode            // a mode that can be reset/muted/unmuted
{
   typedef Mode super;                 // superclass is Mode

   public:

   enum {  // use these bits in the flags byte: 

           MUTE    = _RESERVE0,        // control is currently muted 
           RSTMUTE = _RESERVE1         // control will be muted on reset

        } ;

   Control()
   {
      flags |= RSTMUTE;                // by default, control is muted on reset
   }

   inline boolean muted()              // return mute status
   {
      return flags & MUTE;
   }

   boolean charEv( char );             // process a character event
   void    reset();                    // reset the control
   void    setMute( boolean );         // mute (or unmute) the control

   #ifdef KEYBRD_MENUS
   char    menu( key );                // given a key, return a character  
   #endif

} ;

/******************************************************************************
 *
 *                                DynaControl
 *
 ******************************************************************************/

class DynaControl : public Control     // a dynamic control
{
   typedef Control super;              // superclass is Control

   public:

   virtual void  dynamics() {};        // perform a dynamic update

} ;

/******************************************************************************
 *
 *                                 TControl
 *
 ******************************************************************************/

class TControl : public DynaControl    // a triggerable, chainable, dynamic control 
{
   typedef DynaControl super;          // superclass is DynaControl

   public:

   enum {  // use these bits in the flags byte: 

           LEGATO = _RESERVE2,         // control uses legato re-triggering
           DONE   = _RESERVE3          // control is ready to be triggered

        } ;

   char    shortcut;                   // keycode for pushing control from a chain 

   TControl *next;                     // ptr to next control in chain

   TControl() 
   { 
      next = NULL;                     // no next control by default
      shortcut = 31;                   // "None" (nonprintable char before SPACE)
   };

   boolean charEv( char );             // process a character event
   void    release();                  // release the control trigger
   void    trigger();                  // trigger the control

   #ifdef KEYBRD_MENUS
   char    menu( key );                // given a key, return a character  
   #endif

} ;

/******************************************************************************
 *
 *                                 Effect
 *
 ******************************************************************************/

class Effect : public TControl         // a triggerable, chainable, dynamic control 
                                       // which can process an audio buffer
{
   typedef TControl super;             // superclass is TControl

   public:

   virtual void process( char* ){};    // process an input buffer 

} ;  


/******************************************************************************
 *
 *                                  Factor
 *
 ******************************************************************************/

class Factor : public TControl         // a triggerable, chainable, dynamic control 
                                       // which has an output value of type double
{
   typedef TControl super;             // superclass is TControl

   public:

   double  value;                      // current output value 

} ;

/******************************************************************************
 *
 *                               ControlChain
 *
 ******************************************************************************/

class ControlChain : public Mode       // a chain of controls of type TControl
{
   typedef Mode super;                 // superclass is Mode

   public:

   TControl* chain;                    // pts to 1st control in chain

   ControlChain() 
   { 
      chain = NULL; 
   };

   void    add( TControl* );           // add a control to the chain
   boolean charEv( char );             // process a character event 
   void    clear();                    // remove all controls from the chain
   void    dynamics();                 // perform a dynamic update on all controls
   void    release();                  // release the trigger for all controls
   void    reset();                    // reset all controls
   void    trigger();                  // trigger all controls

   #ifdef KEYBRD_MENUS
   char    menu( key );                // map key event to character 
   #endif

   #ifdef CONSOLE_OUTPUT
   const   char* prompt();             // return object's prompt string
   #endif

} ;

/******************************************************************************
 *
 *                                Effects 
 *
 ******************************************************************************/

class Effects : public ControlChain    // a chain of controls of type Effect
{
   typedef ControlChain super;         // superclass is ControlChain

   public:

   void    process( char* );           // process audio buffer through chain

   #ifdef CONSOLE_OUTPUT
   const char *prompt();               // return object's prompt string
   #endif
} ;


/******************************************************************************
 *
 *                                 Factors
 *
 ******************************************************************************/

class Factors : public ControlChain    // a chain of controls of type Factor
{
   typedef ControlChain super;         // superclass is ControlChain

   public:

   double   value();                   // value of all factors multiplied together

} ;

#endif   // ifndef CONTROL_H_INCLUDED
