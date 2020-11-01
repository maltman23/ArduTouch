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

#define MAX_FRAME_DIM 2

class Control : public Mode            // a mode that can be reset/muted/unmuted
{
   typedef Mode super;                 // superclass is Mode

   public:

   enum {  // use these bits in the flags byte: 

           MUTE    = _RESERVE0,        // control is currently muted 
           RSTMUTE = _RESERVE1         // control will be muted on reset

        } ;

   struct                              // embedded user interface frame
   {
      byte dim0 : 2;                   // limit for column 0
      byte dim1 : 2;                   // limit for column 1
      byte led0 : 2;                   // LED0 current state ( 0=off, 1=on, 2=blink )
      byte led1 : 2;                   // LED1 current state ( 0=off, 1=on, 2=blink )

      frameNum Num()                   // returns frame #
      {
         return (frameNum )( ((led0 << 2) + led1) << 1 ); 
      }

   } frame;                 

   void enableFrame()                  // enable the embedded user interface frame
   {
      flags |= UIFRAME;   
   }

   byte getPotNum( obEvent e )         // return pot # for a (pot) event in the current frame
   {
      e.stripFrame();
      return e.type() - POT0;
   }

   void setFrameDimensions( byte d0, byte d1 )  
   {
      if ( d0 <= MAX_FRAME_DIM ) frame.dim0 = d0;
      if ( d1 <= MAX_FRAME_DIM ) frame.dim1 = d1;
   }

   Control()
   {
      flags |= RSTMUTE;                // by default, control is muted on reset

      frame.dim0 = 0;                  // by default, no frames other than 00
      frame.dim1 = 0;
   }

   inline bool muted()                 // return mute status
   {
      return flags & MUTE;
   }

   bool   charEv( char );              // process a character event
   void   displayLED( byte );          // display a frame LED
   bool   evHandler( obEvent );        // handle an onboard event
   void   reset();                     // reset the control
   void   setMute( boolean );          // mute (or unmute) the control

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

   bool   charEv( char );              // process a character event
   void   release();                   // release the control trigger
   void   trigger();                   // trigger the control

   #ifdef KEYBRD_MENUS
   char   menu( key );                 // given a key, return a character  
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

   void   add( TControl* );            // add a control to the chain
   bool   charEv( char );              // process a character event 
   void   clear();                     // remove all controls from the chain
   void   dynamics();                  // perform a dynamic update on all controls
   void   release();                   // release the trigger for all controls
   void   reset();                     // reset all controls
   void   trigger();                   // trigger all controls

   #ifdef KEYBRD_MENUS
   char   menu( key );                 // map key event to character 
   #endif

   #ifdef CONSOLE_OUTPUT
   const  char* prompt();              // return object's prompt string
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
