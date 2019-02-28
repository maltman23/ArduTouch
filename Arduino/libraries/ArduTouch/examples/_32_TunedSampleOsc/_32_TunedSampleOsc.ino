//
//  _32_TunedSampleOsc.ino
// 
//  This sketch shows how to use a TunedSampleOsc in a Voice, and assign a
//  sample to it. When the voice is triggered to play a note the sample will 
//  play back at the appropriate musical pitch.
//
//  This synth uses 2 voices with different oscillators:
//
//    Voice 0's osc is set to the Wood01 sample from the ArduTouch library.
//    Voice 1's osc is set to a sine wave (by default).
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

#include "ArduTouch.h"                       // use the ArduTouch library 

about_program( TunedSampleOsc, 1.00 )        // specify sketch name & version

class TunedSampleSynth : public TwoVoxSynth
{
   typedef TwoVoxSynth super;             // superclass is TwoVoxSynth

   public:

   Osc *newOsc ( byte nth )                // create nth oscillator
   {
      switch ( nth )
      {
         // oscillator 0 will play the "Wood01" sample when triggered

         case 0:
         {
            TunedSampleOsc *woodOsc = new TunedSampleOsc();
            woodOsc->setTable( wavetable( Wood01 ) );
            return woodOsc;
         }

         // the super class creates sine wave oscillators by default 

         default:  return super::newOsc( nth );
      }
   }

   void welcome()
   {
      // set the release on the sinewave's amplitide envelope so it doesn't play forever
      ((StockVoice *)vox[1])->envAmp.setRelease(150);

      // start the keyboard in the 5th octave
      keybrd.setOctave(5);
   }
      
} mySynth;

// *  Note:  Wood01 is the only sample in the library that "knows its pitch"
//           when played back at the rate of one sample per tick.
//
//           The other samples are considered to be "at 440 Hz" 
//           when played back at the rate of one sample per tick.
//            
//           To override this frequency use TunedSampleOsc::normalize().
//
//           For example, the following sets myOsc to play the kick drum
//           at normal speed when the osc frequency is 220 Hz:
//
//                TunedSampleOsc *myOsc; 
//                myOsc->setTable( wavetable( Kick01 ) );
//                myOsc->normalize( 220.0 );
//

void setup()
{
   ardutouch_setup( &mySynth );                   
}

void loop()
{
   ardutouch_loop();                            
}                                             

                                             
