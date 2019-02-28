/*
    StdEffects.h  

    Declaration of standard effect classes.

    ---------------------------------------------------------------------------
 
    Copyright (C) 2016, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
   ---------------------------------------------------------------------------
*/

#ifndef STD_EFFECTS_H_INCLUDED
#define STD_EFFECTS_H_INCLUDED

#include "types.h"
#include "Control.h"
#include "Envelope.h"
#include "LFO.h"

/******************************************************************************
 *
 *                                 BSFilter 
 *
 ******************************************************************************/

class BSFilter : public Effect      // bit-shift filter
{
   static const byte MaxClip = 7;   // max # high bits to clip after shift

   int     last;                    // last output value   

   byte    shift;                   // # of bits to (left) shift last value
   byte    mltShift;                // equivalent multiplier for shift 

   byte    numClip;                 // # of high bits to clip after shift
   byte    clipMask;                // bitMask corresponding to numClip

   boolean complement;              // if true, complement filter output

   public:

   BSFilter()
   { 
      shortcut = 'b';
   }

   boolean charEv( char );          // process a character event
   boolean evHandler( obEvent );    // handle an onboard event
   void   process( char* );         // process an input buffer 
   void   setClip( byte );          // set # bits to clip avg
   void   setShift( byte );         // set # bits to (left) shift

   #ifdef KEYBRD_MENUS
   char    menu( key );             // map key event to character 
   #endif

   #ifdef CONSOLE_OUTPUT
   const  char *prompt();           // return object's prompt string
   #endif

} ;  // BSFilter


/******************************************************************************
 *
 *                                 LPFilter 
 *
 ******************************************************************************/

class LPFilter : public Effect      // low-pass filter (via exponential movavg)
{
   private:

   char  last;                      // last output value   

   protected:

   byte  weight;                    // weighting applied to input term (n/255)

   public:

   byte  cutoff;                    // cutoff level (n/255) 

   LPFilter() 
   {
      shortcut = 'l';
   }

   virtual void calcWeight();       // calculate weighting applied to input term

   boolean charEv( char );          // process a character event
   boolean evHandler( obEvent );    // handle an onboard event
   void   process( char* );         // process an input buffer 
   void   setCutoff( byte );        // set cutoff value

   #ifdef KEYBRD_MENUS
   char    menu( key );             // map key event to character 
   #endif

   #ifdef CONSOLE_OUTPUT
   const  char  *prompt();          // return object's prompt string
   #endif

} ;  // LPFilter


/******************************************************************************
 *
 *                                 FiltEnv 
 *
 ******************************************************************************/

class FiltEnv : public LPFilter     // filter envelope
{
   ADSR  env;

   public:

   FiltEnv()
   {
      shortcut = 'f';
   }

   void    calcWeight();            // calculate weighting applied to input term
   boolean charEv( char );          // process a character event
   void    dynamics();              // update dynamics

   #ifdef CONSOLE_OUTPUT
   const  char  *prompt();          // return object's prompt string
   #endif

} ;

/******************************************************************************
 *
 *                                  AutoWah 
 *
 ******************************************************************************/

class WahLFO : public LFO           // LFO used as component of AutoWah
{
   public:

   boolean charEv( char );          // process a character event

} ;

class AutoWah : public LPFilter     // oscillating low-pass filter
{
   public:

   WahLFO  lfo;

   AutoWah()
   {
      shortcut = 'a';
   }

   void    calcWeight();            // calculate weighting applied to input term
   boolean charEv( char );          // process a character event
   void    dynamics();              // update dynamics

   #ifdef CONSOLE_OUTPUT
   const  char  *prompt();          // return object's prompt string
   #endif

} ;

#endif   // ifndef STD_EFFECTS_H_INCLUDED
