//
//  _31_White_Noise.ino
// 
//  This sketch illustrates how to use a white noise oscillator in a voice.
//
//  A low pass filter is added to the voice's effects chain.
//
//  A simple synth is constructed in which:
//
//    1) The white noise oscillator can be played at different pitches by
//       playing notes on the keyboard.
//
//    2) The keyboard can be raised or lowered by an octave by tapping the
//       right or left buttons.
//
//    3) The cutoff frequency of the low pass filter can be controlled by
//       turning the top pot.
//
//    4) The attack time of the amplitude envelope can be controlled by
//       turning the bottom pot.
//
//
//  Target:   ArduTouch board
// 
//  ---------------------------------------------------------------------------
// 
//  Copyright (C) 2017, Cornfield Electronics, Inc.
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

about_program( White Noise, 1.00 )           // specify sketch name & version

class WhiteNoiseSynth : public OneVoxSynth
{
   typedef OneVoxSynth super;       // superclass is OneVoxSynth

   LPFilter lowpass;                // use a low pass filter from library

   public:

   void setup()
   {
      super::setup();
      vox[0]->addEffect( &this->lowpass ); 
   }
   
   bool charEv( char code )         // handle a character event
   {
      switch ( code )
      {
         case '!':                  // perform a reset

            super::charEv( code );
            lowpass.setMute( false );        // turn on lowpass filter
            vox[0]->envAmp.setRelease(12);   // set a short release time
            break;

         default:             
                     
            return super::charEv( code );
      }
      return true;
    }

   bool evHandler( obEvent ev )     // handle an onboard event
   {
      switch ( ev.type() )
      {
         case POT0:                 // pot 0 controls cutoff frequency

            // pass POT0 events to the lowpass filter, which by default uses
            // POT0 to control the cutoff frequency

            return lowpass.evHandler(ev);

         case POT1:                 // pot 1 controls envelope attack

            vox[0]->envAmp.setAttack( ev.getPotVal() );
            return true;

         default: 
                      
            return super::evHandler( ev );
      }
      return true;                 
   }

   Osc   *newOsc( byte nth )        // create oscillator for nth voice
   {
      return new WhiteNoise();
   }

} mySynth;

void setup()
{
   ardutouch_setup( &mySynth );                   
}

void loop()
{
   ardutouch_loop();                            
}                                             

                                             
