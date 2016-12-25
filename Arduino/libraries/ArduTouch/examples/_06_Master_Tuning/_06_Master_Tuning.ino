//
//  _06_Master_Tuning.ino
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
set_baud_rate( 115200 )                      // specify serial baud-rate

class EqualTempSynth : public MonoSynth
{
   public:

   WaveOsc osc;                              // use 1 wave oscillator 

   void setup() 
   {                                         
      osc.setTable( wave_descriptor(Rood) ); // choose the Rood wavetable from library
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
         case KEY_DOWN:                      // a key has been pressed

            osc.setFreq( masterTuning->pitch( e.getKey() ) ); 
            return true;                     

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



