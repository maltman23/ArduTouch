/*
    LFO.h  

    Declaration of the LFO class.

    ---------------------------------------------------------------------------
 
    Copyright (C) 2016-2019, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
   ---------------------------------------------------------------------------
*/

#ifndef LFO_H_INCLUDED
#define LFO_H_INCLUDED

#include "Control.h"

#define LFO_MAX_WF      3           // max waveform # for built-in waveforms

// Symbolic constants specifying built-in LFO waveform types

#define LFO_TRI_WF      0           // waveform # for triangle wave 
#define LFO_SQ_WF       1           // waveform # for square wave
#define LFO_RSAW_WF     2           // waveform # for rising sawtooth 
#define LFO_FSAW_WF     3           // waveform # for falling sawtooth 

#define LFO_HIGHBIT     0x8000      // mask for selecting high bit of LFO::idx

/******************************************************************************
 *
 *                                 LFO 
 *                                                                            
 ******************************************************************************/

class LFO : public Factor
{
   typedef Factor super;         // superclass is Factor

   public:

   boolean charEv( char );       // process a character event
   void    dynamics();           // update object dynamics
   boolean evHandler( obEvent ); // handle an onboard event
   virtual void evaluate();      // compute output value from osc position
   byte    getDepth();           // get oscillation depth
   double  getFreq();            // get oscillation frequency
   byte    getWaveform();        // returns oscillator waveform #
   void    iniVal();             // set initial osc position and evaluate
   bool    isSigned();           // returns true if oscillator is signed
   void    revaluate();          // evaluate osc (considering if it is muted)
   void    setDepth( byte );     // set oscillation depth
   void    setFreq( double );    // set oscillation frequency
   void    setSigned( bool );    // set oscillator range as signed or unsigned
   void    setWaveform( byte );  // set oscillator waveform #

   #ifdef KEYBRD_MENUS
   char    menu( key );          // given a key, return a character 
   #endif

   PROMPT_STR( lfo ) 

   protected:

   void    setAmpMethodPtr();    // set *computeAmp() to proper method

   int    (LFO::*computeAmp)();  // ptr to one of following methods:

   int    _signTRI();            // returns amplitude for signed triangle wave
   int    _unsignTRI();          // returns amplitude for unsigned triangle wave
   int    _signSQ();             // returns amplitude for signed square wave
   int    _unsignSQ();           // returns amplitude for unsigned square wave
   int    _signRSAW();           // returns amplitude for signed rising sawtooth
   int    _unsignRSAW();         // returns amplitude for unsigned rising sawtooth
   int    _signFSAW();           // returns amplitude for signed falling sawtooth
   int    _unsignFSAW();         // returns amplitude for unsigned falling sawtooth

   byte    waveform;             // waveform type info

   // bits in waveform:

   enum   { SIGNED    = 0b00000100,  // if set, oscillation range is signed
          };

   byte    depth;                // oscillation depth (0-128)
   byte    instDepth;            // instantaneous oscillation depth (0-128)

   word    idx;                  // current position in oscillator cycle
   word    step;                 // change in idx per dynamic update

   private:

   double  freq;                 // oscillation frequency

} ;

/******************************************************************************
 *
 *                               FadeLFO 
 *                                                                            
 ******************************************************************************/

class FadeLFO : public LFO       // LFO with fade capability
{
   typedef LFO super;            // superclass is LFO

   public:

   enum {  // use these bits in the flags byte: 

         FADEOUT = _RESERVE4     // LFO fades out (if not set, LFO fades in)
                                 
        } ;

   boolean charEv( char );       // process a character event
   void    dynamics();           // update object dynamics
   void    evaluate();           // compute output value from osc position
   void    fadeIn();             // configure LFO to fade in
   void    fadeOut();            // configure LFO to fade out
   void    setFadeTime( byte );  // set time of fade

   #ifdef KEYBRD_MENUS
   char    menu( key );          // given a key, return a character 
   #endif

   protected:

   byte    time;                 // fade time in 1/8th secs
   word    fader;                // current fader value
   int     fadeStep;             // increment fader this much per dynamic upd

   void iniFader();              // initialize fader

} ;

/******************************************************************************
 *
 *                               TermLFO 
 *
 *   "Terminating LFO" - an LFO which can be configured to run for a specified 
 *   number of half-cycles once triggered.
 *
 ******************************************************************************/

class TermLFO : public LFO       // LFO that terminates after n half-cycles
{                                
   typedef LFO super;            // superclass is LFO

   public:

   enum {  // bits in the flags byte: 

           // the PHASE flag determines the beginning or end phase for LFOs 
           // with a non-zero traversal count (explained below). 
           //
           // for unsigned LFOs:
           //
           //        PHASE=1 means terminate at peak value
           //        PHASE=0 means terminate at trough value
           //
           // for signed LFOs:
           //
           //        PHASE=1 means trigger at start of waveform
           //        PHASE=0 means trigger at "0.0"

           PHASE  = _RESERVE4    // determines start/end phase

        } ;

   bool  charEv( char );         // process a character event
   void  dynamics();             // update object dynamics
   void  setTravCount( byte );   // set traversal count

   #ifdef KEYBRD_MENUS
   char   menu( key );           // given a key, return a character 
   #endif

   protected:

   bool  fullCycle();            // returns true if traversal is multiple of full cycle
   bool  sawtooth();             // returns true if waveform is a sawtooth

   /* The traversal count is the # of half-cycles the LFO will traverse
      once triggered. A half-cycle is half a period (180 degrees). 
      
      So a traversal count of 2 means the LFO will run for 1 complete period 
      (i.e. max -> min -> max, or min -> max -> min), then stop. 
      
      A traversal count of 0 means the LFO is free-running.
   */

   byte    trav;                 // traversal count 
   byte    runDC;                // downcounter (in half-cycles) to end of traversal

} ;

#endif   // ifndef LFO_H_INCLUDED
