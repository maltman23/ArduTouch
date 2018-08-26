//
//  _42_XorOsc.ino
// 
//  The XorOsc class inherits from MixOsc. 
//
//  When unmuted, the XorOsc XORs the mixed output of its two oscillators. 
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

about_program( XorOsc, 1.00 )                // specify sketch name & version

class XorOscSynth : public OneVoxSynth
{
   public:

   Osc *newOsc( byte ith )
   {
      return new XorOsc( new Sine(), new Sine() );
   }

   void welcome()
   {
      XorOsc *x = (XorOsc *)(vox[0]->osc);   // type-cast generic ptr
      x->freqRatio = .53;                    // start with an interesting ratio
      x->setMute( false );                   // and turn XOR on
      console.pushMode( x );                 // route U/I events directly to XorOsc
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
//  Tap-pressing the left  button toggles the XOR function on/off.
//
//  ---------------------------------------------------------------------------


