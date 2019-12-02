//
//  _24_Gain.ino
// 
//  This sketch demonstrates how to use the ArduTouch library's Gain effect.
//
//  It shows how to modify the following settings for the Gain effect:
//
//       gain factor      - factor by which to multiply the audio input 
//       max gain factor  - maximum permitted gain factor
//       auto clipping    - when enabled, the audio output will be clipped 
//                          (hard-limited) to 8 bits; otherwise the output
//                          will be allowed to "overflow" 8 bits (large
//                          positive values wrap to negative values and
//                          vice-versa).
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

#include "ArduTouch.h"                    // use the ArduTouch library 

about_program( Gain, 1.00 )               // specify sketch name & version

class GainVoice : public ADSRVoice        // our custom voice uses a gain effect
{
   public:

   Gain   *gain;                          // ptr to gain effect
   MixOsc *dualOsc;                       // typecast ptr to our oscillator

   // Our custom voice will use a double oscillator, which will be fed through
   // a single gain effect. Both the oscillator and the gain effect will be
   // initialized in our synth's welcome() method below.

   GainVoice()
   {
      gain = new Gain();                  // instantiate a gain effect
      addEffect( gain );                  // and add it to the Effects chain

      dualOsc = new MixOsc( new Sine(), new Sine() ); // instantiate a double Sine wave osc
      useOsc( dualOsc );                  // and use it
   }

} ;

class GainSynth : public OneVoxSynth  
{                                         
   typedef OneVoxSynth super;             // superclass is OneVoxSynth

   public:

   GainVoice *gainVox;                    // same as vox[0] (but typecast to GainVoice)

   // in newVox() we create a typecast ptr to our custom voice so that we
   // can easily reference the gain control from within GainSynth methods.

   Voice *newVox( byte nth )              
   {
      gainVox = new GainVoice();          // typecast equivalent of vox[0]
      return gainVox;                     // use our custom GainVoice
   }

   // We initialize the Gain effect so that its max permitted gain factor is 6.0,
   // its gain factor is 4.0, and it is muted at the start of the sketch. By
   // default auto-clipping is enabled. 

   void welcome()
   {
      gainVox->gain->setMaxGain( 6.0 );   // library default setting is 2.0
      gainVox->gain->setGain( 4.0 );      // library default setting is 1.0
      gainVox->gain->setMute( true );     // library default setting is unmuted
      
      gainVox->dualOsc->freqRatio = 1.5;  // play oscillators a fifth apart

      gainVox->envAmp.setRelease( 80 );   // set ADSR release so notes won't play forever
      keybrd.setDefOct(3);                // start keybrd in 3rd octave (default is 4th)
   }

   // We configure our event handler so that:
   //
   //    Pressing the left button toggles the Gain effect on or off
   //    Pressing the right button togges the auto-clipping feature on or off
   //    Turning the top pot controls the gain factor

   bool evHandler( obEvent e )            // handle onboard event
   {
      switch ( e.type() )
      {     
         case BUT0_PRESS:                 // toggle Gain effect on/off

            gainVox->gain->setMute( ! gainVox->gain->muted() );
            break;

         case BUT1_PRESS:                 // toggle Gain's autoClip feature on/off

            gainVox->gain->autoClip = ! gainVox->gain->autoClip;
            break;

         case POT0:                       // pass POT0 events to Gain effect  

            gainVox->gain->evHandler(e);  // Gain effect uses POT0 to control gain factor
            break;
                                              
         default:       

            return super::evHandler(e);   // pass other events through to superclass
      }
      return true;
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

// Once the sketch is uploaded and you play notes on the keyboard you will hear
// "clean" sine waves whose frequencies are a musical 5th apart. Press the left
// button to unmute the gain (press it again and it is muted), and the right button 
// to turn auto-clipping off (press it again to turn it on). Use the top pot to
// change the gain factor. Notice the change in the audio.
