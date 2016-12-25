//
//  _09_Octaves.ino
// 
//  This sketch explains the system octave setting, and how to use it to
//  turn the ArduTouch's single octave into a multi-octave keyboard.
//
//  In this example, a synth is constructed in which the keyboard can be used 
//  to play tones in an equal temperament tuning. The octave of the keyboard 
//  can be lowered by tapping the left button, and raised by tapping the right 
//  button.
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

#include "ArduTouch.h"                       // use the ArduTouch library 

about_program( Octaves, 1.00 )               // specify sketch name & version
set_baud_rate( 115200 )                      // specify serial baud-rate

class MultiOctaveSynth : public MonoSynth
{
   public:

   WaveOsc osc;                              // use 1 wave oscillator 

   void setup() 
   {                                         
      osc.setTable( wave_descriptor(dVox) ); // use dVox wavetable from library
      osc.setFreq( 440.0 );
   }

   // The system maintains a current octave setting for the keyboard. 
   // There are three library routines which can be used to access or change 
   // the octave setting:
   //
   //       byte getOctave();                // return current octave
   //       void setOctave( byte n );        // set current octave
   //       void upOctave();                 // raise keyboard by 1 octave
   //       void downOctave();               // lower keyboard by 1 octave
   //
   // KEY_DOWN and KEY_UP events specify both the key's position in the 
   // octave, and the octave of the note. Given an obEvent e, whose type is
   // KEY_DOWN or KEY_UP, the key position and octave can be retrieved
   // by e.getKey().position() and e.getKey().octave()
   // The above is a shorthand way, in the C language, of saying:
   //
   //      key k    = e.getKey();
   //      byte pos = k.position();
   //      byte oct = k.octave();
   //
   // In past examples we haven't seen it, but the key struct returned by e.getKey() 
   // has both key position and key octave.  In this example, we are changing the octave 
   // info passed along with getKey().

   boolean evHandler( obEvent e )            // event handler
   {
      switch ( e.type() )
      {
         case BUT0_TAP:                      // left button was tapped

            downOctave();                    // lower keyboard by 1 octave
            return true;

         case BUT1_TAP:                      // right button was tapped

            upOctave();                      // raise keyboard by 1 octave
            return true;

         case KEY_DOWN:                      // a key has been pressed

            osc.setFreq( masterTuning->pitch( e.getKey() ) ); 
            return true;                     

         default:       

            return MonoSynth::evHandler(e);  // pass other events to base class
      }
   }

   void output( char *buf )                  // output 1 buffer of audio
   {
      osc.output( buf );                     // pass buffer to oscillator
   }

} synth;

//  ---------------------------------------------------------------------------
//
//                           A Note about Octaves
//
//  The system octave setting can range from 0 to 7. The default octave is 4.
//  ("Middle C" on a piano is the lower C of octave 4).
//
//  ---------------------------------------------------------------------------

void setup()
{
   ardutouch_setup( &synth );                   // initialize ardutouch resources
}

void loop()
{
   ardutouch_loop();                            // perform ongoing ardutouch tasks  
}                                             

//  ---------------------------------------------------------------------------
//
//  If you compile and upload this sketch to the ArduTouch you should hear a 
//  synthetic-choir like tone at 440 Hz. Pressing the keys will play tones in an
//  equal temperament tuning. Tapping the right button will raise the keyboard
//  by 1 octave. Tapping the left button will lower the keyboard by 1 octave.
//
//  ---------------------------------------------------------------------------
