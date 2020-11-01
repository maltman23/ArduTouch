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
//         -----------       Adjusting the Bass Volume        ----------- 
//
//
//    Rotate the top pot to make the bass louder or softer.
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

about_program( Beatitude, 1.11 )             // specify sketch name & version

#ifndef __STNDLONE__
   #error This sketch requires the __STNDLONE__ runtime model (Model.h)
#endif

#ifndef IMPLICIT_SEQUENCER
   #error This sketch requires IMPLICIT_SEQUENCER to be defined (Model.h)
#endif

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

   // the panning coefficient

   bool  panRight;                  // if true pan to right (else, pan to left) 
   word  panCoeff;                  // panning coefficient (for opposite channel)

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
 *        +panRight         - if true pan to right (else, pan to left) 
 *        +panCoeff         - panning coefficient (for opposite channel)
 *
 *  Note:  If you want to use the __FULLHOST__ runtime model, comment out
 *         the "#error" statement after #ifndef __STNDLONE__ near the top of 
 *         this sketch. When compiled with the __FULLHOST__ model, a kit of 
 *         just Bass and Snare is used.
 *
 *----------------------------------------------------------------------------*/      

void DrumKit::noteOn( key note )
{
   byte pos = note.position();   // position of note within octave 

   // set panning defaults 

   panCoeff = 154;
   panRight = true;

   // set wavetable and panning coefficients based on note position

   const desWavTab *drumTable;   // ptr to wavetable descriptor 

   #ifdef USE_SERIAL_PORT        // use reduced kit when the console is enabled

   if ( pos <= 4 )
   {
      drumTable = wavetable( lofi_Kick02 );
      panRight = false;
   }
   else
   {
      drumTable = wavetable( Snare01 );
   }

   #else  // use full kit

   if ( pos <= 2 )
   {
      drumTable = wavetable( lofi_Kick02 );
      panRight = false;
   }
   else if ( pos <= 4 )
   {
      drumTable = wavetable( lofi_Tom02 );
      panRight = false;
      panCoeff = 90;
   }
   else if ( pos <= 7 )
   {
      drumTable = wavetable( Snare01 );
   }
   else if ( pos <= 9 )
   {
      drumTable = wavetable( Rim01 );
      panCoeff = 120;
   }
   else
   {
      drumTable = wavetable( Hat03 );
      panCoeff = 85;
   }
   #endif

   SampleOsc *o = (SampleOsc *)osc;    // typecast generic osc pointer
   o->setSample( drumTable );

   trigger();
}

/******************************************************************************
 *
 *                                  Bass
 *
 ******************************************************************************/

class Bass : public ADSRVoice
{
   typedef ADSRVoice super;            // superclass is ADSRVoice
   
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

   ByteMenu presetMenu;             // keybrd menu for selecting presets

   bool clickOn;                    // if true, click track is playing
   bool cueing;                     // if true, sequencer is cueing to record
   bool liveBass;                   // if true, bass is being played live

   byte scaleBass;                  // scale factor for bass (128 = 50%)

   ClickTrack *click;               // ptr to click track
   DrumKit    *drums;               // ptr to drumkit voice
   Bass       *bass;                // ptr to bass voice

   RealTimeSequencer *sqnc;         // synonym for drums->sqnc;

   public:

   void config()
   {
      configVoices(2);

      click = new ClickTrack();

      keybrd.setTopOct( 2 );         // constrain keyboard to octaves 0-2
      keybrd.setDefOct( 1 );         // start keyboard in octave 1

      sqnc = (RealTimeSequencer *)drums->sqnc;

      presets.load( myPresets );
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
                  sqnc->load( beat0 );
                  break;
               default: 
                  break;
            }
         }
         break;
      }

      case '[':                        // start the sequencer

         sqnc->start();
         break;

      case '!':                        // perform a reset
      {
         super::charEv( code );
         click->reset();

         clickOn  = false;
         setCueing( false );
         liveBass = false;

         scaleBass = 68;

         drums->keybrd.setMute( false );
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
 *        +scaleBass        - scale factor for bass (128 = 50%)
 *         sqnc             - ptr to drum kit's sequencer 
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

         sqnc->record();
         console.runModeWhile( sqnc, &this->clickOn );
         break;

      case POT0:                       // set balance between bass and drums
      
         scaleBass = 32 + ( ev.getPotVal() >> 2 );   // range from 32 to 96 
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

      case BUT1_DTAP:                  // select a preset 

         presetMenu.waitKey();
         execute( (const char *)presets.dataPtr( presetMenu.value ) );
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
 *  Note:  This method is automatically called once per voice by configVoices(). 
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
 *  Note:  This method is automatically called once per voice by configVoices(). 
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
 *  Glob:  audioBufSz       - size of system audio buffers
 *
 *  Memb:  bass             - ptr to bass voice
 *         drums            - ptr to drumkit voice
 *         click            - ptr to click track  
 *         clickOn          - if true, click track is playing
 *         liveBass         - if true, bass is being played live
 *         scaleBass        - scale factor for bass 
 *
 *----------------------------------------------------------------------------*/      

void Beatitude::output( char *bufL, char *bufR )
{
   // create stereo panned output of drums

   drums->output( bufL );  

   Int regX;

   for ( byte i = 0; i < audioBufSz; i++ )
   {
      regX.val  = bufL[i];
      regX.val *= drums->panCoeff;
      if ( drums->panRight )
      {
         bufR[i] = bufL[i];
         bufL[i] = regX._.msb;
      }
      else
         bufR[i] = regX._.msb;
   }

   // return if not recording or playing bass over drum playback

   if ( (! clickOn) && (! liveBass) ) 
      return;

   // Overlay click or bass output on drums output (scaling both)

   char bufOver[ audioBufSz ];               // temp buffer for click or bass output

   word scaleDrums;                          // scale factor for drums
   word scaleOver;                           // scale factor for overlay

   if ( clickOn )
   {
      click->output( &bufOver[0] );
      scaleOver = 64;
   }
   else
   {
      bass->output( &bufOver[0] );
      scaleOver = scaleBass;
   }

   scaleDrums = 256 - scaleOver;

   for ( byte i = 0; i < audioBufSz; i++ )   
   {
      char overlay;                          // scaled overlay value

      regX.val  = bufOver[i];
      regX.val *= scaleOver;
      overlay   = regX._.msb;

      regX.val  = bufR[i];
      regX.val *= scaleDrums;
      bufR[i]   = regX._.msb + overlay;

      regX.val  = bufL[i];
      regX.val *= scaleDrums;
      bufL[i]   = regX._.msb + overlay;
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

                                             
