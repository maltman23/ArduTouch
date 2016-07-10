/*
    Chnl.h  

    Declaration of the Chnl, ChnlSqnc classes.

    ---------------------------------------------------------------------------
 
    Copyright (C) 2016, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
   ---------------------------------------------------------------------------
*/

#ifndef CHNL_H_INCLUDED
#define CHNL_H_INCLUDED

#include "Voice.h"
#include "StepSqnc.h"

class Chnl : public Voice        // a Voice, considered as one of a pair  
{                                // of (possibly latched) audio channels
   public:

   Chnl   *other;                // other channel (for latching)

   byte   led;                   // led # for channel

   boolean amFreqLatching();     // returns true if currently freq latching 
   boolean charEv( char );       // handle a character event
   void    dynamics();           // update dynamics
   boolean evHandler( obEvent ); // handle an onboard event
   char    latchIcon();          // return char denoting current freq latching
   void    onFocus( focus f );   // execute on change in console focus

   #ifdef CONSOLE_OUTPUT
   const char *prompt();         // return object's prompt string
   #endif

   void setFreqDiff( double );   // latch frequency as difference to other chnl's
   void setFreqLatch( boolean ); // set frequency latching on/off
   void setFreqRatio( double );  // latch frequency as ratio to other chnl's
   void setName( const char* );  // set display name

   protected:

   void propFreq();              // propagate freq change to other chnl

   private:

   const char *name;             // name for display to console
   boolean latched;              // if true, channel frequency is latched
   boolean arithmetic;           // if true, frequency latching is arithmetic
                                 //    else, frequency latching is geometric
   double  freqOffset;           // frequency difference between channels
                                 // if ! arithmetic the freqOffset is a ratio

   void setArithmetic(boolean);  // set latching to Arithmetic or Geometic (t/f)

} ;  

class ChnlSqnc : public Chnl, public StepSqnc  // Chnl with dedicated sequencer
{
   public: 

   ChnlSqnc();

   boolean charEv( char );       // handle a character event
   boolean evHandler( obEvent ); // handle an onboard event
   void    output( char * );     // write output to an audio buffer

   #ifdef KEYBRD_MENUS
   char    menu( key );          // map key event to character 
   #endif

} ;

#endif   // ifndef CHNL_H_INCLUDED
