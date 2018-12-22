//
//  Beatitude.ino
//
//  A Drum and Bass synth with real-time drum sequencer for the ArduTouch.
//
//  How To Use:
//
//              ------------       Playing Drums         ------------
//
//    Play the keys on the keyboard. Different drums will sound
//    depending on the note played, as shown in the diagram below:
//
//     K = Kick Drum, T = Tom, S = Snare, R = RimShot, H = High Hat
//
//             -------------------------------------------------
//             |   |   |   |   |   |   |   |   |   |   |   |   |
//             |   |   |   |   |   |   |   |   |   |   |   |   |
//             |   |   |   |   |   |   |   |   |   |   |   |   |
//             |   | K |   | T |   |   | S |   | R |   | H |   |
//             |   |   |   |   |   |   |   |   |   |   |   |   |
//             |   |   |   |   |   |   |   |   |   |   |   |   |
//             |   |   |   |   |   |   |   |   |   |   |   |   |
//             |    ___     ___    |    ___     ___     ___    |
//             |     |       |     |     |       |       |     |
//             |     |       |     |     |       |       |     |
//             |  K  |   K   |  T  |  S  |   S   |   R   |  H  |
//             |     |       |     |     |       |       |     |
//             |     |       |     |     |       |       |     |
//             -------------------------------------------------
//
//
//         -----------       Recording a Drum Sequence     ------------
//
//
//    To record a drum sequence press the right button.
//
//    The blue LED will turn on, and a "click track" (metronome) will start.
//
//    When the blue LED is on, but not blinking, it means that the ArduTouch is
//    playing a "cue measure". This is a single measure which plays before
//    the actual recording starts, and lets you get ready for your performance.
//    Any notes you attempt to play during the cue measure will not sound, and
//    will not be recorded.
//
//    By default there are 4 beats in a measure.
//
//    When the cue measure ends the blue LED will start flashing. This means
//    that the ArduTouch is now recording anything you play. By default the
//    sequence is 4 measures long, after which the blue LED will turn off.
//
//
//         -----------      Playing back a Drum Sequence     ------------
//
//
//    To play back your drum sequence press the left button.
//
//    The red LED will start blinking, and your sequence will begin playing.
//
//    When the sequence reaches its end it will loop back to the beginning
//    and continue to play (forever) until you press the left button again.
//
//
//              ------------       Playing Bass         ------------
//
//    When a drum sequence is playing back, the ArduTouch keyboard will
//    turn into a bass instrument, and you can play bass along with the
//    recorded drums. The octave setting for the bass can be bumped up
//    and down by tapping the right and left buttons respectively.
//
//    To revert to playing drums, stop the sequence (by pressing the
//    left button).
//
//
//         -----------         Controlling the Tempo          -----------
//
//    Rotate the bottom pot to slow down or speed up the tempo.
//
//
//        -------   Setting Beats per Measure & # of Measures    --------
//
//    To change the # of beats per measure, and the # of measures in a
//    sequence, tap and then press the left button (think of this as a
//    "double-tap" but with the second tap being of a longer duration.)
//
//    The red LED will turn on, indicating that the ArduTouch keyboard is
//    now in "Menu" mode.
//
//    Select the # beats per measure by pressing a key:
//
//             -------------------------------------------------
//             |   |   |   |   |   |   |   |   |   |   |   |   |
//             |   |   |   |   |   |   |   |   |   |   |   |   |
//             |   |   |   |   |   |   |   |   |   |   |   |   |
//             |   | 2 |   | 4 |   |   | 7 |   | 9 |   | 1 |   |
//             |   |   |   |   |   |   |   |   |   |   | 1 |   |
//             |   |   |   |   |   |   |   |   |   |   |   |   |
//             |   |   |   |   |   |   |   |   |   |   |   |   |
//             |    ___     ___    |    ___     ___     ___    |
//             |     |       |     |     |       |       |     |
//             |     |       |     |     |       |       |     |
//             |  1  |   3   |  5  |  6  |   8   |   10  |  12 |
//             |     |       |     |     |       |       |     |
//             |     |       |     |     |       |       |     |
//             -------------------------------------------------
//
//
//    Now select the # of measures by pressing a second key:
//
//             -------------------------------------------------
//             |   |   |   |   |   |   |   |   |   |   |   |   |
//             |   |   |   |   |   |   |   |   |   |   |   |   |
//             |   |   |   |   |   |   |   |   |   |   |   |   |
//             |   | 2 |   | 4 |   |   | 7 |   | 9 |   | 1 |   |
//             |   |   |   |   |   |   |   |   |   |   | 2 |   |
//             |   |   |   |   |   |   |   |   |   |   |   |   |
//             |   |   |   |   |   |   |   |   |   |   |   |   |
//             |    ___     ___    |    ___     ___     ___    |
//             |     |       |     |     |       |       |     |
//             |     |       |     |     |       |       |     |
//             |  1  |   3   |  5  |  6  |   8   |   10  |  16 |
//             |     |       |     |     |       |       |     |
//             |     |       |     |     |       |       |     |
//             -------------------------------------------------
//
//    The red LED will now turn off.
//
//
//   ************************** IMPORTANT NOTE ***************************
//
//           For the best sounding snare, use Arduino build 1.6.6
//           and uncomment the line "//#define BUILD_166" in the
//           file Model.h of the ArduTouch library.
//
//   *********************************************************************
//
//  ---------------------------------------------------------------------------
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

#ifndef __STNDLONE__
   #error This sketch requires the __STNDLONE__ runtime model (Model.h)
#endif

#ifndef KEYBRD_MENUS
   #error This sketch requires KEYBRD_MENUS to be defined (Model.h)
#endif

#ifndef IMPLICIT_SEQUENCER
   #error This sketch requires IMPLICIT_SEQUENCER to be defined (Model.h)
#endif

about_program( Beatitude, 1.00 )             // specify sketch name & version

/*----------------------------------------------------------------------------*
 *                                 presets
 *----------------------------------------------------------------------------*/

define_preset( Beat0, "L0\\[" )

const byte beat0[] PROGMEM =
{
   31,
   0, QUART_, 7, QUART__, 0, EIGHT_, 7, QUART_,
   0, QUART_, 7, QUART__, 0, EIGHT_, 7, QUART_,
   0, QUART_, 7, QUART__, 0, EIGHT_, 7, QUART__,
   0, QUART_, 0, QUART_,  0, QUART__, 255
} ;

begin_bank( myPresets )             // these presets will be loaded

   _preset( Beat0 )

end_bank()

/******************************************************************************
 *
 *                                 BeatMenu
 *
 *            The number of beats per meaure is set as follows:
 *
 *           -------------------------------------------------
 *           |   |   |   |   |   |   |   |   |   |   |   |   |
 *           |   |   |   |   |   |   |   |   |   |   |   |   |
 *           |   |   |   |   |   |   |   |   |   |   |   |   |
 *           |   | 2 |   | 4 |   |   | 7 |   | 9 |   | 1 |   |
 *           |   |   |   |   |   |   |   |   |   |   | 1 |   |
 *           |   |   |   |   |   |   |   |   |   |   |   |   |
 *           |   |   |   |   |   |   |   |   |   |   |   |   |
 *           |    ___     ___    |    ___     ___     ___    |
 *           |     |       |     |     |       |       |     |
 *           |     |       |     |     |       |       |     |
 *           |  1  |   3   |  5  |  6  |   8   |   10  |  12 |
 *           |     |       |     |     |       |       |     |
 *           |     |       |     |     |       |       |     |
 *           -------------------------------------------------
 *
 ******************************************************************************/

class BeatMenu : public ByteMenu
{
   public:

   byte evaluate( byte keypos ) { return keypos+1; }

}  beatMenu ;


/******************************************************************************
 *
 *                                MeasureMenu
 *
 *
 *               The number of meaures is set as follows:
 *
 *           -------------------------------------------------
 *           |   |   |   |   |   |   |   |   |   |   |   |   |
 *           |   |   |   |   |   |   |   |   |   |   |   |   |
 *           |   |   |   |   |   |   |   |   |   |   |   |   |
 *           |   | 2 |   | 4 |   |   | 7 |   | 9 |   | 1 |   |
 *           |   |   |   |   |   |   |   |   |   |   | 2 |   |
 *           |   |   |   |   |   |   |   |   |   |   |   |   |
 *           |   |   |   |   |   |   |   |   |   |   |   |   |
 *           |    ___     ___    |    ___     ___     ___    |
 *           |     |       |     |     |       |       |     |
 *           |     |       |     |     |       |       |     |
 *           |  1  |   3   |  5  |  6  |   8   |   10  |  16 |
 *           |     |       |     |     |       |       |     |
 *           |     |       |     |     |       |       |     |
 *           -------------------------------------------------
 *
 ******************************************************************************/

class MeasureMenu : public ByteMenu
{
   public:

   byte evaluate( byte keypos )
   {
      if ( keypos <= 9 )
         return keypos+1;
      else if ( keypos == 10 )
         return 12;
      else
         return 16;
   }

}  measureMenu ;


/******************************************************************************
 *
 *                                DrumKit
 *
 ******************************************************************************/

class DrumKit : public Voice
{
   typedef Voice super;             // superclass is Voice

   public:

   DrumKit()
   {
      useSequencer( new RealTimeSequencer( 120 ) );
   }

   void    noteOn( key );           // turn a note on

} ;

/*----------------------------------------------------------------------------*
 *
 *  Name:  DrumKit::noteOn
 *
 *  Desc:  Turn a note on.
 *
 *  Args:  note             - note to turn on
 *
 *  Memb:  osc              - ptr to raw oscillator
 *
 *  Note:  If you want to use the __FULLHOST__ runtime model, comment out
 *         the "#error" statement after #ifndef __STNDLONE__ near the top of
 *         this sketch. When compiled with the __FULLHOST__ model, a kit of
 *         just Bass and Snare is used.
 *
 *----------------------------------------------------------------------------*/

void DrumKit::noteOn( key note )
{
   SampleOsc *o = (SampleOsc *)osc;
   byte     pos = note.position();

   #ifdef USE_SERIAL_PORT  // use reduced kit when the console is enabled

   if ( pos <= 4 )

      o->setSample( wavetable( lofi_Kick02 ) );

   else

      #ifdef BUILD_166
         o->setSample( wavetable( Snare01 ) );
      #else
         o->setSample( wavetable( lofi_Snare01 ) );
      #endif

   #else  // use full kit

   if ( pos <= 2 )

      o->setSample( wavetable( lofi_Kick02 ) );

   else if ( pos <= 4 )

      o->setSample( wavetable( lofi_Tom02 ) );

   else if ( pos <= 7 )

      #ifdef BUILD_166
         o->setSample( wavetable( Snare01 ) );
      #else
         o->setSample( wavetable( lofi_Snare01 ) );
      #endif

   else if ( pos <= 9 )

      o->setSample( wavetable( Rim01 ) );

   else

      o->setSample( wavetable( Hat03 ) );

   #endif

   trigger();
}

/******************************************************************************
 *
 *                                  Bass
 *
 ******************************************************************************/

class Bass : public Voice
{
   typedef Voice super;                // superclass is Voice

   public:

   bool charEv( char code )            // handle a character event
   {
      switch ( code )
      {
         case '!':                     // perform a reset

            super::charEv( code );
            execute( PSTR( "ed88\\s0\\r8\\" ));
            execute( PSTR( "Or1.006\\0c70\\p45\\lf.27\\<``1c70\\p96\\l<" ));
            break;

         default:

            return super::charEv( code );
      }
      return true;
   }

} ;

/******************************************************************************
 *
 *                               Beatitude
 *
 ******************************************************************************/

class Beatitude : public VoxSynth
{
   typedef VoxSynth super;          // superclass is VoxSynth

   bool clickOn;                    // if true, click track is playing
   bool cueing;                     // if true, sequencer is cueing to record
   bool liveBass;                   // if true, bass is being played live

   ClickTrack *click;               // ptr to click track
   DrumKit    *drums;               // ptr to drumkit voice
   Bass       *bass;                // ptr to bass voice

   public:

   Beatitude()
   {
      setupVoices(2);
      click = new ClickTrack();
      presets.load( myPresets );
   }

   void setup()
   {
      super::setup();
      keybrd.setTopOct( 2 );         // constrain keyboard to octaves 0-2
      keybrd.setDefOct( 1 );         // start keyboard in octave 1
   }

   void welcome()
   {
      execute( PSTR("L0\\") );      // load beat 0 when 1st started
   }

   bool   charEv( char code );      // handle a character event
   void   dynamics();               // update dynamics
   bool   evHandler( obEvent );     // handle an onboard event
   Osc   *newOsc( byte nth );       // create oscillator for nth voice
   Voice *newVox( byte nth );       // create nth voice
   void   noteOn( key );            // turn a note on
   void   noteOff( key );           // turn a note off
   void   output( char*, char* );   // write stereo output to pair of audio buffers

   void   setCueing( bool status )  // set cueing on/off
   {
      cueing = status;
      drums->setMute( status );
   }

} ;

/*----------------------------------------------------------------------------*
 *
 *  Name:  Beatitude::charEv
 *
 *  Desc:  Process a character event.
 *
 *  Args:  code             - character to process
 *
 *  Memb: +cueing           - if true, sequencer is cueing to record
 *         click            - ptr to click track
 *        +clickOn          - if true, click track is playing
 *        +liveBass         - if true, bass is being played live
 *         sqnc             - ptr to sequencer object
 *
 *  Rets:  status           - true if character was handled
 *
 *----------------------------------------------------------------------------*/

bool Beatitude::charEv( char code )
{
   switch ( code )
   {
      case sqncUPBEAT:                 // sequencer is at an upbeat

         click->upbeat();
         break;

      case sqncCUE:                    // sequencer is cueing to record

         clickOn = true;
         setCueing( true );
         onLED( 1 );

         // fall thru to sqncDNBEAT

      case sqncDNBEAT:                 // sequencer is at a downbeat

         click->downbeat();
         break;

      case sqncRECON:                  // sequencer recording switched on

         drums->setMute( false );      // unmute
         setCueing( false );
         blinkLED( 1 );                // blink to indicate recording
         break;

      case sqncRECOFF:                 // sequencer recording switched off

         clickOn = false;
         setCueing( false );
         offLED( 1 );
         break;

      case sqncPLAYON:                 // sequencer is playing back

         blinkLED( 0 );
         liveBass = true;
         break;

      case sqncPLAYOFF:                // sequencer has stopped playing back

         offLED( 0 );
         liveBass = false;
         break;

      case 'L':                        // load a beat
      {
         byte sqncNum;
         if ( console.getByte( CONSTR("sqnc #"), &sqncNum ) )
         {
            switch ( sqncNum )
            {
               case 0:
                  drums->sqnc->load( beat0 );
                  break;
               default:
                  break;
            }
         }
         break;
      }

      case '[':

         drums->sqnc->start();
         break;

      case '!':                        // perform a reset
      {
         super::charEv( code );
         click->reset();

         clickOn  = false;
         setCueing( false );
         liveBass = false;

         drums->keybrd.setMute( false );

         RealTimeSequencer *sqnc = (RealTimeSequencer *)drums->sqnc;
         sqnc->ignoreKeyUp = true;

         break;
      }
      default:

         return super::charEv( code );
   }
   return true;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Beatitude::dynamics
 *
 *  Desc:  Perform a dynamic update.
 *
 *  Memb:  click            - ptr to click track
 *
 *----------------------------------------------------------------------------*/

void Beatitude::dynamics()
{
   super::dynamics();
   click->dynamics();
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Beatitude::evHandler
 *
 *  Desc:  Handle an onboard event.
 *
 *  Args:  ev               - onboard event
 *
 *  Memb:  clickOn          - if true, click track is playing
 *         drums->sqnc      - ptr to drum kit's sequencer
 *
 *  Rets:  status           - true if the event was handled
 *
 *----------------------------------------------------------------------------*/

bool Beatitude::evHandler( obEvent ev )
{
   RealTimeSequencer *sqnc = (RealTimeSequencer *)drums->sqnc;

   switch ( ev.type() )
   {
      case POT1:                       // set Tempo
      case BUT0_PRESS:                 // play/stop sequencer playback

         return sqnc->evHandler(ev);

      case BUT1_PRESS:                 // record a sequence

         drums->sqnc->record();
         console.runModeWhile( sqnc, &this->clickOn );
         break;

      case BUT0_TPRESS:                // set beats and measures

         if ( ! sqnc->recording() && ! sqnc->playing() )
         {
            onLED( 0 );

            beatMenu.waitKey();
            sqnc->setBeats( beatMenu.value );

            measureMenu.waitKey();
            sqnc->setMeasures( measureMenu.value );

            offLED( 0 );
         }
         break;

      default:

         return super::evHandler( ev );
   }
   return true;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Beatitude::newOsc
 *
 *  Desc:  Return a pointer to the oscillator which is to be used by the nth
 *         voice.
 *
 *  Args:  nth              - voice # (0-based)
 *
 *  Rets:  ptrOsc           - pointer to oscillator object.
 *
 *  Note:  This method is automatically called once per voice by setupVoices().
 *         It is not meant to be called from anywhere else!
 *
 *         By overriding this method you can customize which kind of
 *         oscillators are used by which voices.
 *
 *         The return type of this method is Osc*, meaning that it can return
 *         a ptr to an object of any type which has Osc as a base class.
 *
 *----------------------------------------------------------------------------*/

Osc *Beatitude::newOsc( byte nth )
{
   switch( nth )
   {
      case 0:  // oscillator for drums

         return new SampleOsc();

      case 1:  // oscillator for bass

         return new DualOsc( new Square(), new Square() );
   }
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Beatitude::newVox
 *
 *  Desc:  Return a pointer to the nth voice to be used by the synth.
 *
 *  Args:  nth              - voice # (0-based)
 *
 *  Rets:  ptrVox           - pointer to voice object.
 *
 *  Memb: +bass             - ptr to bass voice
 *        +drums            - ptr to drumkit voice
 *
 *  Note:  This method is automatically called once per voice by setupVoices().
 *         It is not meant to be called from anywhere else!
 *
 *         By overriding this method you can customize which kind of
 *         voices are used by the synth.
 *
 *         The return type of this method is Voice*, meaning that it can
 *         return a ptr to an object of any type which has Voice as a base
 *         class.
 *
 *----------------------------------------------------------------------------*/

Voice *Beatitude::newVox( byte nth )
{
   switch( nth )
   {
      case 0:
      {
         drums = new DrumKit();
         return drums;
      }
      case 1:
      {
         bass = new Bass();
         return bass;
      }
   }
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Beatitude::noteOn
 *
 *  Desc:  Turn a note on.
 *
 *  Args:  note             - note to turn on
 *
 *  Memb:  bass             - ptr to bass voice
 *         cueing           - if true, sequencer is cueing to record
 *         drums            - ptr to drumkit voice
 *         liveBass         - if true, bass is being played live
 *
 *----------------------------------------------------------------------------*/

void Beatitude::noteOn( key note )
{
   if ( cueing )                 // suppress notes during cueing
      return;

   if ( liveBass )
      bass->noteOn( note );
   else
      drums->noteOn( note );
}


/*----------------------------------------------------------------------------*
 *
 *  Name:  Beatitude::noteOff
 *
 *  Desc:  Turn a note off.
 *
 *  Args:  note             - note to turn off
 *
 *  Memb:  bass             - ptr to bass voice
 *         cueing           - if true, sequencer is cueing to record
 *         drums            - ptr to drumkit voice
 *         liveBass         - if true, bass is being played live
 *
 *----------------------------------------------------------------------------*/

void Beatitude::noteOff( key note )
{
   if ( cueing )                 // suppress notes during cueing
      return;

   if ( liveBass )
      bass->noteOff( note );
   else
      drums->noteOff( note );
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Beatitude::output
 *
 *  Desc:  Write (stereo) output to a pair of audio buffers.
 *
 *  Args:  bufL             - ptr to left audio buffer
 *         bufR             - ptr to right audio buffer
 *
 *  Memb:  bass             - ptr to bass voice
 *         drums            - ptr to drumkit voice
 *         click            - ptr to click track
 *         clickOn          - if true, click track is playing
 *         liveBass         - if true, bass is being played live
 *
 *----------------------------------------------------------------------------*/

void Beatitude::output( char *bufL, char *bufR )
{
   drums->output( bufL );

   if ( clickOn )
   {
      click->output( bufR );

      Int reg;
      for ( byte i = 0; i < audioBufSz; i++ )  // right buf 1/2 click 1/2 drums
      {
         reg.val = bufR[i] + bufL[i];
         reg.val >>= 1;
         bufR[i] = reg._.lsb;
      }
   }
   else if ( liveBass )
   {
      bass->output( bufR );

      Int reg;
      for ( byte i = 0; i < audioBufSz; i++ )  // right buf 3/4 bass 1/4 drums
      {
         reg.val  = bufR[i] ;
         reg.val *= 3;
         reg.val += bufL[i];
         reg.val >>= 2;
         bufR[i] = reg._.lsb;
      }
   }
   else  // copy kit to right channel
   {
      for ( byte i = 0; i < audioBufSz; i++ )
         bufR[i] = bufL[i];
   }
}

Beatitude mySynth;

void setup()
{
   ardutouch_setup( &mySynth );
}

void loop()
{
   ardutouch_loop();
}


