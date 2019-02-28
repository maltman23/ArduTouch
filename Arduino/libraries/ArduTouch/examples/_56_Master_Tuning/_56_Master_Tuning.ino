//
//  _56_Master_Tuning.ino
// 
//  This sketch shows how to use the system's built-in master tuning object to
//  convert key events into frequencies corresponding to an equal temperament 
//  tuning (which is "normal" tuning for Western musical scales).
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
// 

#include "ArduTouch.h"                       // use the ArduTouch library 

about_program( Master Tuning, 1.00 )         // specify sketch name & version

class EqualTempSynth : public Synth
{
   public:

   WaveOsc osc;                              // use 1 wave oscillator 

   void config() 
   {
      osc.reset();
      osc.setTable( wavetable(Rood) );       // choose the Rood wavetable from library
      osc.setFreq( 440.0 );
   }

   // masterTuning is a library variable which points to the current Tuning 
   // object for the system. The Tuning class contains a pitch() method
   // which takes a key struct as an argument, and returns a frequency.
   // The default Tuning object implements equal temperament tuning.

   boolean evHandler( obEvent e )            // event handler
   {
      switch ( e.type() )
      {
         // on a key down event we fetch the key information, set the octave
         // of the key to 4, pass the key to the master tuning object's
         // pitch method and set the oscillator's frequency to this value.
         //
         // Note: later examples will demonstrate how to use the ArduTouch's
         //       built-in multi-octave keyboard routines, so that you don't
         //       need to set the octave.

         case KEY_DOWN:                      // a key has been pressed
         {
            key myKey = e.getKey();          // get key from event
            myKey.setOctave( 4 );            // set the key's octave to 4

            osc.setFreq( masterTuning->pitch( myKey ) ); 
            return true;                     
         }

         default:       
            return false;                    // pass over all other events
      }
   }

   void output( char *buf )                  // output 1 buffer of audio
   {
      osc.output( buf );                     // pass buffer to oscillator
   }

} myEqualTempSynth;


//  --------------------------------------------------------------------------
//
//                           A Note about Tuning
//
//  You can write your own tuning, for example using quarter-tone intervals 
//  instead of semi-tones (which is the "normal" tuning in Western musical scales).
//  In a later example we will demonstrate how to do this.
//
//  --------------------------------------------------------------------------      

void setup()
{
   ardutouch_setup( &myEqualTempSynth );     // initialize ArduTouch resources
}

void loop()
{
   ardutouch_loop();                         // perform ongoing ArduTouch tasks  
}                                             

//  ---------------------------------------------------------------------------
//
//  If you compile and upload this sketch to the ArduTouch you should hear a 
//  wood-wind like tone (which we call Rood) at 440 Hz. Pressing the keys will 
//  play tones in an equal temperament tuning.
//
//  ---------------------------------------------------------------------------



