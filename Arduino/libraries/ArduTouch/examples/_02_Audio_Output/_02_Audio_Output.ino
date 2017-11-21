//
//  _02_Audio_Output.ino
// 
//  This sketch demonstrates how to create a simple synthsizer that produces 
//  audio output.
//
//  A synthesizer is constructed which plays a saw-tooth waveform at a constant 
//  frequency.
//
//  Target:   ArduTouch board
// 
//  ---------------------------------------------------------------------------
// 
//  Copyright (C) 2016, Cornfield Electronics, Inc.
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

about_program( Audio Output, 1.00 )       // specify sketch name & version
set_baud_rate( 115200 )                   // specify serial baud-rate

//  ---------------------------------------------------------------------------
//
//  Our synthesizer will generate a saw-tooth waveform with a period of 256 
//  ticks. It does this by incrementing an 8-bit output value each tick.
//
//  It so happens that the audio rate used by the ArduTouch library is 15688.3 Hz, 
//  and is given by a library constant:
//
//           double audioRate = 15688.3;  // defined by library
//
//  Knowing this, we can calculate the fundamental frequency for the saw-tooth
//  waveform that this synthesizer will generate:
//
//      frequency of waveform = audioRate  / period of waveform
//                            = 15688.3 Hz / 256
//                            = 61.28 Hz
//
//  In this simple example, we will use 2 of 5 "call back" methods used by the 
//  ArduTouch firmware system:  setup() and output().  (See below for a bit more 
//  about "call back" methods.) Upcoming examples will show how to use other 
//  "call back" methods.
//
//  ---------------------------------------------------------------------------      

class SawToothSynth : public Synth
{
   public:

   char  outVal;                          // current output value


   // this setup() "call back" method will be called once when 
   // the ardutouch_setup() function is executed

   void setup()                           // initialize synth
   { 
      outVal = 0;                         // initial output value = 0
   }


   // Our output() "call back" method is automatically called by ardutouch_loop() 
   // whenever the ArduTouch system needs an audio buffer to be filled.
   // The size of the buffer is given by the library constant audioBufSz.
   // In general, this method needs to be as short and efficient as possible!

   void output( char *buf )               // output 1 buffer of audio
   {
      byte icnt = audioBufSz;             // icnt = # bytes in buffer
      while ( icnt-- )                    // for each byte in buffer
         *buf++ = outVal++;               // write output value to buffer,
                                          // and bump it (wraps at 127)
   }

} mySawtoothSynth;                        // instantiate synthesizer 


//  ---------------------------------------------------------------------------
//
//  In the setup() function below, ardutouch_setup() is passed the address of 
//  our synth. This address is used by ardutouch_setup() to execute a "call back"
//  to the setup() method of our synth.
//
//  ardutouch_loop() will then execute "call backs" to the output() method 
//  of our synth, feeding it new audio buffers to be filled. 
//   
//  Both setup() and output() are virtual methods of the Synth class, which is 
//  defined by the library.
//
//  If our synth was designed to produce stereo audio, then we would not use
//  the output() method, but would instead use the following call-back method:
//
//             void outputs(char *bufLeft, char *bufRight)
//
//  We will see this in a later example.
//
//  ---------------------------------------------------------------------------      

void setup()
{
   ardutouch_setup( &mySawtoothSynth );   // initialize ArduTouch resources
}

void loop()
{                                         
   ardutouch_loop();                      // perform ongoing ArduTouch tasks  
}                                             

//  ---------------------------------------------------------------------------
//
//  If you compile and upload this sketch to the ArduTouch you should hear 
//  a saw-tooth tone at 61.28 Hz.
//
//  ---------------------------------------------------------------------------      


