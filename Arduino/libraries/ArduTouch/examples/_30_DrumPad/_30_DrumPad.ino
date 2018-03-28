//
//  _30_DrumPad.ino
// 
//  This example synth demonstrates how to turn the ArduTouch keyboard into
//  a drum pad by using the ArduTouch Library's built-in drum sounds, and
//  loading them into a special oscillator: SampleOsc.
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

about_program( _30_DrumPad, 1.00 )           // specify sketch name & version
set_baud_rate( 115200 )                      // specify serial baud-rate

/******************************************************************************
 *
 *                           DrumVoice class
 *
 ******************************************************************************/

class DrumVoice : public Voice        
{
   //  Our DrumVoice class derives from Voice, and expects its oscillator to
   //  be a SampleOsc. The SampleOsc is a special oscillator which plays a
   //  waveform once when triggered. This oscillator is created via the
   //  DrumSynth::newOsc() method further down in the sketch.

   public:

   //  We ovverride the Voice's standard noteOn() method with one which loads a
   //  drum sound wave table from the library, depending on which note is played.
   //
   //  To load wave tables we call the SampleOsc::setTable() method.
   //
   //  Notice that we must type-cast the Voice's osc member to a SampleOsc*,
   //  because the generic Osc class has no setTable() method.

   void  noteOn( key note )         
   {
      SampleOsc *o = (SampleOsc *)osc;
      byte     pos = note.position();

      switch ( pos )
      {
         case 0:
         case 1:
         case 2:

            o->setTable( wavetable( Kick02 ) );
            break;

         case 3:
         case 4:

            o->setTable( wavetable( Tom02 ) );
            break;

         case 5:
         case 6:
         case 7:

            o->setTable( wavetable( Snare01 ) );
            break;

         case 8:
         case 9:

            o->setTable( wavetable( Rim01 ) );
            break;

         case 10:
         case 11:

            o->setTable( wavetable( Hat03 ) );
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
   // Our synth creates a SampleOsc oscillator, and a DrumVoice voice.

   public:

   Osc *newOsc( byte nth )          // create oscillator for nth voice
   {
      return new SampleOsc();
   }

   Voice *newVox( byte nth )        // create nth voice 
   {
      return new DrumVoice();
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

                                             
