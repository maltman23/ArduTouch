//
//  _58_Button_Events.ino
// 
//  This sketch demonstates how to handle button events.
//
//  A synth is constructed which turns a vibrato effect on and off by pressing
//  the left and right buttons.
//  
//  This examples builds on the previous example _07_.
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

about_program( Button Events, 1.00 )         // specify sketch name & version

class VibratoSynth : public Synth            // combined synths from example _07_
{
   public:

   WaveOsc osc;                              // use a wave oscillator 
   Vibrato vibrato;                          // use a vibrato control

   void    setup();                          // ... see bottom of file
   void    dynamics();                       //     for implementation
   boolean evHandler( obEvent e );           //     of these methods
   void    output( char *buf );              // ...    

} ;

class ButtonSynth : public VibratoSynth
{
   public:

   //  There are two buttons on the ArduTouch board: the left button 
   //  (button 0) and the right button (button 1)
   //
   //  The system distingushes four different ways of touching a button
   //  and generates four different events accordingly:
   //
   //      1) a tap   (a short duration tap of a button)
   //      2) a press (longer duration than a tap)
   //      3) a double-tap
   //      4) a tap-followed-by-a-press
   //
   //  Because we have two buttons, there are a total of eight possible button events. 
   //
   //  Four of these button events (commented below) have special system 
   //  connotations, and will be discussed in future examples.
   //
   //  In this event handler pressing the left button will turn the vibrato 
   //  control on, and pressing the right button will turn the vibrato 
   //  control off.

   boolean evHandler( obEvent e )            // event handler
   {
      switch ( e.type() )                    // branch on event type
      {
         case BUT0_PRESS:                    // left button was pressed

            vibrato.setMute( false );        // unmute the vibrato (turn it on)
            return true;                     // "event was handled"

         case BUT1_PRESS:                    // right button was pressed

            vibrato.setMute( true );         // mute the vibrato (turn it off)
            return true;                     // "event was handled"

         case BUT0_TPRESS:                   // left button was tap-pressed
         case BUT1_TPRESS:                   // right button was tap-pressed

            return true;                     // not used in this example

         // the following 2 button events are conventionally used by the library
         // to manipulate the octave setting of the keyboard (to be discussed in 
         // later examples) and should be avoided for the time being

         case BUT0_TAP:                      // left button was tapped
         case BUT1_TAP:                      // right button was tapped

         // the following button event is conventionally used by the library 
         // to "pop the current mode" (to be discussed in a later example) and 
         // should be avoided for the time being

         case BUT0_DTAP:                     // left button was double-tapped

         // the following button event is conventionally used by the library 
         // as a signal to interpret the next key-down event as a menu selection
         // (to be discussed in a later example) and should be avoided for the 
         // time being

         case BUT1_DTAP:                     // right button was double-tapped

         // pass the above 4 events on to our super class 

         default:                            

            return VibratoSynth::evHandler(e); 
      }
   }

} myButtonSynth ;                                   


/*----------------------------------------------------------------------------*/      

void setup()
{
   ardutouch_setup( &myButtonSynth );        // initialize ArduTouch resources
}

void loop()
{
   ardutouch_loop();                         // perform ongoing ArduTouch tasks  
}                                             

/*----------------------------------------------------------------------------*/      

void VibratoSynth::setup() 
{
   osc.reset();
   osc.setTable( wavetable(Sine) );          // use Sine wavetable from library
   osc.setFreq( 440.0 );
   vibrato.reset();                          // initialize vibrato
}

void VibratoSynth::dynamics()                // perform a dynamic update
{
   vibrato.dynamics();                       // update the vibrato control
   osc.modFreq( vibrato.value );             // apply vibrato to ideal freq
}

boolean VibratoSynth::evHandler( obEvent e )  
{
   switch ( e.type() )
   {
      case POT0:                             // top pot was moved
      case POT1:                             // bottom pot was moved
         return vibrato.evHandler(e);        // pass pot events to vibrato
      case KEY_DOWN:                         
      {
         key myKey = e.getKey();            // get key from event
         myKey.setOctave( 4 );              // set the key's octave to 4
         osc.setFreq( masterTuning->pitch( myKey ) ); 
         return true;                     
      }
      default:       
         return false;                       
   }
}

void VibratoSynth::output( char *buf )       // output 1 buffer of audio
{
   osc.output( buf );                        // pass buffer to oscillator
}

//  ---------------------------------------------------------------------------
//
//  If you compile and upload this sketch to the ArduTouch you should hear a 
//  Sine wave tone at 440 Hz. Pressing the keys will play tones in an equal 
//  temperament tuning. Pressing the left button once will turn on a vibrato 
//  effect. Pressing the right button once will turn off the vibrato effect. 
//  Turning the top pot will change the frequency of the vibrato, and turning 
//  the bottom pot will change the depth of the vibrato (but you won't hear 
//  the changes unless the vibrato is turned on).
//
//  ---------------------------------------------------------------------------      



