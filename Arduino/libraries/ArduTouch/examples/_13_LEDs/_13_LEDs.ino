//
//  _13_LEDs.ino
//
//  This sketch shows how to program the onboard LEDs.
//
//  In this synth (which will make no sound):
//  The onboard keyboard is used to control the two onboard LEDs (on, off, or blinking).
//  The bottom pot is used to change the LED blink time.
//
//  This sketch *requires* a Runtime Model of __STNDLONE__ or __BAREBONE__.
//  (please consult example _12_ on how to set the Runtime Model).
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

about_program( LEDs, 1.00 )                  // specify sketch name & version

//  If the Runtime Model is set to __STNDLONE__ or __BAREBONE__ then the symbolic 
//  constant USE_SERIAL_PORT will not be defined, so by checking for it we can
//  verify whether we are using the right Runtime Model. If we are not using
//  the right Runtime Model, the following statement will cause the compiler
//  to abort, and to print an error message in the Arduino output window.

#ifdef USE_SERIAL_PORT
   #error This sketch requires a Runtime Model of __STNDLONE__ or __BAREBONE__
#endif

//  Our synth is meant to demonstrate LED functionality. It does not create
//  any sound because we haven't implemented an output() method.

class LEDSynth : public Synth
{
   public:

   // Here is a list of the library's LED routines:
   //
   //    void onLED( byte nth );             // turn nth (0-based) LED on
   //    void offLED( byte nth );            // turn nth LED off
   //    void blinkLED( byte nth );          // blink nth LED 
   //    void setBlinkTime( byte x );        // set blink time for LEDs
   //    byte getBlinkTime();                // returns blink time for LEDs
   //
   //    -----------------------------------------------------------------
   //
   // The blinkLED() routine takes an optional 2nd argument -- invert:
   //
   //    void blinkLED( byte nth, bool invert = false );  // blink LED, invert phase
   //
   // An LED blinking with an inverted phase will be ON when a normally blinking LED
   // is off, and vice-versa. This allows you to set up the two onboard LEDs to blink
   // alternatively instead of in unison. See evHandler() case for the 'B' note below 
   // for an example. 
   //
   //    ------------------------------------------------------------------
   //
   // Symbolic constants are pre-defined which allow you to refer to the LEDs
   // by position ( LEFT_LED, RIGHT_LED ) or color ( RED_LED, BLUE_LED )
   // In the code, below, we'll use all of these, just to show that they are 
   // the same.

   boolean evHandler( obEvent e )            // event handler
   {
      switch ( e.type() )
      {
         case POT1:                          // bottom pot was moved
         {
            setBlinkTime( e.getPotVal() );   // set LED blink time 
            return true;
         }
         case KEY_DOWN:                      // a key has been pressed
         {
            switch( e.getKey().position() )
            {
               case 0:                       // low 'C'
                  onLED( 0 );                // ... turn on LED 0
                  break;
               case 2:                       // 'D'
                  blinkLED( LEFT_LED );      // ... blink LED 0
                  break;
               case 4:                       // 'E'
                  offLED( RED_LED );         // ... turn off LED 0 -- "0", "LEFT_LED", and "RED_LED" are all the same!
                  break;
               case 5:                       // 'F'
                  onLED( 1 );                // ... turn on LED 1
                  break;
               case 7:                       // 'G'
                  blinkLED( RIGHT_LED );     // ... blink LED 1
                  break;
               case 9:                       // 'A'
                  offLED( BLUE_LED );        // ... turn off LED 1 -- "1", "RIGHT_LED", and "BLUE_LED" are all the same!
                  break;
               case 11:                      // 'B'
                  blinkLED( 1, true );       // ... blink LED 1 (with inverted phase)
                  break;
            }
            return true;                     // KEY_DOWN event was handled
         }
         default:
            return false;                    // pass over all other events
      }
   }

} myLEDSynth;

void setup()
{
   ardutouch_setup( &myLEDSynth );           // initialize ArduTouch resources
}

void loop()
{
   ardutouch_loop();                         // perform ongoing ArduTouch tasks
}

//  ---------------------------------------------------------------------------
//
//   Once you compile and upload this sketch to the ArduTouch:
//
//     (1) Pressing the low 'C' note on the keyboard will turn the red LED on.
//     (2) Pressing the 'D' note on the keyboard will blink the red LED.
//     (3) Pressing the 'E' note on the keyboard will turn the red LED off.
//     (4) Pressing the 'F' note on the keyboard will turn the blue LED on.
//     (5) Pressing the 'G' note on the keyboard will blink the blue LED.
//     (6) Pressing the 'A' note on the keyboard will turn the blue LED off.
//     (7) Pressing the 'B' note will blink the blue LED with an inverted phase.
//         Try this together with (2) to set up alternating blinking LEDs.
//     (8) Turning the bottom pot will change the blink time.
//
//  ---------------------------------------------------------------------------

