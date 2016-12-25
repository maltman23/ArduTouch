//
//  _10_Multi_Octave_Keybrd.ino
//
//  In the last example _9_ we explicitly coded octave changes when we tapped
//  the buttons.  In this example we will allow the ArduTouch library to use 
//  its defaults, which automatically uses the buttons to change the octave up
//  or down.  This default behavior happens when we simply pass through 
//  "unhandled events"
//  
//  Though we aren't explicitly using this information in this example, we are
//  taking the opportunity to introduce the concept of the ArduTouch class 
//  hierarchy.  We'll also introduce the flags byte that is a member of the base 
//  class of the hierarchy.
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

about_program( Multi Octave Keybrd, 1.00 )   // specify sketch name & version
set_baud_rate( 115200 )                      // specify serial baud-rate

class MultiOctaveSynth : public MonoSynth
{
   public:

   WaveOsc osc;                              // use 1 wave oscillator 

   //               Introduction to ArduTouch Class Hierarchy
   //
   // All the library classes used to construct a synth and its components 
   // derive from a common base class named Mode (which provides a framework 
   // for defining the user interfaces for the particular objects of your 
   // synthesizer).  
   //
   // There is another class called Control that is derived from Mode. This class 
   // allows you to reset an object, mute an object (turn it off), unmute an object 
   // (turn it on), and add dynamics.
   //
   // For example, the class hierarchy for MonoSynth is:
   //
   //       Mode <- Control <- Phonic <- Synth <- MonoSynth
   //
   // and the class hierarchy for Vibrato is:
   //
   //       Mode <- Control <- TControl <- LFO <- FadeLFO <- Vibrato
   //
   // We are not concerned here with the intermediate classes in the above hierachies
   // only with Mode and Control.
   //
   // The evHandler() method we have been working with is a virtual method  
   // of Mode. By design ArduTouch classes pass unhandled events through to
   // the class they inherited from, so that Mode::evHandler() ultimately
   // gets called for any unhandled event. By "unhandled event, we mean any 
   // event that is not explicitly processed in your synthesizer code 
   // (but may be handled by default by the ArduTouch library).
   //
   // The Mode class contains an instance variable named: flags. 
   // This is a byte whose bits are used by the system for interpreting the 
   // current user interface (such as vibrato, as we've seen in a previous 
   // example). One of these bits is named PLAYTHRU. By default this bit is 
   // set (high). If the PLAYTHRU bit is set, the system intercepts BUT0_TAP 
   // and BUT1_TAP events, converts them into special octave-shifting "meta" 
   // events, and sends these meta events to evHandler(). 
   // Mode::evHandler() contains logic for automatically processing meta events. 
   // In the case of octave-shifts the button presses are handled by calling the 
   // system's bumpOctave() routine.
   //
   // The above info is provided for you know so that you can see a bit of what's
   // happening under the hood. This info will be repeated again when we come to 
   // examples that explicitly use it (and it will make more sense when we do).
   
   void setup() 
   {                                         
      osc.setTable( wave_descriptor(dVox) ); // use dVox wavetable from library
      osc.setFreq( 440.0 );                  // osc starts at A440
   }

   // In the event handler below we pass unhandled events to the class we
   // inherited from. This insures that meta events (which are generated 
   // by tapping on the left and right buttons) are ultimately handled 
   // in a standard way by Mode::evHandler(). 

   boolean evHandler( obEvent e )            // event handler
   {
      switch ( e.type() )
      {
         case KEY_DOWN:                      // a key has been pressed

            osc.setFreq( masterTuning->pitch( e.getKey() ) ); 
            return true;                     

         default:       

            return MonoSynth::evHandler(e);  // pass other events through
      }
   }

   void output( char *buf )                  // output 1 buffer of audio
   {
      osc.output( buf );                     // pass buffer to oscillator
   }

} myMultiOctaveSynth;

//  ---------------------------------------------------------------------------
//
//                       A Note on the PLAYTHRU flag
//                  
//  Setting the PLAYTHRU flag has wider implications than just translating
//  button-tap events into octave-shifting meta events, and will be discussed
//  in a later example.
//
//  ---------------------------------------------------------------------------      


void setup()
{
   ardutouch_setup( &myMultiOctaveSynth );   // initialize ArduTouch resources
}

void loop()
{
   ardutouch_loop();                         // perform ongoing ArduTouch tasks  
}                                             

//  ---------------------------------------------------------------------------
//
//  If you compile and upload this sketch to the ArduTouch you should hear a 
//  synthetic-choir like tone at 440 Hz. Pressing the keys will play tones in an
//  equal temperament tuning. Tapping the right button will raise the keyboard
//  by 1 octave. Tapping the left button will lower the keyboard by 1 octave.
//
//   --------------------------------------------------------------------------



