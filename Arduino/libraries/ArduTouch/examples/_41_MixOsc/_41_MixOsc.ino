//
//  _41_MixOsc.ino
// 
//  The MixOsc class inherits from DualOsc. It adds a mixer function,
//  and supplies a built-in U/I which makes it easier to change the
//  frequency ratio between the two oscillators by using the onboard
//  pots and buttons.
//
//  The MixOsc class serves as a super class for more elaborate classes
//  (to be covered in the sketches that follow), so it is a good idea to
//  understand how to use it.
//
//  This sketch creates a one voice synth using a MixOsc. 
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

about_program( MixOsc, 1.00 )                // specify sketch name & version

class MixOscSynth : public OneVoxSynth
{
   public:

   // Our oscillator is a MixOsc composed of 1 wavetable oscillator using
   // the "dVox" wave table from the library, and 1 sine wave oscillator.
   // Notice that the 2 oscillators in a dual oscillator do not have to
   // be of the same type :)

   Osc *newOsc( byte ith )
   {
      WaveOsc *o = new WaveOsc();
      o->setTable( wavetable(dVox) );
      return new MixOsc( o, new Sine() );
   }

   // In welcome() we push our MixOsc to the top of the mode stack.
   // This means that when our synth starts we will be "inside" the MixOsc, 
   // so that all the U/I events coming from the ArduTouch board will be sent 
   // directly to the MixOsc object (instead of to the synth). 

   void welcome()
   {
      console.pushMode( vox[0]->osc );
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
//  We can now investigate the U/I for the MixOsc class:
//
//  The upper pot controls the mix between the two oscillators.
//  The lower pot controls the freqeuency ratio between the two oscillators.
//  Tap-pressing the right button selects the next frequency spectrum.
//  Tap-pressing the left  button will toggle the MixOsc on/off ("unmute/mute")
//
//  Note 1: Think of a tap-press as a "double-tap" where the 2nd tap is of a
//          longer duration.
//
//  Note 2: The MixOsc has 2 built-in frequency spectrums. When you turn the
//          lower pot it will select a number from the current frequency spectrum. 
//          The 1st built-in frequency spectrum consists of numbers that end in 
//          .00 .25 .33 .5 .66 and .75. The 2nd frequency spectrum consists of
//          numbers that correspond to diatonic intervals. Frequency spectrums
//          are customizable. A later sketch will show you how to override or
//          add new frequency spectrums.
//
//  Note 3: Toggling the MixOsc on/off does nothing to the MixOsc itself. However, 
//          classes which inherit from MixOsc will use this feature (see the 
//          following sketches).
//
//  ---------------------------------------------------------------------------


