//
//  _71_PureSquare.ino
//
//  The PureSquare oscillator is an example of a "quantum" oscillator.
//
//  It oscillates only at frequencies whose periods are an integral and even
//  number of samples.
//
//  This synth uses a PureSquare oscillator, and shows how to set its frequency
//  by specifying its edge length.
//
//  The top pot controls the edge length.
//
//  Target:   ArduTouch board
// 
//  ---------------------------------------------------------------------------
// 
//  Copyright (C) 2018, Cornfield Electronics, Inc.
// 
//  This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
//  Unported License.
// 
//  To view a copy of this license, visit
//  http://creativecommons.org/licenses/by-sa/3.0/
// 
//  Created by Bill Alessi & Mitch Altman.
// 
//  ---------------------------------------------------------------------------

#include "ArduTouch.h"                       // use the ArduTouch library 

about_program( PureSquare, 1.00 )            // specify sketch name & version

class PureSquareSynth : public Synth
{
   typedef Synth super;                      // super class is Synth

   public:

   PureSquare osc;                           // use 1 pure square wave oscillator 

   void welcome()
   {
      osc.reset();                           // initialize osc 
      osc.setFreq( 440.0 );                  // start osc at (about) 440 Hz
   }

   bool evHandler( obEvent e )               // event handler
   {
      switch ( e.type() )
      {                                      
         // set the oscillator frequency by specifying the length of an edge
         // via the top pot (POT0)
         //
         // note that   period = length of edge * 2  
         //           oec freq = audioRate / period 
         
         case POT0:                          
            osc.setEdgLen( e.getPotVal() );
            break;
         default:       
            return Synth::evHandler(e);      // pass other events through
      }
      return true;
   }

   void output( char *buf )                  // output 1 buffer of audio
   {
      osc.output( buf );                     // pass buffer to oscillator
   }


} myPureSquareSynth;


void setup()
{
   ardutouch_setup( &myPureSquareSynth );    // initialize ArduTouch resources
}

void loop()
{
   ardutouch_loop();                         // perform ongoing ArduTouch tasks  
}                                             

//  ---------------------------------------------------------------------------
//
//  If you compile and upload this sketch to the ArduTouch you should hear a 
//  square wave tone at about 440 Hz. Turning the top pot will alter the
//  length of the square wave edge, and thus the frequency of the tone.
//
//   --------------------------------------------------------------------------



