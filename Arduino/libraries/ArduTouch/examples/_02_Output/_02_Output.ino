//
//  _02_Output.ino
//
//  Example 02 of introductory examples
//     to learn basics of programming your own ArduTouch synthesizers
//  To get the most of these exmples, please go through them in sequence. 
//
// 
//  This example sketch shows how to produce audio output. 
//  This example synthesizer will create a sawtooth wave,
//    just one note that cannot be changed with the touch keyboard
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

about_program( Output, 1.00 )             // specify sketch name & version

class SawToothSynth : public Synth        // output a sawtooth waveform
{
   public:

   char  sample;                          // sample value used in output()

   // to make a sound with ArduTouch, you simply fill an audio buffer 
   // (given to you by the system) with your desired waveform.

   // whenever the ArduTouch system needs an audio buffer to be filled
   // output() is automatically called by ardutouch_loop().

   // the following fills the audio buffer with a sawtooth waveform
   void output( char *buf )               // output 1 buffer of audio
   {
      byte icnt = audioBufSz;             // audioBufSz is a library constant
      while ( icnt-- )                    // for each byte in buffer
         *buf++ = sample++;               // write sample to buffer,
                                          // and bump it (wraps at 127)
   }

} mySynth;                                // instantiate synthesizer 


// remember to only have this one initialization line in setup()

void setup()
{
   ardutouch_setup( &mySynth );           // initialize ArduTouch resources
}


// remember to only have this one line in loop()

void loop()
{                                         
   ardutouch_loop();                      // perform ongoing ArduTouch tasks  
}                                             


//  ---------------------------------------------------------------------------
//
//  This synthesizer generates a saw-tooth waveform with a period of 256 samples.
//
//  The audio rate used by the ArduTouch library is 15688.3 Hz and is given 
//  by a library constant:
//
//           double audioRate = 15688.3;  // defined by library
//
//  Knowing this, we can calculate the fundamental frequency for the saw-tooth
//  waveform that this synthesizer generates:
//
//      frequency of waveform = audioRate  / period of waveform
//                            = 15688.3 Hz / 256
//                            = 61.28 Hz
//
//  ---------------------------------------------------------------------------      


