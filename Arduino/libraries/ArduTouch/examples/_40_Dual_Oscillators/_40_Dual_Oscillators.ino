//
//  _40_Dual_Oscillators.ino
// 
//  The ArduTouch library provides a base class named DualOsc.
//
//  DualOsc inherits from Osc. Instead of generating its own waveform 
//  it is simply a container which holds pointers to two other oscillators.
//  When calls are made to a DualOsc, it propagates these calls to its
//  two oscillators, and transparently combines their output.
//
//  In this sketch we will construct a one voice synth that uses a DualOsc
//  as its oscillator. The Dualosc will consist of two sine wave oscillators
//  generating tones that are a diatonic fifth apart.
//
//  This sketch also introduces:
//
//     1. The Sine class: a faster, leaner Sine wave oscillator.
//     2. The welcome() method for a Synth. 
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

about_program( Dual Oscillators, 1.00 )      // specify sketch name & version

class DualOscSynth : public OneVoxSynth
{
   public:

   // The newOsc() method is where we can specify a custom oscillator.
   // Here, we create a dual oscillator composed of 2 sine wave oscillators.

   Osc *newOsc( byte ith )
   {
      return new DualOsc( new Sine(), new Sine() );
   }

   // --------------------- A note about Sine Waves -----------------------
   //
   // Up until now we have used a general purpose wave table oscillator for 
   // our sine waves, like this:
   //
   //    WaveOsc *o = new WaveOsc();
   //    o->setTable( wavetable(Sine) );
   //
   // or, if we wanted to optimize for speed (over space) we did this:
   //
   //    FastWaveOsc *o = new FastWaveOsc();
   //    o->setTable( wavetable(Sine) );
   //
   // In our newOsc() method above we introduce an even faster and leaner 
   // version of a Sine wave oscillator:
   //
   //    Sine *o = new Sine();
   //
   // The Sine class inherits from the TabOsc256 class, which expects a
   // table of exactly 256 samples describing exactly one waveform period.
   //
   // ----------------------------------------------------------------------

   // welcome() is a virtual method of Synth. It is called once by the system
   // in ardutouch_setup(), whose logic runs like this: 
   //
   //    void ardutouch_setup( Synth *s )
   //    {
   //       ... initialize the system ...
   //
   //       s->config();
   //       s->reset();
   //       s->welcome();
   //    }

   // The DualOsc class has a public state variable named freqRatio 
   // which controls the frequency ratio between the two oscillators.
   // Here we set it to 1.5 (a "fifth" in the diatonic scale.)

   void welcome()
   {
      ((DualOsc *)vox[0]->osc)->freqRatio = 1.5;  // we have to typecast the generic osc               
   }                                              // ptr of vox[0] to set freqRatio

   // The reason we use welcome(), and not config(), to set the frequency ratio
   // for our dual oscillator is that this ratio would be reset to 1.0 (the default)
   // when the synth is reset in ardutouch_setup().

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
//  two Sine Waves to play together, separated by a "fifth".
//
//  ---------------------------------------------------------------------------


