//
//  _60_LFO.ino
//
//  This sketch demonstrates the basic features of the LFO class. It shows how
//  to turn an LFO on and off, how to set its frequency, its depth (intensity), 
//  and its waveform shape.
//
//  In this synth an LFO will be used to control the volume of a two voice
//  synth. The synth will be configured to do the following:
//
//    Pressing the left button will turn the LFO on ("unmute" it). 
//    Pressing the right button will turn the LFO off ("mute" it). 
//
//    Turning the top pot will change the LFO frequency.
//    Turning the bottom pot will change the LFO depth.
//
//    Pressing 'C' on the keyboard will set the LFO waveform to a triangle wave.
//    Pressing 'D' on the keyboard will set the LFO waveform to a square wave.
//    Pressing 'E' on the keyboard will set the LFO waveform to a rising sawtooth.
//    Pressing 'F' on the keyboard will set the LFO waveform to a falling sawtooth.
//
//    Pressing 'A' on the keyboard will unmute the synth.
//    Pressing 'B' on the keyboard will mute the synth.
//
//  Target:   ArduTouch board
// 
//  ---------------------------------------------------------------------------
// 
//  Copyright (C) 2019, Cornfield Electronics, Inc.
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

#include "ArduTouch.h"                    // use the ArduTouch library 

about_program( LFO, 1.00 )                // specify sketch name & version

class LFOSynth : public TwoVoxSynth
{
   typedef TwoVoxSynth super;             // superclass is TwoVoxSynth

   public:

   LFO volLFO;                            // LFO for modulating synth volume

   // An LFO has an output value that potentially oscillates between 0.0 and 1.0
   //
   // The range of output values depends on the LFO depth setting:
   //  
   //   if the depth is set to 100% the oscillation range is 0.0 to 1.0
   //   if the depth is set to  50% the oscillation range is 0.5 to 1.0
   //   if the depth is set to  20% the oscillation range is 0.8 to 1.0
   //   ... etc
   //
   // The LFO method setDepth( byte depth ) is used to set the LFO depth.
   // The depth argument is a number between 0 and 128, where 128 is 100%.
   //
   // The LFO can have one of four waveforms:
   //
   //   triangle (default), square, rising sawtooth, falling sawtooth.

   void welcome()
   {
      // initialize the LFO:

      volLFO.reset();                     // always reset an onject before using it
      volLFO.setDepth( 64 );              // set LFO depth to 50% (64/128)
      volLFO.setFreq( 1.0 );              // set LFO frequency to 1 Hz

      // start the voices playing a "fifth" apart

      vox[0]->setFreq( 300.0 );
      vox[1]->setFreq( 450.0 );
   }

   boolean evHandler( obEvent e )         // event handler
   {
      switch ( e.type() )
      {
         case BUT0_PRESS:                 // unmute the LFO

            volLFO.setMute( false );      
            break;

         case BUT1_PRESS:                 // mute the LFO

            volLFO.setMute( true );       
            break;

         // We route POT events to the LFO object, whose event handler is
         // pre-programmed to use POT0 to control the LFO frequency, and POT1
         // to control the LFO depth:

         case POT0:                       // change LFO frequency
         case POT1:                       // change LFO depth

            volLFO.evHandler( e );
            break;
         
         // Key events are intercepted. Instead of playing notes they will 
         // execute pieces of code:

         case KEY_DOWN:                      // a key has been pressed
         {
            switch( e.getKey().position() )
            {
               case 0:                       // 'C'

                  volLFO.setWaveform( LFO_TRI_WF );   // use triangle waveform for LFO
                  break;

               case 2:                       // 'D'

                  volLFO.setWaveform( LFO_SQ_WF );    // use square waveform for LFO
                  break;

               case 4:                       // 'E'

                  volLFO.setWaveform( LFO_RSAW_WF );  // use rising sawtooth waveform for LFO
                  break;

               case 5:                       // 'F'

                  volLFO.setWaveform( LFO_FSAW_WF );  // use falling sawtooth waveform for LFO
                  break;

               case 9:                       // 'A'

                  setMute( false );          // unmute synth
                  break;

               case 11:                      // 'B'

                  setMute( true );           // mute synth
                  break;
            }
            break;                        // KEY_DOWN event was handled
         }

         case KEY_UP:                     // a key has been released

            break;                        // "eat" key up events
         
         default:       

            return super::evHandler(e);   // pass all other events to superclass
      }
      return true;                        // event was handled
   }

   // We use the dynamics method to update the LFO's output value, and then use
   // that value to adjust the volume of our synth.
   //
   // The synth volume is set via the setVol() method, which takes as an argument
   // a number between 0 and 255.

   void dynamics()                        // perform a dynamic update
   {
      super::dynamics();                  // perform synth dynamics
      volLFO.dynamics();                  // update LFO
      setVol( 255 * volLFO.value );       // use it's updated output value to set volume
   }

} myLFOSynth;


void setup()
{
   ardutouch_setup( &myLFOSynth );           // initialize ArduTouch resources
}

void loop()
{
   ardutouch_loop();                         // perform ongoing ArduTouch tasks  
}                                             




