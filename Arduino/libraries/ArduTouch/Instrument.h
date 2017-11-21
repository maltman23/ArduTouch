/*
    Instrument.h  

    Declaration of the Instrument and StereoInstrument classes.

    ---------------------------------------------------------------------------
 
    Copyright (C) 2016, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
   ---------------------------------------------------------------------------
*/

#ifndef INSTRUMENT_H_INCLUDED
#define INSTRUMENT_H_INCLUDED

#include "KeyBrd.h"
#include "Phonic.h"

/******************************************************************************
 *
 *                               Instrument 
 *                                                                            
 ******************************************************************************/

#ifdef IMPLICIT_SEQUENCER
class Sequencer;
#endif

class Instrument : public Phonic
{
   typedef Phonic super;            // superclass is Phonic

   public:

   KeyBrd  keybrd;                  // resident keyboard
   char    xpose;                   // transpose notes by this many intervals

   #ifdef IMPLICIT_SEQUENCER

   Sequencer *sqnc;                 // resident sequencer (optional)

   Instrument()
   {
      sqnc = NULL;                  // no sequencer by default
   }

   void    dynamics();              // update dynamics

   void  useSequencer(Sequencer*);  // assign a resident sequencer 

   #endif // IMPLICIT_SEQUENCER

   virtual void noteOn( key )  {};  // turn a note on
   virtual void noteOff( key ) {};  // turn a note off

   boolean charEv( char );          // process a character event
   boolean evHandler( obEvent );    // handle an onboard event

   #ifdef KEYBRD_MENUS
   char    menu( key );             // given a key, return a character 
   #endif

} ;

/******************************************************************************
 *
 *                            StereoInstrument 
 *                                                                            
 ******************************************************************************/

class StereoInstrument : public Instrument
{
   typedef Instrument super;        // superclass is Instrument

   public:

   using Instrument::output;

   virtual void output( char*, char* ); // write stereo output to pair of audio buffers

} ; 


#endif   // ifndef INSTRUMENT_H_INCLUDED
