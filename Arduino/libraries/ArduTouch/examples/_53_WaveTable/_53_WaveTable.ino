//
//  _53_WaveTable.ino
// 
//  This sketch demonstrates how to create a wave table, and how to define a 
//  simple synthesizer which plays that wave table as a 440 hz tone.
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

about_program( WaveTable, 1.00 )          // specify sketch name & version

//  ---------------------------------------------------------------------------
//
//  First we create a simple wave table consisting of 2 samples. 
//  Samples are signed, 8-bit values.
//
//  ArduTouch library algorithms expect sample arrays to repeat the 1st 
//  sample at the end, so our wave table of 2 samples will actually have 3 
//  entries. 
//
//  The library macros begin_samples() and end_samples() are used to simplify
//  the syntax for creating a sample array. The argument for each macro is the 
//  name of the wave table being created.
//
//  ---------------------------------------------------------------------------
   
begin_samples( Triangle )                   

   127, -127,
   127                                    // always repeat 1st sample at end

end_samples( Triangle )


//  ---------------------------------------------------------------------------
//
//  Next we create a wave-table descriptor which provides information on how to
//  interpret the array of samples.
//
//  A wave-table descriptor has 3 fields:
//   
//            A) the address of the array of samples
//            B) the # of samples in the wave-table (excluding repeat entry at end)
//            C) the # of samples in a single wavelength
//
//  The library macro define_wavetable() is used to simplify the syntax. 
//
//  ---------------------------------------------------------------------------      

define_wavetable( Triangle, 2, 2.0 )        // name, # samples, wavelength


//  ---------------------------------------------------------------------------
//
//  Now we define our synthesizer, TriangleSynth. 
//   
//  We give TriangleSynth a single member: a wave-table oscillator (which is
//  part of the ArduTouch library).
//
//  In the setup() method we set the oscillator's wave-table to Triangle 
//  (using the macro wavetable() in the argument), and then set the oscillator's 
//  frequency to 440 Hz.
//
//  The ArduTouch library contains a class called WaveOsc. When we want to 
//  generate sound using a wave-table, we instantiate a WaveOsc, and assign it 
//  a table describing the waveform. For this example, we are using a triangle 
//  waveform.
//
//  Our output() method passes the audio buffers to osc, which performs 
//  the actual computations for generating the tone.
//
//  ---------------------------------------------------------------------------      

class TriangleSynth : public Synth
{
   WaveOsc  osc;                             // a wave-table oscillator

   public:

   // this setup() method sets up our triangle wave at 440.0Hz

   void setup() 
   { 
      osc.reset();
      osc.setTable( wavetable( Triangle ) ); 
      osc.setFreq( 440.0 );
   }

   // this output() method automatically fills buffers with our triangle waveform 
   // as the ArduTouch system needs them filled

   void output( char *buf )                  // output 1 buffer of audio
   {
      osc.output( buf );
   }

} myTriangleSynth;                           // instantiate synthesizer 


void setup()
{
   ardutouch_setup( &myTriangleSynth );      // initialize ArduTouch resources
}


void loop()
{
   ardutouch_loop();                         // perform ongoing ArduTouch tasks  
}                                             


//  --------------------------------------------------------------------------
//
//  If you compile and upload this sketch to the ArduTouch you should hear 
//  a triangle wave tone at 440 Hz.
//
//  --------------------------------------------------------------------------


//  --------------------------------------------------------------------------
//
//                           Library Wave Tables
//
//  You don't have to make your own wave table from scratch. The ArduTouch 
//  library comes with the following ready-to-use wave tables:
//
//             Sine 
//             dVox
//             Ether
//             Rood
//             SynStr
//
//  Any of these can be used as arguments to the wave_descrptor() macro.
//  Please try them -- they are cool sounds!
//
//  Also, try changing the above Triangle array of samples and hear what it 
//  sounds like -- there's lots of cool (and wonderfully horrible) sounds 
//  that you can create!
//
//  --------------------------------------------------------------------------


