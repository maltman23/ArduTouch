//
//  _07_WaveOsc.ino
// 
//  Example 07 of introductory examples
//     to learn basics of programming your own ArduTouch synthesizers
//  To get the most of these exmples, please go through them in sequence. 
//
// 
//  This sketch shows how to use a more interesting waveform in a OneVoxSynth.
//
//  In the previous example (06) we used a built-in oscillator that generated
//  a sawtooth waveform.  In this example synthesizer, we will use a short 
//  recording of a waveform to generate sounds.  The short recording is stored
//  in memory.  This is called "Wave Table synthesis".  For this example we are 
//  using a built-in recording called: dVox.
//
//  To make use of dVox, we will use the "WaveOsc" class, which can play any wave
//  table in the ArduTouch library.
//
//  This will sound way cooler than the sawtooth!  But since this example does
//  not have any dynamics, it is not as nice as it can be.  We'll get to this soon!
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

#include "ArduTouch.h"                    // use the ArduTouch library 

about_program( WaveOsc, 1.00 )            // specify sketch name & version

class WaveOscSynth : public OneVoxSynth   // output a wavetable from the library
{                                         
   public:

   Osc* newOsc( byte nth )
   {
      WaveOsc *o = new WaveOsc();         // use a wavetable oscillator
      o->setTable( wavetable( dVox ) );   // with "dVox" wavetable from library
      return o;              
   }

} mySynth;                                // instantiate synthesizer 


void setup()
{
   ardutouch_setup( &mySynth );           // initialize ArduTouch resources
}


void loop()
{
   ardutouch_loop();                      // perform ongoing ArduTouch tasks  
}                                             

