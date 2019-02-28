//
//  _09_Vibrato.ino
// 
//  Example 09 of introductory examples
//     to learn basics of programming your own ArduTouch synthesizers
//  To get the most of these exmples, please go through them in sequence. 
//
//  This sketch shows how to use the built-in vibrato control in StockVoice.
//
//  This example synthesizer also introduces button press events.
//
//  NOTE:
//  Since the microcontroller in Arduinos don't have many input/output pins, 
//  the ArduTouch board is limited to two push-buttons.  To make more use of 
//  these two button, the ArduTouch library distinguishes these different ways 
//  of pressing the buttons:
//      Tap         --  short click
//      Double-Tap  --  double-click
//      Press       --  long press
//      Tap-Press   --  double-click, with the 2nd being a long press
//
//
//  There are many, many more way cool things available in the ArduTouch library
//  that we haven't covered in these introductory examples!!  
//
//  Please check out the higher numbered example synths for more advanced cool stuff
//  you can do with the ArduTouch library.
// 
//  Please also see our open source synthesizers (DuoPoly, Thick, Arpology, Xoid, 
//  Mantra, Beatidtude) for how to do such things as adding tremolo, portamento, 
//  many different effects, pitch modifiers, amplitude modifiers, and multiple 
//  oscillators per voice.
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

#include "ArduTouch.h"                    // use the ArduTouch library 

about_program( Vibrato, 1.00 )            // specify sketch name & version

class VibratoSynth : public OneVoxSynth   // output a wavetable from the library and
{                                         // manipulate its vibrato
   public:

   // The StockVoice class, which is used by default by OneVoxSynth, also includes 
   // a built-in Vibrato control named "vibrato". As in the previous example we must
   // typecast the generic vox[0] pointer in order to access the vibrato control
   // (which is not a member of the generic Voice object).

   StockVoice *voice;                     // we will use this to typecast vox[0]

   void welcome()
   {
      voice = (StockVoice *)vox[0];       // we typecast vox[0] to access envAmp & vibrato

      voice->envAmp.setAttack( 60 );      // initialize envelope ADSR ...
      voice->envAmp.setDecay( 40 );                                                 
      voice->envAmp.setSustain( 192 );
      voice->envAmp.setRelease( 8 );
   }

   // our event handler uses:
   //     top    pot          -  controls vibrato speed
   //     bottom pot          -  controls vibrato depth
   //     left   button Press -  turn vibrato on
   //     right  button Press -  turn vibrato off
   //
   //     NOTE:  Tapping the left and right buttons change the octave of the notes
   //            (this is taken care of inside of OneVoxSynth::evHandler)

   boolean evHandler( obEvent e )         // event handler
   {
      switch ( e.type() )                 // branch on event type
      {
         case POT0:                       // top pot was moved
         case POT1:                       // bottomn pot was moved

            voice->vibrato.evHandler( e ); // pass pot events to vibrato   
            break;

         case BUT0_PRESS:                 // left button was pressed

            voice->vibrato.setMute( false );  // turn vibrato on ("unmute" it)
            break;

         case BUT1_PRESS:                 // left button was pressed

            voice->vibrato.setMute( true );   // turn vibrato off ("mute" it)
            break;

         default:                         // pass all other events to normal handler

            return OneVoxSynth::evHandler(e); 
      }
   }

   Osc* newOsc( byte nth )
   {
      WaveOsc *o = new WaveOsc();         // use a wavetable oscillator
      o->setTable( wavetable( Rood ) );   // with "Rood" wavetable from library
      return o;              
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

