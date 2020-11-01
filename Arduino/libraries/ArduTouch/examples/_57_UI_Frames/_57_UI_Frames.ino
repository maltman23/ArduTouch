//
//  _57_UI_Frames.ino
// 
//  This sketch explains how to configure and use User Interface Frames in 
//  your synth. 
//
//  User Interface Frames provide a built-in mechanism for controlling up to 
//  18 different parameters with just the 2 on-board pots!
//
//   -----------------------------------------------------------------------
//              
//               ==== Diagram of a User Interface Frame ====
//
//   A User Interface Frame (or "UI Frame", for short) has two sides: 
//   a left side (side 0) and a right side (side 1). 
//
//   Each side consists of an LED and a corresponding button on the board.
//
//   The red LED and the left button constitute the left side of the UI frame.
//   The blue LED and the right button constitute the right side of the UI frame.
//              
//
//                   Left Side (0)       Right Side (1)
//      
//                -----------------------------------------
//                |                  |                    |
//                |    Red LED (0)   |    Blue LED (1)    |
//                |                  |                    |
//                |----    .     ----|----     .      ----|
//                |                  |                    |
//                |   * blinking *   |    * blinking *    |   state = 2
//                |                  |                    |
//                |   *    on    *   |    *    on    *    |   state = 1
//                |                  |                    |
//                |   *   off   *    |    *   off    *    |   state = 0
//                |                  |                    |
//                |---------------------------------------|
//                |                  |                    |
//                |    Left Button   |    Right Button    |
//                |                  |                    |
//                |----    .     ----|----     .      ----|
//                |                  |                    |
//                |  press to bump   |   press to bump    |
//                |   left state     |    right state     |
//                |                  |                    |
//                -----------------------------------------
//
//
//  Each side of the UI frame can be in one of up to three different states (0, 1, or 2).
//
//  When a UI frame's side is in state 0, it's LED will be off.
//  When a UI frame's side is in state 1, it's LED will be on.
//  When a UI frame's side is in state 2, it's LED will be blinking.
//
//  By pressing the left or right buttons you can change the state of the respective 
//  side by +1, and the LED for that side will change in accordance with the new
//  state, as shown in the above diagram.
//  
//  If a UI frame's side is already in it's maximum state, then pressing it's button 
//  will reset that side's state to 0.
//
//         ------------------------------------------------------------
//         
//                            A Note on Terminology
//
//          The term "frame" when used alone (i.e. when not preceded 
//          by "User Interface", or "UI") refers to a specific state 
//          of the UI Frame (i.e., the left side is in state x, and
//          in right side is in state y).
//
//         ------------------------------------------------------------
//
//  The ArduTouch library defines a number of symbolic constants for referring
//  to frames. These are of the form "FRAMExy", where x = the state of the
//  left side, and y = the state of the right side. For example:
//
//       FRAME00 refers to the frame where both sides are in state 0
//               (both LEDS are off). 
//  
//       FRAME12 refers to the frame in which the left side is in state 1 and 
//               the right side is in state 2 (red LED is on and blue LED is
//               blinking).
//
//  If you want to know what frame you are in (perhaps you wish to make 
//  certain button gestures execute conditional code depending on what 
//  frame you are in) you can get the current frame number by calling:
//
//       byte currentFrame = frame.Num();
//
//  For example:
//
//       switch ( frame.Num() )
//       {
//          case FRAME00:  { do task A }; break;
//          case FRAME01:  { do task B }; break;
//          default:       { } 
//       }
//
//  When a frame is referred to as part of a compound event symbol (see next
//  section), FRAME is abbreviated to F (so FRAME12 becomes F12).
// 
//   -----------------------------------------------------------------------
//              
//                          ==== POT_FRAME Events ====
//
//  In sketch _55_Pot_Events.ino it was shown how changes to the top and bottom
//  pots are sent to your evHandler() method as POT0 and POT1 events by the 
//  system. If you're not familiar with how to process pot events this would be 
//  a good time to read through the _55_Pot_Events.ino sketch ...
//
//  Virtually every object in your synth, including the synth itself, inherits
//  from the Control base class. The Control class contains a UI Frame, which
//  by default is disabled. In this example sketch we will only use the synth's 
//  UI Frame, which we will call "the UI Frame", but remember that all the
//  controls in the synth potentially have their own UI Frames.
//
//  When the UI Frame is disabled, changes to pot positions are sent by the 
//  system to your evHandler() method as simple POT0 and POT1 events.
//
//  When the UI Frame is enabled, changes to pot positions are sent by the
//  the system to your evHandler() method as compound POT_FRAME events. 
//  For example, if the current frame is 21 (red LED blinking, blue LED on),
//  and pot 0 is changed, then the system will send a POT0_F21 event. If the 
//  current frame is 00 (both LEDs off) and POT1 is moved then the system will 
//  send a POT1_F00 event.
//
//  Notice that FRAME is abbreviated to F when it is part of a compound event
//  symbol.
//
//   -----------------------------------------------------------------------
//              
//                   ==== Setting UI Frame Dimensions ====
//
//  In the above diagram of a UI Frame, it shows that each side can be in one 
//  of three states (0, 1, 2), meaning that the pots can control a total of 18
//  parameters (2 pots x 3 states x 3 states). But we may not need to control
//  so many paraemeters. For this reason the max # of states per side is
//  configuable via this method:
//
//       setFrameDimensions( byte max_side_0_state, byte max_side_1_state )
//
//  To control 18 parameters, as in the above diagram we call:
//
//       setFrameDimensions( 2, 2 )
//
//  In this synth we are only going to control 12 parameters, so we call:
//
//       setFrameDimensions( 1, 2 )
//
//  This means the left side can take on states 0 or 1, and the right side
//  can take on states 0, 1, or 2.
//
//
//   -----------------------------------------------------------------------
//              
//                     ==== Enabling the UI Frame ====
//
//  UI Frames are disabled by default. To enable the UI Frame you must call:
//
//       enableFrame()
//
//  The call to this method should be put in your reset code, which is the
//  code that handles the '!' character in the charEv( char code ) method.
//
//
//   -----------------------------------------------------------------------
//              
//          ==== Passing POT_FRAME Events to Secondary Controls ====
//
//  In many instances you may want to pass a POT event to a secondary control 
//  which does not use frames. To do this use the potEv( obEvent ev ) method. 
//  For example:
//
//  switch ( ev.type() )
//  {
//     { various cases ... }
//
//     case POT0_F11:                  // pass POT0/1 events to tremolo
//     case POT1_F11:
//
//       return tremolo->potEv( ev );
//
//     default:
//
//       .....
//
//  }
//
//  The potEv() method will automatically strip the frame # from the event, if
//  the object does not have frames enabled, and then call the object's
//  evHandler() method. 
//
//  For examples of calling setFrameDimensions(), enableFrame(), and potEv() 
//  see the code below ...
//
//
//   -----------------------------------------------------------------------
//              
//                        ==== LED Visibility ====
//
//  Once the UI Frame for a synth has been enabled the visible state of the
//  LEDs (off/on/blinking) will relect whatever frame you are in.
//
//  However, for all other controls when their UI Frames are enabled the
//  LEDs do not by default reflect what frame they are in when they control
//  the focus of the interface. This is because there is no way of knowing 
//  whether some higher-level object within which the control is nested is 
//  using the LEDs for some other purpose. For example, in the DuoPoly synth
//  the LEDs are used to indicate which of the 2 voices are active.
//
//  The visibility of each LED in a control's UI Frame is independently 
//  settable via the frameLED0 and frameLED1 character events. 
//
//    bool MyControl::charEv( char code )
//    {
//       switch ( code )
//       {
//          case frameLED0: return true;  // make UI Frame LED0 visible
//          case frameLED1: return false; // make UI Frame LED1 invisible
//       }
//    }
//  
//  So if you want to make a UI Frame's LEDs visible for any object other
//  than a synth, you will have to explicitly return true for the frameLED0
//  and frameLED1 character events for that control.
//
//  Target:   ArduTouch board
// 
//  ---------------------------------------------------------------------------
// 
//  Copyright (C) 2020, Cornfield Electronics, Inc.
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

about_program( UI Frames, 1.00 )             // specify sketch name & version

// The __STNDLONE__ runtime model is necessary for the LEDs to be functional

#ifndef __STNDLONE__
   //#error This sketch requires the __STNDLONE__ runtime model (Model.h)
#endif

//  We will use a standard TwoVoxSynth for this sketch. The TwoVoxSynth 
//  instantiates 2 voices of the StockVoice class. The StockVoice class 
//  adds a built-in vibrato and amplitude envelope to the basic Voice class. 
//
//  This sketch will configure the UI Frame to have a dimension of (1,2).
//  This allows the pots to control a total of 12 parameters.
//
//  The following chart shows the LED states and parameter mappings for each 
//  pot per frame:
//
//   frame    red LED    blue LED         top POT                  bot POT
//  -------   -------    --------   -------------------      -------------------
//  FRAME00     OFF         OFF     voice 0 env attack       voice 1 env attack
//  FRAME01     OFF          ON     voice 0 env decay        voice 1 env decay
//  FRAME02     OFF       BLINK     voice 0 env sustain      voice 1 env sustain         
//  FRAME10      ON         OFF     voice 0 env release      voice 1 env release   
//  FRAME11      ON          ON     voice 0 vibrato freq     voice 0 vibrato freq
//  FRAME12      ON       BLINK     voice 1 vibrato depth    voice 1 vibrato depth
//
//  To operate this synth press the left and right buttons to switch between the
//  various frames, play notes and turn the pots to change the associated parameters
//  for each frame.

class UIFrameSynth : public TwoVoxSynth      
{
   typedef TwoVoxSynth super;                // superclass is TwoVoxSynth

   public:

   void config()
   {
      super::config();                       // execute normal config
      setFrameDimensions( 1, 2 );            // set dimensions for synth's UI Frame
   }

   bool charEv( char code )                  // handle a character event
   {
      switch ( code )
      {
         case '!':                           // perform an object reset

            super::charEv( code );           // execute normal reset code

            enableFrame();                   // enable UI Frame for synth

            vox[1]->xpose = -5;              // transpose voice 1 down a fifth 

            ((StockVoice *)vox[0])->vibrato.setMute( false ); // enable voice 0 vibrato
            ((StockVoice *)vox[1])->vibrato.setMute( false ); // enable voice 1 vibrato

            ((StockVoice *)vox[0])->envAmp.setSustain( 128 ); // init voice 0 env sustain
            ((StockVoice *)vox[1])->envAmp.setSustain( 128 ); // init voice 1 env sustain

            ((StockVoice *)vox[0])->envAmp.setRelease( 60 );  // init voice 0 env release
            ((StockVoice *)vox[1])->envAmp.setRelease( 60 );  // init voice 1 env release

            return true;

         default:

            return super::charEv( code );
      }
   }

   bool evHandler( obEvent ev )              // handle an onboard event
   {
      // Process all non-pot events in the usual way 

      if ( ! ev.amPot() )
         return super::evHandler(ev); 

      // Process pot event (which is compound because the UI Frame has been enabled)

      byte   potVal   = ev.getPotVal();      // cache pot value

      switch ( ev.type() )                   // execute according to pot#_frame#
      {
         case POT0_F00:                      // set envelope attack for voice 0

            ((StockVoice *)vox[0])->envAmp.setAttack( potVal );
            break;

         case POT1_F00:                      // set envelope attack for voice 1 

            ((StockVoice *)vox[1])->envAmp.setAttack( potVal );
            break;

         case POT0_F01:                      // set envelope decay for voice 0 

            ((StockVoice *)vox[0])->envAmp.setDecay( potVal );
            break;

         case POT1_F01:                      // set envelope decay for voice 1 

            ((StockVoice *)vox[1])->envAmp.setDecay( potVal );
            break;

         case POT0_F02:                      // set envelope sustain for voice 0 

            ((StockVoice *)vox[0])->envAmp.setSustain( potVal );
            break;

         case POT1_F02:                      // set envelope sustain for voice 1  

            ((StockVoice *)vox[1])->envAmp.setSustain( potVal );
            break;

         case POT0_F10:                      // set envelope release for voice 0  

            ((StockVoice *)vox[0])->envAmp.setRelease( potVal );
            break;

         case POT1_F10:                      // set envelope release for voice 1  

            ((StockVoice *)vox[1])->envAmp.setRelease( potVal );
            break;

         // The vibrato control uses POT0 and POT1 to control its frequency 
         // and depth respectively. This means we can send the event to its 
         // potEv() handler (which will automatically strip the frame #
         // from the event type before handling it as a simple pot event)

         case POT0_F11:                      // set vibrato freq for voice 0
         case POT1_F11:                      // set vibrato depth for voice 0

            ((StockVoice *)vox[0])->vibrato.potEv( ev );
            break;

          case POT0_F12:                     // set vibrato freq for voice 1
          case POT1_F12:                     // set vibrato depth for voice 1

            ((StockVoice *)vox[1])->vibrato.potEv( ev );
            break;
      }
      return true;
   }

} mySynth ;

void setup()
{
   ardutouch_setup( &mySynth );              // initialize ArduTouch resources
}

void loop()
{
   ardutouch_loop();                         // perform ongoing ArduTouch tasks  
}                                             

