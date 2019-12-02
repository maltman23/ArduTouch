//
//  _61_Signed_LFO.ino
//
//  To fully understand this sketch you should first work through the following:
//
//       _60_LFO.ino
//       _21_Pitch_Modifiers.ino
//
//  In the prior sketch, _60_LFO.ino, we saw how the LFO had an output value which 
//  oscillated between 1.0 and potentially 0.0 -- depending on the depth setting
//  of the LFO. If the LFO depth was set to 64 (64/128 = 50%) then the LFO output
//  value would oscillate between 1.0 and 0.5; if the depth was set to 128 (128/
//  128 = 100%) then the LFO output value would oscillate between 1.0 and 0.0. This 
//  type of output range works well for controls such as a tremolo, where you are 
//  multiplying the amplitude of a signal by the LFO output value. 
//
//  One drawback to the output range discussed above (lets call it an "unsigned 
//  range") is that the center value of oscillation changes depending on the depth 
//  setting. At a depth setting of 50% the LFO output value oscillates about the
//  center value of 0.75; at a depth setting of 100% the LFO output value oscillates
//  about the center value of 0.50. 
//
//  There are situations which call for an LFO to oscillate around a fixed center 
//  value. To handle this case the LFO can be configured via the following method:
//
//         LFO::setSigned( boolean )
//
//  If x is pointer to an LFO then x->setSigned( true ) will configure the LFO to
//  have an output range with a fixed center value of 0.0 and an amplitude of 
//  oscillation that varies with the depth setting. At a max depth setting of 100% 
//  the LFO output value will oscillate between -1.0 and 1.0. At a depth setting of 
//  50% the LFO output value will oscillate between -0.5 and 0.5. We call this a
//  "signed range".
//
//  By default an LFO's range is unsigned. 
//  It can be set to signed by setSigned( true ). 
//  It can be set to an unsigned by setSigned( false ).
//
//            ----------------------------------------------------------
//
//  In this synth an LFO will be used as the basis for a vibrato control. 
//  The synth will be configured to do the following:
//
//    Pressing the left button will turn the vibrato on ("unmute" it). 
//    Pressing the right button will turn the vibrato off ("mute" it). 
//
//    Turning the top pot will change the vibrato frequency.
//    Turning the bottom pot will change the vibrato depth.
//
//    Pressing 'C' on the keyboard will set the vibrato waveform to a triangle wave.
//    Pressing 'D' on the keyboard will set the vibrato waveform to a square wave.
//    Pressing 'E' on the keyboard will set the vibrato waveform to a rising sawtooth.
//    Pressing 'F' on the keyboard will set the vibrato waveform to a falling sawtooth.
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

// Our vibrato LFO inherits from the standard LFO class.
// It's range is set to signed in LFOSynth::welcome() below.

class VibratoLFO : public LFO
{
   typedef LFO super;                     // superclass is LFO

   public:

   // evaluate() is a virtual method of the LFO class. It is called from
   // within LFO::dynamics(), and calculates a new output value which
   // will lie between the limits of -1.0 and 1.0 depending on the depth
   //
   // Here we take the output value calculated by LFO::evaluate() and convert
   // it into a number that lies between 1/RATIO_SEMITONE and RATIO_SEMITONE.
   // This means that when the depth is at 100% the osc frequency to which the 
   // vibrato is applied will flucuate by up to one semi-tone up and one 
   // semi-tone down.

   // How an osc's frequency is automatically modulated without having to write 
   // explicit code to do so is explained in _21_Pitch_Modifiers.ino

   void evaluate()
   {
      super::evaluate();

      #define RATIO_SEMITONE   1.059463   // frequency ratio between adjacent pitches
      #define INVERT_SEMITONE   .943874   // 1 / RATIO_SEMITONE
      
      if ( value >= 0.0 )
         value *= (RATIO_SEMITONE-1.0);
      else
         value *= (1.0-INVERT_SEMITONE);
      value += 1.0;
  }

} vibLFO ;


class LFOSynth : public OneVoxSynth
{
   typedef OneVoxSynth super;             // superclass is OneVoxSynth

   public:

   // The config() callback method is where we allocate / configure all the
   // components of our synth

   void config()
   {
      super::config();                    // the super class will set up a default vox[0]
      vox[0]->addPitchMod( &vibLFO );     // register LFO as a pitch modulator for vox[0]
   }

   // The welcome() callback method is called once by the system after it has called
   // config() and then reset() 

   void welcome()
   {
      // initialize the LFO:

      // We don't need to reset out LFO because it was added as a pitch modulator
      // to vox[0] in config() and the system automatically resets all voices,
      // including their modifiers, before executing a callback to welcome().
      //
      // It is instructive to compare this with _60_LFO.ino where we had to 
      // explicitly reset our LFO before using it.

      // vibLFO.reset();                  // done automatically in vox[0]->reset()

      vibLFO.setSigned( true );           // set LFO to oscillate around 0.0
      vibLFO.setDepth( 32 );              // set LFO depth to 25% (32/128)
      vibLFO.setFreq( 1.25 );             // set LFO frequency to 1.25 Hz

      // start the voice playing

      vox[0]->setFreq( 300.0 );
   }

   boolean evHandler( obEvent e )         // event handler
   {
      switch ( e.type() )
      {
         case BUT0_PRESS:                 // unmute the vibrato

            vibLFO.setMute( false );      
            break;

         case BUT1_PRESS:                 // mute the vibrato

            vibLFO.setMute( true );       
            break;

         // We route POT events to the LFO object, whose event handler is
         // pre-programmed to use POT0 to control the LFO frequency, and POT1
         // to control the LFO depth:

         case POT0:                       // change vibrato frequency
         case POT1:                       // change vibrato depth

            vibLFO.evHandler( e );
            break;
         
         // Key events are intercepted. Instead of playing notes they will 
         // execute pieces of code:

         case KEY_DOWN:                      // a key has been pressed
         {
            switch( e.getKey().position() )
            {
               case 0:                       // 'C'

                  vibLFO.setWaveform( LFO_TRI_WF );   // use triangle waveform for vibrato
                  break;

               case 2:                       // 'D'

                  vibLFO.setWaveform( LFO_SQ_WF );    // use square waveform for vibrato
                  break;

               case 4:                       // 'E'

                  vibLFO.setWaveform( LFO_RSAW_WF );  // use rising sawtooth waveform for vibrato
                  break;

               case 5:                       // 'F'

                  vibLFO.setWaveform( LFO_FSAW_WF );  // use falling sawtooth waveform for vibrato
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

} myLFOSynth;


void setup()
{
   ardutouch_setup( &myLFOSynth );           // initialize ArduTouch resources
}

void loop()
{
   ardutouch_loop();                         // perform ongoing ArduTouch tasks  
}                                             




