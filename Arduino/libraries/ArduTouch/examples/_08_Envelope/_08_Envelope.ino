//
//  _08_Envelope.ino
// 
//  Example 08 of introductory examples
//     to learn basics of programming your own ArduTouch synthesizers
//  To get the most of these exmples, please go through them in sequence. 
//
// 
//  In the previous two example synthesizers (06 and 07) we used OneVoxSynth.
//  OneVoxSynth has many more capabilities.  In this example we will show how 
//  to use the built-in amplitude envelope for the voice in a OneVoxSynth.
//  (More advanced example synthesizers show how to make use of other functionality.)
//
//  With the dynamics of an amplitude envelope the synth sounds way nicer!
//  FYI:  Amplitude envelopes are a standard feature of most synthesizers.
//        It is usually called "ADSR", for Attack, Decay, Sustain, Release:
//          Attack -- the length of time after hitting a note to get to max volume
//          Decay -- the amount of time after reaching max to get to the Sustain volume
//          Sustain -- the volume of the voice for as long as you keep pressing the key
//          Release -- the amount of time after letting go of the key to go to zero volume
//
//  This example synth allows the two pots to change the attack and decay of the 
//  notes as you touch the keys on the board's keyboard.
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

about_program( Envelope, 1.00 )           // specify sketch name & version

class EnvSynth : public OneVoxSynth       // output a wavetable from the library
{                                         // and manipulate its amplitude envelope
   public:

   // We use welcome() to initialize the ADSR values of the amplitude envelope

   // The state variable vox[0] (inherited from OneVoxSynth) is typed as a pointer
   // to a (generic) Voice object. The Voice object contains our oscillator. 
   // However, by default OneVoxSynth does not use a generic Voice object, but 
   // a StockVoice object (which inherits from Voice). StockVoice adds some
   // built-in standard controls to the Voice object, including an ADSR envelope 
   // named "envAmp" which controls the amplitude of the oscillator.  

   StockVoice *voice;                     // we will use this to typecast vox[0]

   void welcome()
   {
      voice = (StockVoice *)vox[0];       // we must typecast vox[0] to access envAmp

      voice->envAmp.setAttack( 60 );      // initialize envelope ADSR ...
      voice->envAmp.setDecay( 40 );                                                 
      voice->envAmp.setSustain( 128 );
      voice->envAmp.setRelease( 8 );
   }

   // our event handler uses the top pot to change the Attack of the envelope,
   // and the bottom pot to change the Decay of the envelope.

   boolean evHandler( obEvent e )         // event handler
   {
      switch ( e.type() )                 // branch on event type
      {
         case POT0:                       // top pot was moved

            voice->envAmp.setAttack( e.getPotVal() );   // set envelope attack
            break;

         case POT1:                       // bottomn pot was moved

            voice->envAmp.setDecay( e.getPotVal() );   // set envelope decay
            break;

         default:                         // pass all other events to normal handler

            return OneVoxSynth::evHandler(e); 
      }
   }

   Osc* newOsc( byte nth )
   {
      WaveOsc *o = new WaveOsc();         // use a wavetable oscillator
      o->setTable( wavetable( Rood ) );   // with "Rood" wavetable from library
      return o;              
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

