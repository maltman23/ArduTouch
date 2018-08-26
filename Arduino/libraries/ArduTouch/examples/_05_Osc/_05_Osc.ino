//
//  _05_Osc.ino
// 
//  Example 05 of introductory examples
//     to learn basics of programming your own ArduTouch synthesizers
//  To get the most of these exmples, please go through them in sequence. 
//
// 
//  This sketch shows how to use a pre-built oscillator to generate sound.
//
//  In the previous example synthesizers (02, 03, and 04), we mathematically 
//  calculated a sawtooth waveform.  In this example synthesizer we will use 
//  a pre-built oscillator (provided by the ArduTouch library) to generate a 
//  sawtooth waveform.
//
//  With the pre-built SawTooth oscillator (or any pre-built oscillator), we
//  can now change the waveform's frequency.
//
//  In this example synthesizer, we will set the sawtooth frequency to 100 Hz,
//  and then be able to vary the frequency with the top pot on the ArduTouch 
//  board. 
//
//  We also introduce here the welcome() function, which is called only once,
//  when the ArduTouch is powered on (or when the reset button is pushed).  
//  This can be used for some initialization. (We will see in a later example 
//  that there are better ways to perform initialization.)
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

about_program( Osc, 1.00 )                // specify sketch name & version

class OscSynth : public Synth             // output a sawtooth waveform from an oscillator
{                                         // and control its frequency by the top pot
   public:

   SawTooth osc;                          // a SawTooth oscillator (defined in ArduTouch library)

   // welcome() is called once by the system when your ArduTouch board is powered on (or when the reset button is pushed)

   void welcome()
   {
      osc.reset();                        // initialize oscillator
      osc.setFreq( 100.0 );               // set initial freqeuncy to 100.0 Hz
   }

   // the evHandler() here is used to control the sawtooth waveform's pitch (frequency)
   boolean evHandler( obEvent e )         // event handler
   {
      switch ( e.type() )                 // branch on event type
      {
         case POT0:                       // top pot was moved

            osc.setFreq( e.getPotVal() ); // set freq from pot value (0-255)
            return true;                  // event was handled
            break;

         default:                         // pass all other events to normal Synth handler

            return Synth::evHandler(e); 
      }
   }

   // FYI: now that we are using a pre-built osc, we no longer need to calculate anything
   // to fill the audio buffer with a waveform -- the osc does it all for us
   void output( char *buf )               // output 1 buffer of audio
   {
      osc.output( buf );                  // pass buffer to oscillator
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

