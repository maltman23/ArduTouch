//
//  _43_Ring_Modulation.ino
// 
//  The RingMod class inherits from MixOsc. 
//
//  When unmuted, RingMod multiplies the output of its two oscillators. 
//  When muted, it behaves as a simple mixer.
//
//  This sketch creates a one voice synth using an XorOsc. 
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

about_program( RindMod, 1.00 )               // specify sketch name & version

class RingModSynth : public OneVoxSynth
{
   public:

   Osc *newOsc( byte ith )
   {
      WaveOsc *o = new WaveOsc();
      o->setTable( wavetable( Rood ) );
      return new RingMod( o, new Sine() );
   }

   void welcome()
   {
      RingMod *r = (RingMod *)(vox[0]->osc); // type-cast generic ptr
      r->freqRatio = 2.13;                   // start with an interesting ratio
      r->setMute( false );                   // and turn ring modulation on
      console.pushMode( r );                 // route U/I events directly to RingMod
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
//  The upper pot controls the mix between the two oscillators.
//  The lower pot controls the freqeuency ratio between the two oscillators.
//  Tap-pressing the right button selects the next frequency spectrum.
//  Tap-pressing the left  button toggles the ring modulation on/off.
//
//  Try toggling the ring modulation on and off and notice the change in timbre.
//
//  Note: the mix setting has no effect on the output when ring modulation is on.
//
//  ---------------------------------------------------------------------------


