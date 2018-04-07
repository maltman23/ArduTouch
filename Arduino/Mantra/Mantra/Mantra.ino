//
//  Mantra.ino
//
//  A play-along synth in an exotic scale.
//
//  How to use:
//
//  There are two drone sequences, A and B, which you can play along to.
//
//  Drone A is pre-loaded and ready to play when Mantra is turned on. 
//
//  To start the drone press the left button. The red LED will begin blinking, 
//  and the drone -- which consists of percussion, bass, and a melodic obligato 
//  part -- will begin playing. Now you can play a "lead" over it.
//
//  To stop the drone, press the left button again. The red LED will stop
//  blinking.
//
//  To choose drone B, double-tap the right button and press D on the keybrd.
//  To choose drone A, double-tap the right button and press low C.
//
//  The keyboard is tuned to an exotic scale. 
//
//  The "white" keys are the scale itself (so you can't go wrong if you 
//  just stay on the white keys :)
// 
//  The "black" keys repeat the note of a neighboring white key with an added
//  "bend" to the note.
//
//  The top pot controls the portamento speed of the lead voice. Turning it
//  all the way to the right turns portamento off.
//
//  The bottom pot controls the speed and intensity of an autowah effect on
//  the lead voice. Turning it all the way to the left turns the autowah off.
//
//  To raise the lead voice by an octave, tap the right button once.
//  To lower the lead voice by an octave, tap the left button once.
//
//  ---------------------------------------------------------------------------
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

#ifndef __STNDLONE__
   #error This sketch requires the __STNDLONE__ runtime model (Model.h)
#endif

#ifndef IMPLICIT_SEQUENCER
   #error This sketch requires IMPLICIT_SEQUENCER to be defined (Model.h)
#endif

about_program( Mantra, 0.90 )                // specify sketch name & version
set_baud_rate( 115200 )                      // specify serial baud-rate

/*----------------------------------------------------------------------------*
 *                                 Drones
 *----------------------------------------------------------------------------*/      

struct Drone                // initialization info for a drone of 3 parts
{
   const byte *beat;        // ptr to drum sequence  
   const byte *bass;        // ptr to bass sequence 
   const byte *oblig;       // ptr to obligato sequence 
   double tempo;            // tempo
   double tuning;           // tuning scalar 
} ;


const byte beat0[] PROGMEM = 
{ 
   16,
   0, QUART_*3, 0, QUART_*4, 8, QUART_,
   0, QUART_*3, 0, QUART_*2, 8, QUART_*2, 8, QUART_,
   255 
} ;

const byte bass0[] PROGMEM = 
{ 
   8,
   12, QUART_*8,  
   12, QUART_*6, 14, QUART_*2,
   255 
} ;

#define OB_OCT 3*16                 // octave portion of obligato notes
const byte oblig0[] PROGMEM = 
{ 
   16,
   OB_OCT+11, QUART_*5, OB_OCT+9, QUART_*3, OB_OCT+7, QUART_*5, 
   OB_OCT+5, QUART_*3, OB_OCT+4, QUART_*5, OB_OCT+2, QUART_*3, 
   OB_OCT+0, QUART_*8,
   255 
} ;


#define OCT0  0*16
#define OCT1  1*16
#define OCT2  2*16
#define OCT3  3*16

const byte beat1[] PROGMEM = 
{ 
   62,
   0, QUART_*3, 0, QUART_*2, 5, QUART_, 0, QUART_*4,
   0, QUART_*3, 0, QUART_*5, 5, QUART_*2,
   0, QUART_*2, 5, QUART_, 0, QUART_*3, 0, QUART_*4,
   0, QUART_*3, 0, QUART_*2, 5, QUART_*3, 0, QUART_*2,
   0, QUART_*3, 0, QUART_*2, 5, QUART_, 0, QUART_*4,
   0, QUART_*3, 0, QUART_*5, 5, QUART_*2,
   0, QUART_*2, 5, QUART_, 0, QUART_*3, 0, QUART_*4,
   0, QUART_*3, 0, QUART_*4, 5, QUART_*2, 5, QUART_,
   255 
} ;

const byte bass1[] PROGMEM = 
{ 
   28,
   OCT1+0, QUART_*6, OCT1+0, QUART_*2, OCT1+0, QUART_*2,
   OCT1+2, QUART_*5, OCT1+2, QUART_, OCT1+2, QUART_*2, OCT1+2, QUART_*2,
   OCT1+0, QUART_*6, OCT1+0, QUART_*2, OCT1+0, QUART_*2,
   OCT1+2, QUART_*6, OCT1+2, QUART_*2, OCT1+2, QUART_*2,
   255 
} ;

const byte oblig1[] PROGMEM = 
{ 
   72,
   tokenRest, QUART_, 
   tokenRest, QUART_, OCT3+11, QUART_, OCT3+9, QUART_, OCT3+7, QUART_*7,
   tokenRest, QUART_, OCT3+9, QUART_, OCT3+7, QUART_, OCT3+5, QUART_*7,
   tokenRest, QUART_, OCT3+7, QUART_, OCT3+5, QUART_, OCT3+4, QUART_*7,
   tokenRest, QUART_, OCT3+9, QUART_, OCT3+7, QUART_, OCT3+5, QUART_*7,

   tokenRest, QUART_, OCT3+11, QUART_, OCT3+9, QUART_, OCT3+7, QUART_*7,
   tokenRest, QUART_, OCT3+9, QUART_, OCT3+7, QUART_, OCT3+5, QUART_*7,
   tokenRest, QUART_, OCT3+4, QUART_, OCT3+5, QUART_, OCT3+7, QUART_*7,
   tokenRest, QUART_*2, OCT3+2, QUART_, OCT3+4, QUART_, OCT3+5, QUART_, 
   OCT3+4, QUART_*2, OCT3+2, QUART_*2,
   255 
} ;

#define WHOLE_TONE 1.122460      // interval between whole tones
#define SEMI_TONE  1.059462      // interval between semi tones    
#define QUART_TONE 1.029301      // interval between quarter tones 

const Drone droneA PROGMEM = { beat0, bass0, oblig0, 148.0, 1.00 };
const Drone droneB PROGMEM = { beat1, bass1, oblig1, 178.0, WHOLE_TONE };

/*----------------------------------------------------------------------------*
 *                               MantraTuning
 *----------------------------------------------------------------------------*/      

class MantraTuning : public Tuning
{
   private:

   static const byte   numNotes = 13;     // number of notes in an octave (+1)
   static const double freq[ numNotes ];  // frequencies for notes in octave 0

   public:

   MantraTuning()
   {
      scalar = 1.00;                       
   }

   volatile double scalar;                // global tuning scalar for all keys

   double pitch( key k )                  // return frequency for a given key
   {
      double f = pgm_read_float_near( freq + k.position() );
      return f * scalar * ( 1 << k.octave() );
   }

} ;

// This tuning maps an "Eastern" scale to the white keys.
// Black keys repeat neighboring white keys.

const double MantraTuning::freq[] PROGMEM = 
{ 
   16.351601, 16.351601, 17.323906,
   17.323906, 20.601718, 21.826757,
   21.826757, 24.499726, 24.499726,
   25.956562, 25.956562, 29.135234, 
   32.703202
} ;

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
      useSequencer( new SequencerROM() );
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
 *  Note:  A low 'C' plays a low tom. All other keys play a high tom.
 *
 *----------------------------------------------------------------------------*/      

void DrumKit::noteOn( key note )
{
   SampleOsc *o = (SampleOsc *)osc;
   byte     pos = note.position();

   switch ( pos )
   {
      case 0:
         o->setSample( wavetable( lofi_Tom03 ) );
         break;
      default:
         o->setSample( wavetable( lofi_Tom04 ) );
   }
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

   Bass()
   {
      useSequencer( new SequencerROM() );
   }

   bool charEv( char code )            // handle a character event
   {                                      
      switch ( code )
      {
         case '!':                     // perform a reset

            super::charEv( code );
            execute( PSTR( "ed88\\s120\\r8\\`0c70\\p45\\lf.27\\``1c70\\p96\\l" ));
            break;

         case sqncPLAYOFF:

            super::charEv( code );
            execute( PSTR("O0l.``1l.") );   // mute "clicky" pw LFOs
            break;

         case sqncPLAYON:

            execute( PSTR("O0l<``1l<") );    // unmute pw LFOs

         default:                         

            return super::charEv( code );
      }
      return true;
   }

} ;

/******************************************************************************
 *
 *                              Obligato
 *
 ******************************************************************************/

class Obligato : public Voice
{
   typedef Voice super;                // superclass is Voice
   
   public:

   DelayVibrato  vibrato;
   FiltEnv       filt;

   Obligato()
   {
      useSequencer( new SequencerROM() );
      addPitchMod( &vibrato );
      addEffect( &filt );
   }

   bool charEv( char code )            // handle a character event
   {                                      
      switch ( code )
      {
         case '!':                     // perform a reset

            super::charEv( code );
            execute( PSTR( "Pv-<D203\\d.3\\t8\\" ) );
            execute( PSTR( "ea38\\d135\\s32\\" ) );
            execute( PSTR( "Ef<c194\\ea2\\d96\\s96\\" ) );
            break;

         default:                         

            return super::charEv( code );
      }
      return true;
   }

} ;

/******************************************************************************
 *
 *                                  Bend
 *
 * This simple "Bend" control will oscillate in value from 1.0 to 2.0.
 *
 * We derive it from the library's TermLFO class ("Terminating LFO").
 * The TermLFO class implements a Low Frequency Oscillator that can be 
 * configured to traverse a specific number of half cycles after being
 * triggered and then stop oscillating.
 *
 * (In the "reset" code for our synth further down in this file, we will 
 * initialize our Ramp control so that it traverses 2 half cycles. 
 * This means that it will start at 1.0, move to 2.0, return to 1.0, and 
 * then stop oscillating, each time a note is played).
 *
 ******************************************************************************/

class Bend : public TermLFO            // bend frequency effect
{
   typedef TermLFO super;              // superclass is TermLFO

   public:

   // We set the shortcut state variable to 'b'. 
   // This allows us to access our control by typing 'b' 
   // when we are at the "Pitch>" prompt in the Console.

   Bend() 
   {
      shortcut = 'b';                  
   }

   // The evaluate() method is automatically called by the system at each dynamic
   // update. This is where we set the output value of our control.

   // The LFO state variable "pos", which is referenced here, oscillates between 
   // 0.0 and 1.0 and is automatically computed at each dynamic update before 
   // evaluate() is called.

   void evaluate()
   {
      value = 1.0 + (pos*(1.05946-1));  // value will be between 1.0 and semi-tone
   }

   // we set the prompt for our control to "bend". 
   // this is also the name which will be displayed for our control in the
   // Console when we type '?' at the "Pitch>" prompt.
   // (typing '?' at the "Pitch>" prompt lists all the pitch modifiers).

   #ifdef CONSOLE_OUTPUT
   const char *prompt()
   {
      return CONSTR( "bend" );
   }
   #endif

} ;

/******************************************************************************
 *
 *                                  Lead 
 *
 ******************************************************************************/

class Lead : public Voice
{
   typedef Voice super;                // superclass is Voice

   public:

   AutoWah       wah;
   Bend          bend;
   DelayVibrato  vibrato;
   
   Lead()
   {
      addEffect( &wah );
      addPitchMod( &bend );
      addPitchMod( &vibrato );
   }

   bool charEv( char code )            // handle a character event
   {                                      
      switch ( code )
      {
         case 'a':                     // set autowah
         {
            byte v;
            if ( console.getByte( CONSTR("auto"), &v ) )
               setAutoWah( v );
         }
         case '!':                     // perform a reset

            super::charEv( code );
            execute( PSTR("Ea<") );
            execute( PSTR("Pb-f2\\t2\\d1\\<`v<D100\\d.16\\t27\\") );
            execute( PSTR("ea18\\d120\\s220\\r50\\") );
            setAutoWah( 26 );
            setGlide( 225 );
            break;

         default:                         

            return super::charEv( code );
      }
      return true;
   }

   void noteOn( key note )             // turn a note on
   {
      switch ( note.position() )
      {
         case 1:                       // enable bend effect for black keys
         case 3:
         case 6:
         case 8:
         case 10:
            bend.setMute( false );
            break;
         default:                      // disable bend effect for white keys
            bend.setMute( true );
      }
      super::noteOn( note );           // play the note
   }

   void setAutoWah( byte val )
   {
      #define loFreq  0.50
      #define hiFreq  4.75
      #define loDepth 0.55
      #define hiDepth 1.00

      double percent = (double )val / 255.0;

      wah.lfo.setFreq( loFreq + ((hiFreq - loFreq) * percent) );
      if ( val > 0 )
         wah.lfo.setDepth( loDepth + ((hiDepth - loDepth) * percent) );
      else
         wah.lfo.setDepth( 0.00 );
   }

} ;

/******************************************************************************
 *
 *                               Mantra 
 *
 ******************************************************************************/

class Mantra : public VoxSynth
{
   typedef VoxSynth super;          // superclass is VoxSynth

   Lead     *lead;                  // ptr to lead voice     (same as vox[0])

   // the following 3 voices comprise the "drone":

   DrumKit  *drums;                 // ptr to drumkit voice  (same as vox[1])
   Bass     *bass;                  // ptr to bass    voice  (same as vox[2])
   Obligato *oblig;                 // ptr to obligato voice (same as vox[3])

   #define begDRONE 1               // indice of 1st drone voice in vox[]
   #define endDRONE 3               // indice of last drone voice in vox[]

   public:

   Mantra()
   {
      setupVoices(4);
      masterTuning = new MantraTuning();
   }

   void setup()
   {
      super::setup();
      keybrd.setDefOct( 4 );         // start keyboard in octave 4
   }

   bool   charEv( char code );      // handle a character event
   bool   evHandler( obEvent );     // handle an onboard event
   void   loadDrone(const Drone*);  // load the drone voice sequencers
   char   menu( key );              // given a key, return a character 
   Osc   *newOsc( byte nth );       // create oscillator for nth voice
   Voice *newVox( byte nth );       // create nth voice 
   void   output( char*, char* );   // write stereo output to pair of audio buffers
   void   start();                  // start playback of drone sequencers
   void   stop();                   // stop playback of drone sequencers

} ;

/*----------------------------------------------------------------------------*
 *
 *  Name:  Mantra::charEv
 *
 *  Desc:  Process a character event.
 *
 *  Args:  code             - character to process
 *
 *  Memb:  bass             - ptr to bass voice
 *         drums            - ptr to drums voice
 *         oblig            - ptr to obligato voice
 *         vox[]            - array of ptrs to synth voices
 *
 *  Rets:  status           - true if character was handled
 *
 *----------------------------------------------------------------------------*/      

bool Mantra::charEv( char code )      
{
   switch ( code )
   {
      case 'A':                        // load and play droneA

         loadDrone( &droneA );
         start();  
         break;

      case 'B':                        // load and play droneB

         loadDrone( &droneB );
         start();
         break;

      case '[':                        // start drone sequences

         start();
         break;

      case ']':                        // stop drone sequences

         stop();
         break;

      case '!':                        // perform a reset

         super::charEv( code );

         for ( byte i = begDRONE; i <= endDRONE; i++ )
            vox[i]->keybrd.setMute( false );

         loadDrone( &droneA );  
         break;

      default:                         
         
         return super::charEv( code );
   }
   return true;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Mantra::evHandler
 *
 *  Desc:  Handle an onboard event.
 *
 *  Args:  ev               - onboard event
 *
 *  Memb:  drums            - ptr to drum kit voice 
 *
 *  Rets:  status           - true if the event was handled
 *
 *----------------------------------------------------------------------------*/      

bool Mantra::evHandler( obEvent ev )
{
   switch ( ev.type() )
   {
      case BUT0_PRESS:                 // toggle playing of drone sequences

         if ( drums->sqnc->playing() )
            charEv(']');
         else
            charEv('[');
         break;

      case POT0:                       // set portamento rate for lead
            
         lead->setGlide( ev.getPotVal()+1 );
         break;
            
      case POT1:       

         lead->setAutoWah( ev.getPotVal() );
         break;

      default: 

         return super::evHandler( ev );
   }
   return true;                 
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Mantra::loadDrone
 *
 *  Desc:  Load the drone voice sequencers.
 *
 *  Args:  drone            - ptr to drone struct in ROM
 *
 *  Glob:  mantraTuning     - master tuning object for synth
 *
 *  Memb:  bass             - ptr to bass voice
 *         drums            - ptr to drums voice
 *         oblig            - ptr to obligato voice
 *         vox[]            - array of ptrs to synth voices
 *
 *----------------------------------------------------------------------------*/      

void Mantra::loadDrone( const Drone *drone )
{
   Drone d;                            // buffer for RAM copy of drone

   // copy drone struct to RAM, so that we can manipulate it field-wise

   const byte *source = (const byte *)drone;
   byte *dest = (byte *)&d;

   for ( byte i = 0; i < sizeof( Drone ); i++ )
      *dest++ = pgm_read_byte_near( source++ );

   // load the sequencers

   drums->sqnc->load( d.beat );
   bass->sqnc->load( d.bass );
   oblig->sqnc->load( d.oblig );

   // set the sequencer tempos

   for ( byte i = begDRONE; i <= endDRONE; i++ )
      vox[i]->sqnc->setTempo( d.tempo );

   // set the tuning scalar

   ((MantraTuning *)masterTuning)->scalar = d.tuning;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Mantra::menu
 *
 *  Desc:  Given a key, return a character (to be processed via charEv()). 
 *
 *  Args:  k                - key
 *
 *  Rets:  c                - character (0 means "no character")
 *
 *  Note:  If a sketch is compiled with KEYBRD_MENUS defined, then this method 
 *         can be used to map the onboard keys to characters which the system 
 *         will automatically feed to the charEv() method.
 *
 *         This method is only called by the system if the MENU flag in this
 *         object is set (in the ::flags byte inherited from Mode), or if the
 *         keyboard is in a "oneShot menu selection" state.
 *
 *         The key mapping (inclusive of super class) is as follows:
 *
 *           -------------------------------------------------
 *           |   |   |   |   |   |   |   |   |   |   |   |   |
 *           |   |   |   |   |   |   |   |   |   |   |   |   |
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |   | # |   |   |   |   |   |   | k |   | . |   | 
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |    ___     ___    |    ___     ___     ___    | 
 *           |     |       |     |     |       |       |     |
 *           |     |       |     |     |       |       |     |
 *           |  A  |   B   |     |     |   x   |   <   |  !  |
 *           |     |       |     |     |       |       |     |
 *           |     |       |     |     |       |       |     |
 *           -------------------------------------------------
 *
 *----------------------------------------------------------------------------*/      

char Mantra::menu( key k )
{
   switch ( k.position() )
   {
      case  0: return 'A';
      case  2: return 'B';
      default: return super::menu(k);
   }
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Mantra::newOsc
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

Osc *Mantra::newOsc( byte nth )                
{
   switch( nth )
   {  
      case 0:  // oscillator for lead
      {
         WaveOsc *o = new WaveOsc();
         o->setTable( wavetable_named( Rood ) ); 
         return o;
      }
      case 1:  // oscillator for drums

         return new SampleOsc();        

      case 2:  // oscillator for bass

         return new DualOsc( new Square(), new Square() );

      case 3:  // oscillator for obligato

         return new SawTooth();
   }
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Mantra::newVox
 *
 *  Desc:  Return a pointer to the nth voice to be used by the synth.
 *
 *  Args:  nth              - voice # (0-based)
 *
 *  Rets:  ptrVox           - pointer to voice object.
 *
 *  Memb: +bass             - ptr to bass voice
 *        +drums            - ptr to drumkit voice
 *        +lead             - ptr to lead voice
 *        +oblig            - ptr to obligato voice
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

Voice *Mantra::newVox( byte nth )              
{
   switch( nth )
   {
      case 0: 
      {
         lead = new Lead();
         return lead;
      }
      case 1: 
      {
         drums = new DrumKit();
         return drums;
      }
      case 2: 
      {
         bass = new Bass();
         return bass;
      }
      case 3:
      {
         oblig = new Obligato();
         return oblig;
      }
   }
}
   
/*----------------------------------------------------------------------------*
 *
 *  Name:  Mantra::output
 *
 *  Desc:  Write (stereo) output to a pair of audio buffers.
 *
 *  Args:  bufL             - ptr to left audio buffer  
 *         bufR             - ptr to right audio buffer  
 *
 *  Glob:  audioBufSz       - size of system audio buffers
 *
 *  Memb:  bass             - ptr to bass voice
 *         drums            - ptr to drums voice
 *         lead             - ptr to lead voice
 *         oblig            - ptr to obligato voice
 *
 *----------------------------------------------------------------------------*/      

void Mantra::output( char *bufL, char *bufR )
{
   char  bassBuf[ audioBufSz ];     // temp buffer for for holding bass output
   Int   reg;                       // temp register for 16 bit math

   lead->output( bufR );  
   oblig->output( bufL );           
   bass->output( &bassBuf[0] );

   for ( byte i = 0; i < audioBufSz; i++ )  // left buf: 
   {                                        // 3/4 lead 1/8 obligato 1/8 bass
      reg.val = bassBuf[i] + bufL[i];
      reg.val >>= 1;
      reg.val += bufR[i];
      reg.val >>= 1;
      reg.val += bufR[i];
      reg.val >>= 1;
      bufL[i] = reg._.lsb;
   }

   drums->output( bufR );

   for ( byte i = 0; i < audioBufSz; i++ )  // right buf: 1/2 bass 1/2 drums
   {
      reg.val = bufR[i] + bassBuf[i];
      reg.val >>= 1;
      bufR[i] = reg._.lsb;
   }
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Mantra::start
 *
 *  Desc:  Start the playback of the drone sequencers.
 *
 *  Memb:  vox[]            - array of ptrs to synth voices
 *
 *----------------------------------------------------------------------------*/

void Mantra::start()
{
   for ( byte i = begDRONE; i <= endDRONE; i++ )
      vox[i]->sqnc->start();
   onLED( 0 );
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Mantra::stop
 *
 *  Desc:  Stop the playback of the drone sequencers.
 *
 *  Memb:  vox[]            - array of ptrs to synth voices
 *
 *----------------------------------------------------------------------------*/

void Mantra::stop()
{
   for ( byte i = begDRONE; i <= endDRONE; i++ )
      vox[i]->sqnc->stop();
   offLED( 0 );
}

Mantra mySynth;

void setup()
{
   ardutouch_setup( &mySynth );                   
}

void loop()
{
   ardutouch_loop();                            
}                                             

                                             
