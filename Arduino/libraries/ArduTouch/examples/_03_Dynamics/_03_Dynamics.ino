//
//  _03_Dynamics.ino
// 
//  Example 03 of introductory examples
//     to learn basics of programming your own ArduTouch synthesizers
//  To get the most of these exmples, please go through them in sequence. 
//
// 
//  This sketch shows how to dynamically change the sound of your synth.
//  Dynamics is what makes your synthesizer sound cool!
//
//  This example sketch adds a simple dynamics example, which is making the volume
//  of a sawtooth wave go from nothing to max repeatedly.
//
//  Please see comments at the bottom for more details.
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

about_program( Dynamics, 1.00 )           // specify sketch name & version

class DynaToothSynth : public Synth       // output a sawtooth waveform whose volume
{                                         // dynamically ramps from 0 to max
   public:

   char  sample;                          // sample value 
   byte  volume;                          // volume level

   // this is where we will change the volume of the sawtooth waveform
   // FYI: dynamics is called by the system about 150 times per second
   void dynamics()                        // perform a dynamic update
   {
      volume++;                           // bump volume level
   }

   // as in the previous example 02, this creates a sawtooth waveform in an audio buffer,
   // but this time modifying the sawtooth by the volume
   void output( char *buf )               // output 1 buffer of audio
   {
      byte icnt = audioBufSz;             // audioBufSz is a library constant
      while ( icnt-- )                    // for each byte in buffer
      {                                   // ...
         int sampleXV = sample * volume;  // multiply sample value by volume level 
         *buf++ = sampleXV >> 8;          // write high byte of multiplied sample to buffer
         sample++;                        // bump sample value
      }
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


//  The dynamics() method of your synth is called by the system at regular,
//  timed intervals of approximately 150 times per second. The dynamics() 
//  method is where you should perform all the calculations involving the 
//  changing state of your synth, such as updating envelopes, LFOs, and 
//  just about anything that makes a sound interesting. 
//    
//  The dynamics() method, together with the output() method, is where "all 
//  the action takes place" in forming the sound of your synth.
//
//  Without dynamics the output of the synthesizer is static and unchanging.
//
//  In this example, we use the dynamics() method to ramp the volume of a
//  sawtooth generator.
//


//  --------------------------------------------------------------------------
//
//                 A Note about the Dynamic Update Rate
//
//  When you write your own dynamic effects, to properly calibrate your 
//  algorithms you may need to know the exact rate at which the system calls 
//  your dynamics() method. This rate is given by the system constant:
//
//          double dynaRate;              // system dynamic update rate
//
//  ---------------------------------------------------------------------------



