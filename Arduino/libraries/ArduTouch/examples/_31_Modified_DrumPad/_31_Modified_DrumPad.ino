//
//  _31_Modified_DrumPad.ino
// 
//  Here is the _30_DrumPad.ino synth with some improvements:
//
//    The drum samples can be played backwards.
//    The duration of sample playback can be stretched.
//    
//
//  How To Use:
//
//    Play the keys on the keyboard. Different drums will sound
//    depending on the note played, as shown in the diagram below:
// 
//     K = Kick Drum, T = Tom, S = Snare, R = RimShot, H = High Hat
//
//           -------------------------------------------------
//           |   |   |   |   |   |   |   |   |   |   |   |   |
//           |   |   |   |   |   |   |   |   |   |   |   |   |
//           |   |   |   |   |   |   |   |   |   |   |   |   | 
//           |   | K |   | T |   |   | S |   | R |   | H |   | 
//           |   |   |   |   |   |   |   |   |   |   |   |   | 
//           |   |   |   |   |   |   |   |   |   |   |   |   | 
//           |   |   |   |   |   |   |   |   |   |   |   |   | 
//           |    ___     ___    |    ___     ___     ___    | 
//           |     |       |     |     |       |       |     |
//           |     |       |     |     |       |       |     |
//           |  K  |   K   |  T  |  S  |   S   |   R   |  H  |
//           |     |       |     |     |       |       |     |
//           |     |       |     |     |       |       |     |
//           -------------------------------------------------
//
//      Press the left  button to play the drum sounds backwards. 
//      Press the right button to play the drum sounds forwards. 
//
//      Turn the top pot to stretch the playback duration.
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

about_program( Modified_DrumPad, 1.00 )      // specify sketch name & version

// If the Runtime Model is set for serial host communications and you're not 
// using Arduino build 1.6.6, the drum samples won't fit in ROM

#ifdef USE_SERIAL_PORT
   #ifndef BUILD_166
      #error This sketch requires a Runtime Model of __STNDLONE__ or __BAREBONE__
   #endif
#endif

/******************************************************************************
 *
 *                           DrumVoice class
 *
 ******************************************************************************/

class DrumVoice : public Voice        
{
   public:

   //  We override the Voice's standard noteOn() method with one which loads a
   //  drum wave table from the library, depending on which note is played.

   void  noteOn( key note )         
   {
      ModSampleOsc *o = (ModSampleOsc *)osc;

      byte pos = note.position();
      switch ( pos )
      {
         case 0:
         case 1:
         case 2:

            o->setSample( wavetable( Kick02 ) );
            break;

         case 3:
         case 4:

            o->setSample( wavetable( Tom02 ) );
            break;

         case 5:
         case 6:
         case 7:

            o->setSample( wavetable( Snare01 ) );
            break;

         case 8:
         case 9:

            o->setSample( wavetable( Rim01 ) );
            break;

         case 10:
         case 11:

            o->setSample( wavetable( Hat03 ) );
            break;
      }
      trigger();
   }

   //          ================        NOTES       ===============
   //
   // We have used only a few of the drum sounds available in the library.
   // Here is a full list:
   //
   //             Kick01, Kick02, Kick03
   //             Tom01, Tom02, Tom03, Tom04, Tom05
   //             Snare01, Snare02, Snare03
   //             Rim01, Rim02, Rim03
   //             Hat01, Hat02, Hat03, Hat04, Hat05, Hat06
   //             Wood01 (a wood block)
   //
   // The ArduTouch library provides "lofi" versions of all the above samples. 
   // These versions take up half as much space as the normal samples. For
   // low frequency sounds, such as kick drums or bass toms, there is almost no
   // drop-off in sound quality between the normal and lofi versions.
   //
   // By convention lofi samples are given the same name as the normal samples,
   // with a prefix of "lofi_" added:
   //
   //             lofi_Kick01, lofi_Kick02, lofi_Kick03
   //             lofi_Tom01, lofi_Tom02, lofi_Tom03, lofi_Tom04, lofi_Tom05
   //             lofi_Snare01, lofi_Snare02, lofi_Snare03
   //             lofi_Rim01, lofi_Rim02, lofi_Rim03
   //             lofi_Hat01, lofi_Hat02, lofi_Hat03 
   //             lofi_Hat04, lofi_Hat05, lofi_Hat06
   //             lofi_Wood01
   //
   // If you are running out of program storage space you may want to mix
   // "lofi" versions of the kick drum with normal versions of the other
   // percussion sounds.

} ;

/******************************************************************************
 *
 *                          DrumSynth class
 *
 ******************************************************************************/

class DrumSynth : public OneVoxSynth
{
   typedef OneVoxSynth super;             // superclass is OneVoxSynth

   // Our synth uses a ModSampleOsc as an oscillator.

   // The ModSampleOsc inherits from SampleOsc and adds these new methods:
   //
   //    backward()       - play sample backwards when triggered
   //    forward()        - play sample forwards when triggered 
   //    flip()           - flip current direction of playback 
   //    stretch(n)       - stretch duration of playback (n is a power of 2 
   //                                                       in the range -3:3)

   public:

   ModSampleOsc *drumOsc;              // ptr to drum oscillator

   Osc *newOsc( byte nth )             // create an oscillator for drums
   {
      drumOsc = new ModSampleOsc();
      return drumOsc;
   }

   Voice *newVox( byte nth )           // create a special voice  for drums
   {
      return new DrumVoice();
   }

   bool evHandler( obEvent e )         // event handler
   {
      switch ( e.type() )              // branch on event type
      {
         case BUT0_PRESS:              // play samples in reverse direction

            drumOsc->backward();
            break;

         case BUT1_PRESS:              // play samples in normal direction

            drumOsc->forward();
            break;

         case POT0:                    // stretch the duration of sample
         {
            // the following 3 lines map pot values of 0:256 to the range -3:3

            double dval  = e.getPotVal();
                   dval -= 128.0;
                   dval *= 7.0 / 256.0; 
            
            drumOsc->stretch( -(int )dval );
            break;
         }

         default:                         // pass all other events to normal handler

            return super::evHandler(e); 
      }
      return true;                     // event was handled
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

                                             
