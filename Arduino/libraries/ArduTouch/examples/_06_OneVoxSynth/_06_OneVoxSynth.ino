//
//  _06_OneVoxSynth.ino
// 
//  Example 06 of introductory examples
//     to learn basics of programming your own ArduTouch synthesizers
//  To get the most of these exmples, please go through them in sequence. 
//
// 
//  This example synth is the first in this introductory series that is what 
//  most people would think of as a "real" synthesizer.
//
//  This sketch shows how to use a OneVoxSynth to produce a sawtooth waveform,
//  which can then be played using the keyboard.
//
//  A OneVoxSynth uses a "voice", which allows for a lot of cool stuff!
//  See the comments at the bottom for more info.
//  We'll see more of what it can do in later examples.
//
//  By using OneVoxSynth, our sketch becomes very simple!
//
//  NOTE:  This sawtooth waveform only sounds OK for bass notes.
//         There are much better sounding oscillators available.
//         We'll hear one in the next example.
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

about_program( OneVoxSynth, 1.00 )        // specify sketch name & version

class SawToothSynth : public OneVoxSynth  // output a sawtooth waveform using a OneVoxSynth
{                                         
   public:

   // newOsc() is called by the system during setup.
   // This is where you specify what oscillator to use.
   // In this example we use a SawTooth.

   Osc* newOsc( byte nth )                // (the argument "nth" isn't really used here, but is useful for when we have more than one voice)
   {
      return new SawTooth();              // use a SawTooth oscillator
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


// OneVoxSynth is a high-level class which allows you to use the keyboard
// to play notes. Tapping the left button lowers the keyboard by an octave,
// tapping the right button raises the keyboard by an octave.
// Also with OneVoxSynth, the top pot is a volume control by default.
