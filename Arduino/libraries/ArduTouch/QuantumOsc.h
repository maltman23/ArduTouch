/*
    QuantumOsc.h  

    Declaration of Quantum Oscillator classes.

    A quantum oscillator is restricted to only those frequencies with
    wavelengths which are an integral number of audio cycles long.

    ---------------------------------------------------------------------------
 
    Copyright (C) 2018, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
   ---------------------------------------------------------------------------
*/

#ifndef QUANTUMOSC_H_INCLUDED
#define QUANTUMOSC_H_INCLUDED

#include "Osc.h"

/******************************************************************************
 *
 *                                QuantumOsc 
 *                                                                            
 ******************************************************************************/

class QuantumOsc : public Osc       
{
   typedef Osc super;               // super class is Osc

   public:

   bool  tracking;                  // if true, frequency tracking is enabled

   bool   charEv( char code );      // process a character event
   double periodHz( word period );  // return the freq of a period 
   void   onFreq();                 // compute frequency dependent state vars

   virtual void track( double ) {}; // set osc to freq nearest target freq

} ;


/******************************************************************************
 *
 *                                PureSquare 
 *                                                                            
 ******************************************************************************/

class PureSquare : public QuantumOsc               
{
   typedef QuantumOsc super;        // super class is QuantumOsc

   protected:

   byte flipDC;                     // downcounter to next edge flip
   byte edgLen;                     // # of ticks of output per edge
   char edgVal;                     // current output value for edge
   byte peak;                       // peak amplitude of waveform

   public:

   bool   charEv( char code );      // process a character event
   void   output( char *buf );      // write one buffer of output
   double realFreq();               // return the real frequency
   void   setEdgLen( byte );        // set the edge length
   void   setPeak( byte );          // set the peak amplitude
   void   track( double freq );     // set osc to freq nearest target freq

   PROMPT_STR( pureSq ) 

} ;

/******************************************************************************
 *
 *                                 SweepOsc 
 *                                                                            
 ******************************************************************************/

class SweepOsc : public PureSquare 
{
   typedef PureSquare super;        // super class is PureSquare

   protected:

   byte  begin;                     // beginning edge length
   byte  end;                       // ending edge length
   byte  step;                      // step edgLen this much per dynamic update

   bool  autotrig;                  // auto trigger on reaching end
   byte  repeats;                   // # of times to auto trigger (0=forever)
   byte  repsToGo;                  // # of repeats remaining

   byte  stretch;                   // stretch factor (# updates between edge bumps)
   byte  bumpDC;                    // downcounter to bump edge length

   public:

   virtual void setBegin( byte );   // set beginning edge length of sweep
   virtual void setEnd( byte );     // set ending edge length of sweep
   virtual void startSweep();       // initiate a new sweep from begin to end

   bool autoTrigEnabled()           { return autotrig; }

   bool charEv( char code );        // process a character event
   void dynamics();                 // perform a dynamic update
   bool evHandler( obEvent ev );    // handle an onboard event
   void loadEdge( byte val );       // set the current edge length
   void onFreq() {};                // compute frequency dependent state vars
   void setRepeats( byte val );     // set # times to play upon trigger
   void setStretch( byte val );     // set the stretch factor

   PROMPT_STR( sweep ) 

   private:

   void calibrate();                // reconcile step and edgLen with begin/end

} ;
  
#endif   // ifndef QUANTUMOSC_H_INCLUDED
