//
//  _11_Voice_Part_1.ino
// 
//  This sketch introduces the Voice class, a ready-to-use class from
//  the ArduTouch library, which implements features found in a typical 
//  synthesizer voice (such as an oscillator, a volume control, detuning, 
//  vibrato, tremolo, portamento, amplitude and filter envelopes, and an 
//  effects loop). In this example we are only interested in:
//
//    1) how to initialize a Voice by assigning it an oscillator
//    2) how to call the Voice's noteOn() and noteOff() methods 
//    
//  We will use the Voice's built-in vibrato control to reproduce the 
//  VibratoSynth from example _08_. 
//
//  Target:   ArduTouch board
// 
//  ---------------------------------------------------------------------------
// 
//  Copyright (C) 2016, Cornfield Electronics, Inc.
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

about_program( Voice Part 1, 1.00 )          // specify sketch name & version
set_baud_rate( 115200 )                      // specify serial baud-rate

class VoiceVibratoSynth : public Synth
{
   WaveOsc    osc;                           // use a wave oscillator 
   StockVoice voc;                           // and a stock voice to run it in

   public:

   void setup() 
   { 
      osc.setTable( wavetable( Sine ) );     // use Sine wave from library

      // The voice object must be assigned an oscillator before it can be used

      voc.useOsc( &osc );                    // assign oscillator to voice
      voc.reset();                           // reset the voice

      // Voice contains a built-in ADSR amplitude envelope (public Voice::envAmp). 
      // ADSR stands for Attack, Decay, Sustain, Release.  It is a standard way 
      // of shaping the amplitude of waveforms in synthesizers. By default the 
      // release time for the envelope is 0, which means that a note will play 
      // forever after released. Because this sketch is meant to demonstrate
      // the noteOn() and noteOff() methods of Voice we set the envelope release
      // time to the smallest non-zero value (which is 1) -- so that the note stops 
      // playing almost instantly after lifting your finger from the key. 
      // (Envelopes will be fully covered in a later sketch).

      voc.envAmp.setRelease(1);              // insure near-instantaneous note off

      // We next initialize the voice's "global" volume level, so that it can
      // be heard. Note: "global" vs "local" volume levels will be discussed
      // in a later example.

      voc.setGlobVol( 255 );                 // set the voice's "global" volume
   }                                         

   // In our event handler we no longer have to process the KEY_DOWN event through
   // the masterTuning object, but instead call the Voice's noteOn() method, which
   // does this for us.

   boolean evHandler( obEvent e )            // event handler
   {
      switch ( e.type() )                    // branch on event type
      {
         case KEY_DOWN:                      // a key has been pressed
         {
            key myKey = e.getKey(); 
            myKey.setOctave( keybrd.getOctave() );

            voc.noteOn( myKey );             // notify voice of key press
            return true;                     // KEY_DOWN event was handled
         }
         case KEY_UP:                        // key has been released
         {
            key myKey = e.getKey(); 
            myKey.setOctave( keybrd.getOctave() );

            voc.noteOff( myKey );            // notify voice of key release
            return true;                     // KEY_UP event was handled
         }
         case POT0:                          // top pot was moved
         case POT1:                          // bottom pot was moved

            return voc.vibrato.evHandler(e); // pass pot events to vibrato

         case BUT0_PRESS:                    // left button was pressed

            voc.vibrato.setMute( false );    // turn vibrato on
            return true;                     // "event was handled"

         case BUT1_PRESS:                    // right button was pressed

            voc.vibrato.setMute( true );     // turn vibrato off
            return true;                     // "event was handled"

         default:                            // pass unhandled events on (so 
                                             // that they are handled in a
            return Synth::evHandler(e);      // standard way)
      }
   }

   // Our output() and dynamics() methods are routed through the voice 
   // object (not the osc object, as before). Since we assigned the osc 
   // to the voice in setup, the voice object will automatically manage 
   // calls to the oscillator, as well as manage calls to all the voice's 
   // built-in controls (such as the vibrato).
               
   void output( char *buf )  {  voc.output( buf ); }
   void dynamics()           {  voc.dynamics();    }

} myVoiceVibratoSynth;

//  ---------------------------------------------------------------------------
//
//                An Interesting Side-by-Side Sound Comparison
//                  
//  The VoiceVibratoSynth in this sketch reproduces the VibratoSynth from
//  example _08_. Both use a Sine wave wave-table oscillator and a vibrato 
//  control, and play notes only while a key is pressed. But they are not 
//  created equal! 
//
//  Upload the VibratoSynth from example _08_ and play some notes. Listen to 
//  the clicking noise that happens every time a note is played and released. 
//  Now upload this sketch and play some notes. :)
//  
//  One of the many benefits to running your oscillator inside of a Voice
//  object is that the Voice class contains low-level logic for smoothing out 
//  abrupt transitions in oscillator volume which would otherwise cause pops 
//  and clicks in the audio.  Cool?
//
//  ---------------------------------------------------------------------------      

void setup()
{
   ardutouch_setup( &myVoiceVibratoSynth );  // initialize ArduTouch resources
}

void loop()
{
   ardutouch_loop();                         // perform ongoing ArduTouch tasks  
}                                             

//  ---------------------------------------------------------------------------
//
//  Once this sketch is uploaded to the ArduTouch, pressing keys will play 
//  a Sine wave tone in an equal temperament tuning. When a key is released
//  the tone will stop playing. Pressing the left button once will turn on 
//  a vibrato effect. Pressing the right button once will turn off the vibrato 
//  effect. Turning the top pot will change the frequency of the vibrato. 
//  Turning the bottom pot will change the depth of the vibrato. Tapping the 
//  right button will raise the keyboard by 1 octave. Tapping the left button 
//  will lower the keyboard by 1 octave.
//
//  Unlike example _08_, you will not hear clicks or pops when you press and 
//  release notes on the keyboard.
//
//  ---------------------------------------------------------------------------


