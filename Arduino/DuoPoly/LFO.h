/*
    LFO.h  

    Declaration of the LFO class.

    ---------------------------------------------------------------------------
 
    Copyright (C) 2016, Cornfield Electronics, Inc.
 
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

class LFO : public Control
{
   public:

   double  val;                  // current output value 

   void    charEv( char );       // process a character event
   void    dynamics();           // update object dynamics
   virtual void evaluate();      // compute output value from osc position
   void    info();               // display object info to console
   virtual void iniPos();        // set initial osc position
   char    menu( key );          // map key event to character 
   void    iniOsc(double,double);// initialize oscillator
   void    setDepth( double );   // set oscillation depth
   void    setFreq( double );    // set oscillation frequency

   protected:

   double  depth;                // oscillation depth (0.0 - 1.0)
   double  pos;                  // cur position within oscillation range
   double  step;                 // change in pos per dynamic update

   private:

   double  freq;                 // oscillation frequency

   void    calcStep();           // calculate step 

} ;


class FadeLFO : public LFO       // LFO with fade capability
{
   public:

   enum { FADEOUT = _RESERVE1 }; // flags.FADEOUT = 1 if LFO fades out

   void    charEv( char );       // process a character event
   void    dynamics();           // update object dynamics
   void    info();               // display object info to console
   char    menu( key );          // map key event to character 

   protected:

   byte     time;                // fade time in 1/8th secs (0-255)
   double   fader;               // current fader value
   double   fadeStep;            // increment fader this much per dynamic upd

} ;


class TrigLFO : public LFO       // Triggered LFO
{
   public:

   enum {  // use these bits in the flags byte: 

           PEAK    = _RESERVE1,   // trigger LFO starting at peak value 
           LEGATO  = _RESERVE2    // ignore trigger if LFO is still running

        } ;

   void    charEv( char );       // process a character event
   void    dynamics();           // update object dynamics
   void    info();               // display object info to console
   void    iniPos();             // set initial osc position
   char    menu( key );          // map key event to character 

   protected:

   /* The traversal count is the # of half-cycles the LFO will traverse
      once triggered. For example, a traversal count of 2 means the
      LFO will run for 1 complete cycle (i.e. max -> min -> max, 
      or min -> max -> min), then stop. A traversal count of 0 means the
      LFO is free-running (i.e. does not respond to triggers).
   */

   byte    trav;                 // traversal count (0 - 255)
   byte    runDC;                // downcounter (in traversals) to end of run

} ;

#endif   // ifndef LFO_H_INCLUDED
