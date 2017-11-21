//
//  Beatitude.ino
// 
//  A drum machine with real-time sequencer for the ArduTouch. 
// 
//  (bass and other goodies coming soon :)
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
//             |   | K |   | T |   |   | S |   | R |   | R |   | 
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
//    You can play along with it if you want.
//
//    When the sequence reaches its end it will loop back to the beginning
//    and continue to play (forever) until you press the left button again.
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

about_program( Beatitude, 0.66 )             // specify sketch name & version
set_baud_rate( 115200 )                      // specify serial baud-rate

/*----------------------------------------------------------------------------*
 *                                 presets
 *----------------------------------------------------------------------------*/      


define_preset( Beat0, "L0\\[" )

const byte beat0[] PROGMEM = 
{ 
   31,
   0, 4, 7, 6, 0, 2, 7, 4,
   0, 4, 7, 6, 0, 2, 7, 4,
   0, 4, 7, 6, 0, 2, 7, 6,
   0, 4, 0, 4, 0, 6, 255 
} ;

begin_bank( myPresets )             // these presets will be loaded

   _preset( Beat0 )

end_bank()

/******************************************************************************
 *
 *                                  Kit 
 *
 ******************************************************************************/

class Kit : public Voice
{
   typedef Voice super;             // superclass is Voice
   
   public:

   void    noteOn( key );           // turn a note on

} ;

/*----------------------------------------------------------------------------*
 *
 *  Name:  Kit::noteOn
 *
 *  Desc:  Turn a note on.
 *
 *  Args:  note             - note to turn on  
 *
 *  Memb:  osc              - ptr to raw oscillator
 *
 *  Note:  If you want to use the __FULLHOST__ runtime model, comment out
 *         the "#error" statement near the top of this sketch. When compiled
 *         with the __FULLHOST__ model, a kit of just Bass and Snare is used.
 *
 *----------------------------------------------------------------------------*/      

void Kit::noteOn( key note )
{

   SampleOsc *o = (SampleOsc *)osc;
   byte     pos = note.position();

   #ifdef USE_SERIAL_PORT  // use reduced kit when the console is enabled

   switch ( pos )
   {
      case 0:
      case 1:
      case 2:
      case 3:

         o->setTable( wavetable( Kick02 ) );
         break;

      case 4:
      case 5:

         o->setTable( wavetable( Tom02 ) );
         break;

      case 6:
      case 7:
      case 8:
      case 9:
      case 10:
      case 11:

         o->setTable( wavetable( Snare01 ) );
         break;
   }

   #else  // use full kit

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

   #endif

   trigger();
}

/******************************************************************************
 *
 *                              Metronome
 *
 ******************************************************************************/

class Metronome : public Voice
{
   typedef Voice super;                // superclass is Voice
   
   public:

   bool charEv( char code );           // handle a character event
   void downbeat();                    // strike a downbeat
   void upbeat();                      // strike an upbeat

} ;

/*----------------------------------------------------------------------------*
 *
 *  Name:  Metronome::charEv
 *
 *  Desc:  Process a character event.
 *
 *  Args:  code             - character to process
 *
 *  Rets:  status           - true if character was handled
 *
 *----------------------------------------------------------------------------*/      

bool Metronome::charEv( char code )      
{
   switch ( code )
   {
      case '!':

         super::charEv( code );
         execute( PSTR( "k<`es0\\d4\\" ) );
         break;

      default:                         
         
         return super::charEv( code );
   }
   return true;
}

void Metronome::downbeat()
{
   noteOn( key( 45 ) );
}

void Metronome::upbeat()
{
   noteOn( key( 30 ) );
}


/******************************************************************************
 *
 *                                 BeatMenu 
 *
 ******************************************************************************/

class BeatMenu : public Mode
{
   typedef Mode super;                 // superclass is Mode
   
   public:

   byte   numBeats;                    // number of beats
   bool   waiting;                     // if true, waiting for key press

   void getBeats( byte iniBeats )
   {
      numBeats = iniBeats;
      waiting  = true;
      console.runModeWhile( this, &this->waiting );
   }

   bool   evHandler( obEvent );        // handle an onboard event

}  beatMenu ;


/*----------------------------------------------------------------------------*
 *
 *  Name:  BeatMenu::evHandler
 *
 *  Desc:  Set the number of beats per measure by pressing a key.
 *
 *  Args:  ev               - onboard event
 *
 *  Rets:  status           - true if the event was handled
 *
 *         The number of beats per meaure is set as follows:
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
 *----------------------------------------------------------------------------*/      

bool BeatMenu::evHandler( obEvent ev )
{
   switch ( ev.type() )
   {
      case KEY_DOWN:
      
         numBeats = ev.getKey().position() + 1; 
         break;
      
      case KEY_UP:

         waiting = false;
         break;

      default:                 

         return super::evHandler( ev );
   }
   return true;                 
}


/******************************************************************************
 *
 *                                MeasureMenu 
 *
 ******************************************************************************/

class MeasureMenu : public Mode
{
   typedef Mode super;                 // superclass is Mode
   
   public:

   byte   numMeasures;                 // number of meaures
   bool   waiting;                     // if true, waiting for key press

   void getMeasures( byte iniMeasures )
   {
      numMeasures = iniMeasures;
      waiting  = true;
      console.runModeWhile( this, &this->waiting );
   }

   bool   evHandler( obEvent );        // handle an onboard event

}  measureMenu ;


/*----------------------------------------------------------------------------*
 *
 *  Name:  MeasureMenu::evHandler
 *
 *  Desc:  Set the number of measures by pressing a key.
 *
 *  Args:  ev               - onboard event
 *
 *  Rets:  status           - true if the event was handled
 *
 *         The number of meaures is set as follows:
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
 *----------------------------------------------------------------------------*/      

bool MeasureMenu::evHandler( obEvent ev )
{
   switch ( ev.type() )
   {
      case KEY_DOWN:
      {
         key k    = ev.getKey();
         byte pos = k.position();
         if ( pos <= 9 )
            numMeasures = pos+1;
         else if ( pos == 10 )
            numMeasures = 12;
         else
            numMeasures = 16;
         break;
      }
      case KEY_UP:

         waiting = false;
         break;

      default:                 

         return super::evHandler( ev );
   }
   return true;                 
}


/******************************************************************************
 *
 *                           RealTimeSequencer
 *
 ******************************************************************************/

   // the following are Metronome commands (generally issued by a Sequencer)

#define metroON         -11            // turn metronome on
#define metroOFF        -12            // turn metronome off
#define metroTICK       -13            // sound metronome's downbeat
#define metroTOCK       -14            // sound metronome's upbeat
#define metroCUE        -15            // Cue the metronome and turn it on 
#define metroUNCUE      -16            // Uncue the metronome (but keep it on) 

class RealTimeSequencer : public Sequencer
{
   typedef Sequencer super;            // superclass is Sequencer

   using Sequencer::Sequencer;         // use Sequencer constructor

   protected:

   // recording status values:

   static  const byte recOFF   = 0;    // not recording 
   static  const byte recCUE   = 2;    // cueing before recording
   static  const byte recTRANS = 3;    // transitioning from cueing to recording
   static  const byte recON    = 4;    // recording

   byte  recording;                    // recording status

   byte  beatDC;                       // downcounter (in jiffs) to next beat
   byte  transDC;                      // downcounter (in jiffs) to transition phase
   byte  measureDC;                    // downcounter (in beats) to next measure
   byte  endDC;                        // downcounter (in measures) to end of recording

   byte  jiffsPerBeat;                 // # jiffs in one beat
   byte  jiffsToTrans;                 // # jiffs from start of cueing to transition phase
   byte  beatsPerMeasure;              // # beats in one measure
   byte  numMeasures;                  // # measures in sequence

   byte  duration;                     // ongoing duration of prior entry being recorded
   bool  quantized;                    // if true, prior entry was quantized to next jiff
   bool  fullHouse;                    // if true, sequence buffer is full

   void  done();                       // executed at end of recording
   void  informFull();                 // print 'buffer full' msg on console

   public:

   bool  charEv( char code );          // handle a character event
   void  dynamics();                   // perform a dynamic update 
   bool  evHandler( obEvent );         // handle an onboard event
   void  setBeats( byte );             // set the number of beats per measure
   void  setMeasures( byte );          // set the number of measures in sqnc

} ;

#define midTempo    384.0              // default tempo (a medium gait)
#define maxBeats    11                 // maximum beats per measure
#define maxMeasures 16                 // maximum # of measures

/*----------------------------------------------------------------------------*
 *
 *  Name:  RealTimeSequencer::charEv
 *
 *  Desc:  Process a character event.
 *
 *  Args:  code             - character to process
 *
 *  Memb: +beatDC           - downcounter (in jiffs) to next beat
 *        +beatsPerMeasure  - # beats in one measure
 *        +endDC            - downcounter (in measures) to end of recording
 *        +fullHouse        - if true, sequence buffer is full
 *        +idx              - index of next record in sqnc[] 
 *        +jiffDC           - downcounter (in audio ticks) to next jiff
 *        +jiffsPerBeat     - # jiffs in one beat
 *        +measureDC        - downcounter (in beats) to next measure
 *        +numMeasures      - # measures in sequence
 *        +quantized        - if true, prior entry was quantized to next jiff
 *        +recording        - recording status
 *         target           - ptr to instrument being sequenced 
 *        +transDC          - downcounter (in jiffs) to transition phase
 *         ticksPerDyna     - # audio ticks per dynamic update 
 *         ticksPerJiff     - # audio ticks per jiff 
 *
 *  Rets:  status           - true if character was handled
 *
 *----------------------------------------------------------------------------*/      

bool RealTimeSequencer::charEv( char code )      
{
   switch ( code )
   {
      case 'r':                        // record a sequence

         super::charEv( code );

         recording = recCUE;
         target->charEv( metroCUE );

         idx       = 1;                // rewind to 1st record
         fullHouse = false;
         quantized = false;

         // initialize downcounters

         jiffDC    = ticksPerJiff;        
         beatDC    = jiffsPerBeat;
         measureDC = beatsPerMeasure;
         endDC     = numMeasures+1; 
         transDC   = jiffsToTrans;

         break;

      case focusPOP: 

         done();
         break;
      
      case '[':                        // playback-transport cmds
      case '|':

         super::charEv( code );
         if ( playing() )
            blinkLED( 0 );
         break;

      case ']':

         super::charEv( code );
         offLED( 0 );
         break;

      #ifdef INTERN_CONSOLE

      case 'b':                        // set beatsPerMeasure

         console.getByte( CONSTR("beats"), &beatsPerMeasure );
         setBeats( beatsPerMeasure );
         break;

      case 'm':                        // set # measures in sequence

         console.getByte( CONSTR("measures"), &numMeasures );
         setMeasures( numMeasures );
         break;

      #endif // INTERN_CONSOLE

      #ifdef CONSOLE_OUTPUT

      case chrInfo:

         super::charEv( code );
         console.newlntab();
         console.infoByte( CONSTR("beats"), beatsPerMeasure );
         console.infoByte( CONSTR("measures"), numMeasures );
         break;

      #endif // CONSOLE_OUTPUT

      case '!':                        // reset sequencer

         super::charEv( code );
         setTempo( midTempo );

         recording = recOFF;
         target->charEv( metroOFF );

         jiffsPerBeat    = 4;
         setMeasures(4);
         setBeats(4);
         break; 

      default:                         
         
         return super::charEv( code );
   }
   return true;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  RealTimeSequencer::done
 *
 *  Desc:  This routine should be called when a recording has ended or the 
 *         sequencer is popped.
 *
 *  Memb:  duration         - ongoing duration of note/rest in progress 
 *         fullHouse        - if true, sequence buffer is full
 *        +recording        - recording status
 *        +sqnc[]           - buffer for compiled sequence records
 *         target           - ptr to instrument being sequenced 
 *
 *----------------------------------------------------------------------------*/

void RealTimeSequencer::done()
{
   if ( recording )
   {
      if ( recording == recCUE )
         target->charEv( metroUNCUE );    // uncue metronome 
      else
      {
         if ( idx > 1 && ! fullHouse )    // compile duration for last entry
         {
            if ( duration )
               sqnc[ idx++ ] = duration; 
            else
               idx--;                     // on zero duration cull last entry
         }
      }
      sqnc[ idx ] = tokenEOS;
      recording = recOFF;           // turn recording off
      target->charEv( metroOFF );   // turn metronome off
   }
}


/*----------------------------------------------------------------------------*
 *
 *  Name:  RealTimeSequencer::dynamics
 *
 *  Desc:  Update the recording/playback of a sequence.
 *
 *  Memb: +beatDC           - downcounter (in jiffs) to next beat
 *        +duration         - ongoing duration of note/rest in progress 
 *        +endDC            - downcounter (in measures) to end of recording
 *        +jiffDC           - downcounter (in audio ticks) to next jiff
 *         jiffsPerBeat     - # jiffs in one beat
 *        +quantized        - if true, prior entry was quantized to next jiff
 *        +recording        - recording status
 *         target           - ptr to instrument being sequenced 
 *         ticksPerDyna     - # audio ticks per dynamic update 
 *         ticksPerJiff     - # audio ticks per jiff 
 *        +transDC          - downcounter (in jiffs) to transition phase
 *
 *  Note:  This method is to be called once per system dynamic update.       
 *
 *----------------------------------------------------------------------------*/

void RealTimeSequencer::dynamics()
{
   if ( recording )
   {
      if ( jiffDC <= ticksPerDyna )             // process next jiff
      {
         jiffDC += ticksPerJiff - ticksPerDyna; // reload jiff downcounter

         if ( transDC && (--transDC == 0) )
         {
            // transition phase has arrived
            target->charEv( metroUNCUE );       // uncue the metronome
            recording = recTRANS;               // set recording status
         }
         else
         {
            duration++;                         // bump ongoing duration
            if ( quantized )                    // prior entry quantized up
            {
               duration--;                      //
               quantized = false;
            }
         }

         if ( --beatDC == 0 )                   // next beat has arrived
         {
            beatDC = jiffsPerBeat;              // reload downcounter
            if ( --measureDC == 0 )             // next measure has arrived
            {
               if ( recording == recTRANS )     // true recording phase has arrived
               {
                  recording = recON;            // set recording status
                  duration = 0;                 // reset duration
               }
               measureDC = beatsPerMeasure;
               if ( --endDC == 0 )              // end of sqnc has arrived
                  done();
               else
                  target->charEv( metroTICK );  // downbeat for metronome
            }
            else
               target->charEv( metroTOCK );     // upbeat for metronome
         }
      }
      else                                      // next jiff not yet arrived
         jiffDC -= ticksPerDyna;                // update jiff downcounter
   }
   else
      super::dynamics();
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  RealTimeSequencer::evHandler
 *
 *  Desc:  Handle an onboard event.
 *
 *  Args:  ev               - onboard event
 *
 *  Memb:  beatsPerMeasure  - # beats in one measure
 *        +fullHouse        - if true, sequence buffer is full
 *        +duration         - ongoing duration of note/rest in progress 
 *        +idx              - index of next record in sqnc[] 
 *         jiffDC           - downcounter (in audio ticks) to next jiff
 *         numMeasures      - # measures in sequence
 *        +quantized        - if true, prior entry was quantized to next jiff
 *         recording        - recording status
 *         target           - ptr to instrument being sequenced 
 *         ticksPerHalf     - # ticks per 1/2 jiff
 *        +sqnc[]           - buffer for compiled sequence records
 *
 *  Rets:  status           - true if the event was handled
 *
 *----------------------------------------------------------------------------*/      

bool RealTimeSequencer::evHandler( obEvent ev )
{
   switch ( ev.type() )
   {
      case KEY_DOWN:
      {
         if ( recording < recTRANS ) 
            return super::evHandler( ev );

         // we are either in transition or true recording phase

         if ( fullHouse )           // if sequencer buffer is full, eat key
         {
            informFull();
            return true;            
         }
      
         if ( jiffDC < ticksPerHalf ) // quantize to next Jiff
         {
            duration++;
            quantized = true;
         }

         if ( recording == recON )
         {
            if ( idx > 1 )             // compile duration for prior note/rest
            {
               if ( duration )
                  sqnc[ idx++ ] = duration;
               else                    // cull prior entry on 0 duration
                  --idx;
            }
            else if ( duration > 0 )   // compile a Rest-duration-UnMute sequence
            {
               sqnc[ idx++ ] = tokenRest;  
               sqnc[ idx++ ] = duration;  
               sqnc[ idx++ ] = tokenUnMute;  
            }
         }

         /*  --- Begin the compilation of a new note, space permitting --- */
      
         if ( idx > sqnc[0] - 3 )    // no space for another note + EOS
         {
            fullHouse = true;
            informFull();
            return true;
         }
         else
         {
            //ev.setOctave( octave );    // set octave info in event

            key k = ev.getKey();

            sqnc[ idx++ ] = k.val;     // compile 1st byte of new note record
            duration      = 0;         // set initial duration

            //ev.setOctOn();             // flag as containing octave info

            target->evHandler( ev );   // and play on target instrument
            break;
         }
      }

      case POT1:                       // set Tempo
      {
         // create a tempo that ranges nicely around midTempo

         int relVal = (ev.getPotVal() - 128) * 2;

         if ( relVal > 0 ) 
            relVal *= 4;
         relVal += 1000;

         setTempo( midTempo * (double )relVal/1000.0 );
         break;
      }

      case BUT0_PRESS:                 // play/stop sequencer playback
      
         if ( playing() )
            stop();
         else
            start();
         break;

      case BUT0_TPRESS:                // set beats and measures

         if ( ! recording && ! playing() )
         {
            onLED( 0 );
            beatMenu.getBeats( beatsPerMeasure );
            setBeats( beatMenu.numBeats );
            measureMenu.getMeasures( numMeasures );
            setMeasures( measureMenu.numMeasures );
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
 *  Name:  RealTimeSequencer::setBeats
 *
 *  Desc:  Set the number of beats per measure
 *
 *  Args:  bpm              - beats per measure
 *
 *  Memb: +beatsPerMeasure  - # beats in one measure
 *        +jiffsToTrans     - # jiffs from start of cueing to transition phase
 *
 *  Note: The "transition phase" is a short period of time at the end of cueing 
 *        when any played notes are considered to be "on the 1st beat" of the
 *        1st measure of the recorded sequence.
 *
 *----------------------------------------------------------------------------*/      

void RealTimeSequencer::setBeats( byte bpm )
{
   if ( bpm > maxBeats )
      bpm = maxBeats;
   else if ( ! bpm )
      bpm = 1;

   beatsPerMeasure = bpm;
   jiffsToTrans    = (jiffsPerBeat * beatsPerMeasure) - 1;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  RealTimeSequencer::setMeasures
 *
 *  Desc:  Set the number of measures for the sequence.
 *
 *  Args:  measures         - # of measures in sequence
 *
 *  Memb: +numMeasures      - # of measures in sequence
 *
 *----------------------------------------------------------------------------*/      

void RealTimeSequencer::setMeasures( byte measures )
{
   if ( measures > maxMeasures )
      measures = maxMeasures;
   else if ( ! measures )
      measures = 1;
   numMeasures = measures;
}

void RealTimeSequencer::informFull()
{
   console.romprint( CONSTR("full!") );
   console.newprompt();
}

/******************************************************************************
 *
 *                               DrumSynth 
 *
 ******************************************************************************/

class DrumSynth : public TwoVoxSynth
{
   typedef TwoVoxSynth super;       // superclass is OneVoxSynth

   bool metronomeOn;                // if true, metronome is playing
   bool cueing;                     // if true, metronome is cueing

   public:

   DrumSynth()
   {
      useSequencer( new RealTimeSequencer( 120 ) );
      presets.load( myPresets );
   }

   bool   charEv( char code );      // handle a character event
   bool   evHandler( obEvent );     // handle an onboard event
   Osc   *newOsc( byte nth );       // create oscillator for nth voice
   Voice *newVox( byte nth );       // create nth voice 
   void   noteOn( key );            // turn a note on
   void   output( char*, char* );   // write stereo output to pair of audio buffers

} ;

/*----------------------------------------------------------------------------*
 *
 *  Name:  DrumSynth::charEv
 *
 *  Desc:  Process a character event.
 *
 *  Args:  code             - character to process
 *
 *  Memb: +cueing           - if true, metronome is cueing
 *        +metronomeOn      - if true, metronome is playing
 *         sqnc             - ptr to sequencer object
 *         vox[1]           - ptr to metronome object
 *
 *  Rets:  status           - true if character was handled
 *
 *----------------------------------------------------------------------------*/      

bool DrumSynth::charEv( char code )      
{
   switch ( code )
   {
      case metroTOCK:                  // metronome upbeat

         ((Metronome *)vox[1])->upbeat();
         break;

      case metroCUE:                   // cue the metronome 

         cueing = true;
         onLED( 1 );

         // fall thru to metroON

      case metroON:                    // turn metronome on

         metronomeOn = true;

         // fall thru to metroTICK

      case metroTICK:                  // metronome downbeat

         ((Metronome *)vox[1])->downbeat();
         break;

      case metroOFF:                   // turn metronome off

         metronomeOn = false;
         cueing = false;
         offLED( 1 );
         break;

         // fall thru to metroUNCUE

      case metroUNCUE:                 // uncue the metronome 

         cueing = false;
         blinkLED( 1 );
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

      case '!':                        // perform a reset

         super::charEv( code );

         metronomeOn = false;
         cueing      = false;
         sqnc->setTargetMuting( false );
         break;

      default:                         
         
         return super::charEv( code );
   }
   return true;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  DrumSynth::evHandler
 *
 *  Desc:  Handle an onboard event.
 *
 *  Args:  ev               - onboard event
 *
 *  Memb:  metronomeOn      - if true, metronome is playing
 *         sqnc             - ptr to sequencer object
 *
 *  Rets:  status           - true if the event was handled
 *
 *----------------------------------------------------------------------------*/      

bool DrumSynth::evHandler( obEvent ev )
{
   switch ( ev.type() )
   {
      case POT1:                       // set Tempo
      case BUT0_PRESS:                 // play/stop sequencer playback
      case BUT0_TPRESS:                // beats/measures

         return sqnc->evHandler(ev);         

      case BUT1_PRESS:                 // record a sequence

         sqnc->record();
         console.runModeWhile( sqnc, &this->metronomeOn );
         break;

      default: 
                      
         return super::evHandler( ev );
   }
   return true;                 
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  DrumSynth::newOsc
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

Osc *DrumSynth::newOsc( byte nth )                
{
   switch( nth )
   {
      case 0: return new SampleOsc();
      case 1: return new WhiteNoise();
   }
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  DrumSynth::newVox
 *
 *  Desc:  Return a pointer to the nth voice to be used by the synth.
 *
 *  Args:  nth              - voice # (0-based)
 *
 *  Rets:  ptrVox           - pointer to voice object.
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

Voice *DrumSynth::newVox( byte nth )              
{
   switch( nth )
   {
      case 0: return new Kit();
      case 1: return new Metronome();
   }
}
   
/*----------------------------------------------------------------------------*
 *
 *  Name:  DrumSynth::noteOn
 *
 *  Desc:  Turn a note on.
 *
 *  Args:  note             - note to turn on  
 *
 *  Memb:  cueing           - if true, metronome is cueing
 *
 *----------------------------------------------------------------------------*/      

void DrumSynth::noteOn( key note )
{
   if ( cueing ) return;         // suppress notes during cueing
   super::noteOn( note );
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  DrumSynth::output
 *
 *  Desc:  Write (stereo) output to a pair of audio buffers.
 *
 *  Args:  bufL             - ptr to left audio buffer  
 *         bufR             - ptr to right audio buffer  
 *
 *  Memb:  metronomeOn      - if true, metronome is playing
 *         vox[0]           - ptr to Kit 
 *         vox[1]           - ptr to metronome 
 *
 *----------------------------------------------------------------------------*/      

void DrumSynth::output( char *bufL, char *bufR )
{
   vox[0]->output( bufL );  

   if ( metronomeOn )  // left channel audio = 1/2 metronome + 1/2 kit
   {
      vox[1]->output( bufR );
      Int reg;
      for ( byte i = 0; i < audioBufSz; i++ )
      {
         reg.val = bufR[i] + bufL[i];
         reg.val >>= 1;
         bufR[i] = reg._.lsb;
      }
   }
   else  // copy kit to right channel
   {
      for ( byte i = 0; i < audioBufSz; i++ )
         bufR[i] = bufL[i];
   }
}

DrumSynth synth;

void setup()
{
   ardutouch_setup( &synth );                   
}

void loop()
{
   ardutouch_loop();                            
}                                             

                                             
