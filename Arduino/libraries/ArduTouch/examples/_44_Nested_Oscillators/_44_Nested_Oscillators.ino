//
//  _44_Nested_Oscillators.ino
// 
//  Oscillator containers such as DualOsc, MixOsc, XorOsc, and RingMod can be
//  nested -- that is, one or both of a container's oscillators can be an 
//  oscillator container itself.
//
//  In this synth we will combine three oscillators into an oscillator "section" 
//  which is plugged into our voice as though it were a single oscillator. A
//  RingMod container will be our root oscillator, and one of its oscillators 
//  will be a MixOsc container with 2 oscillators, like this:
//
//
//               ==== Diagram of our Voice's Oscillator Section ====
//
//                           -------           -------
//              Osc 1 ---->  |     |           |  R  |
//                           | MIX |   ---->   |  I  |
//              Osc 2 ---->  |     |           |  N  |
//                           -------           |  G  |   --->   [output]
//                                             |  M  |        
//              Osc 3 --------------------->   |  O  |      
//                                             |  D  |
//                                             -------
//
//                           
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

#include "ArduTouch.h"                       // use the ArduTouch library 

about_program( Nested Oscillators, 1.00 )    // specify sketch name & version

class NestedOscSynth : public OneVoxSynth
{
   typedef  OneVoxSynth super;               // superclass is OneVoxSynth

   public:

   RingMod *root;                            // pointer to root container
   MixOsc  *mix;                             // pointer to mix container

   // Our "root" oscillator is a RingMod container.
   // The 1st oscillator in the RingMod container is a MixOsc of two wave tables 
   // from the library: dVox and Rood.
   // The 2nd oscillator in the RingMod container is a Sine wave.

   Osc *newOsc( byte ith )
   {
      WaveOsc *o1 = new WaveOsc();
      WaveOsc *o2 = new WaveOsc();
      o1->setTable( wavetable_named( dVox ) );
      o2->setTable( wavetable_named( Rood ) );

      mix  = new MixOsc( o1, o2 );
      root = new RingMod( mix, new Sine() );
      return root;
   }

   // Our event handler will route POT0 events to the mix container;
   // and POT1, BUT0_TPRESS and BUT1_TPRESS to the root container.

   boolean evHandler( obEvent ev )
   {
      switch ( ev.type() )
      {
         case POT0:                       // send POT0 to mix container
         
            return mix->evHandler( ev );
            
         case POT1:                       // send POT1 to root container
         case BUT0_TPRESS:                // send BUT0 tap-press to root container 
         case BUT1_TPRESS:                // send BUT1 tap-press to root container 

            return root->evHandler( ev );

         default:

            return super::evHandler( ev );
      }
   }

   void welcome()
   {
      // the following macros set up the voice envelope, voice vibrato, 
      // and root container initial mix and frequency ratio

      execute( PSTR( "0er32\\" ) );
      execute( PSTR( "0V<d.09\\t5\\f4.5\\" ) );
      execute( PSTR( "0Om24\\r1.500\\" ) );
   }

} mySynth;

void setup()
{
   ardutouch_setup( &mySynth );              // initialize ArduTouch resources
}

void loop()
{
   ardutouch_loop();                         // perform ongoing ArduTouch tasks  
}                                             

//  ---------------------------------------------------------------------------
//
//  Once this sketch is uploaded to the ArduTouch, pressing keys will cause 
//  two oscillators to play together.
//
//  The upper pot controls the mix between the two wave table oscillators.
//  The lower pot controls the RingMod frequency ratio.
//  Tap-pressing the right button selects the next RingMod frequency spectrum.
//  Tap-pressing the left  button toggles the ring modulation on/off.
//
//  Try toggling the ring modulation on and off, and changing the mix and
//  frequency ratio to see how it changes the timbre.
//
//  ---------------------------------------------------------------------------


