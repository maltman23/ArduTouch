/*
    Sequencer.h  

    Declaration of Sequencer classes.

    ---------------------------------------------------------------------------
 
    Copyright (C) 2016, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
   ---------------------------------------------------------------------------
*/

#ifndef STEPSQNC_H_INCLUDED
#define STEPSQNC_H_INCLUDED

#include "Control.h"
#include "Instrument.h"

/* sequence record tokens */

#define tokenRest    253         // rest token (# jiffs follows in next byte)
#define tokenUnMute  254         // unMute target
#define tokenEOS     255         // end-of-sequence token

/******************************************************************************
 *
 *                                 Sequencer
 *
 ******************************************************************************/

class Sequencer : public Mode    
{
   typedef Mode super;           // superclass is Mode

   public:
   
   Instrument *target;           // ptr to instrument being sequenced 

   Sequencer( byte bufSz = 66 )
   {
      sqnc = (byte *)malloc( bufSz );
      sqnc[0] = bufSz;
   }

   inline byte bufSize()         // returns size of sequence buffer
   { 
      return sqnc[0]; 
   }

   bool   charEv( char );        // process a character event
   double getTempo();            // get sequencer playback tempo 
   byte   length();              // returns length of compiled sequence
   bool   load( const byte* );   // load a sequence from ROM
   bool   read( word );          // read a sequence from EEPROM
   void   record();              // record a new sequence
   void   setTargetMuting(bool); // enable/disable target muting
   void   setTempo( double );    // set sequencer playback tempo 
   void   start();               // start sequencer playback
   void   stop();                // stop sequencer playback 
   void   write( word );         // write sequence to EEPROM
   void   _pause_();             // toggle pausing of sequencer playback

   virtual void  dynamics();     // perform a dynamic update 

   #ifdef CONSOLE_OUTPUT
   const char *prompt();
   #endif

   bool   playing() { return ( playback == pbON ); }

   protected:

   byte    *sqnc;                // buffer for compiled sequence records

   static  const byte pbOFF    = 0;  // playback status values
   static  const byte pbON     = 1; 
   static  const byte pbPAUSED = 2; 
   
   byte    playback;             // playback status

   // The sequencer tempo is measured in jiffs ("in a jiffy") per minute.
   // A jiff is the smallest unit of time kept by the sequencer.
   // The duration of notes and rests in a sequence are quantized in jiffs.

   word    ticksPerJiff;         // # ticks per jiff
   word    ticksPerHalf;         // # ticks per 1/2 jiff
   double  tempo;                // sequencer tempo (jiffs per minute)

   bool    targetMuting;         // enable target muting
   bool    unMute;               // unMute target when encountering tokenUnMute  

   word    jiffDC;               // downcounter (in audio ticks) to next jiff
   byte    exeDC;                // downcounter (in jiffs) to next record execution

   byte    idx;                  // index of next record in sqnc[] 

   void    exeRec();             // execute next record in sqnc[]
} ;

/******************************************************************************
 *
 *                                StepSqnc
 *
 ******************************************************************************/

class StepSqnc : public Sequencer    
{
   typedef Sequencer super;      // superclass is Sequencer

   using Sequencer::Sequencer;

   public:

   bool   charEv( char );        // process a character event

} ;

#endif   // ifndef STEPSQNC_H_INCLUDED
