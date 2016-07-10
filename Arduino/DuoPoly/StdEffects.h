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

#include "Effect.h"

class Filter : public Effect
{
   protected:

   boolean complement;              // if true, complement filter output

   public:

   boolean charEv( char );          // process a character event

   #ifdef KEYBRD_MENUS
   char    menu( key );             // map key event to character 
   #endif


} ;  // Filter


class BSFilter : public Filter
{
   static const byte MaxClip = 7;   // max # high bits to clip after shift

   int     last;                    // last output value   

   byte    shift;                   // # of bits to (left) shift last value
   byte    mltShift;                // equivalent multiplier for shift 

   byte    numClip;                 // # of high bits to clip after shift
   byte    clipMask;                // bitMask corresponding to numClip

   public:

   BSFilter(); 

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


class XMFilter : public Filter
{
   private:

   char  last;                      // last output value   
   byte  weight;                    // weighting (n/255) to input term 
   Word  norm;                      // normalization factor when complemented

   public:

   XMFilter(); 

   boolean charEv( char );          // process a character event
   boolean evHandler( obEvent );    // handle an onboard event
   void   process( char* );         // process an input buffer 
   void   setShift( byte );         // set # bits to (left) shift
   void   setWeight( byte );        // set input term weighting

   #ifdef KEYBRD_MENUS
   char    menu( key );             // map key event to character 
   #endif

   #ifdef CONSOLE_OUTPUT
   const  char  *prompt();          // return object's prompt string
   #endif

} ;  // XMFilter

#endif   // ifndef STD_EFFECTS_H_INCLUDED
