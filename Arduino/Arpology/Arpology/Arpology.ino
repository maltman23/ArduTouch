//
//  Arpology.ino
// 
//  A 4-voice arpeggiating synth, with an auto-improvisation feature.
//
//  How to use:
//
//    1) Press a piano key and Arpolgy will play a 4-voice appegiating pattern
//       based on what note you are pressing.
//
//    2) To shift the keyboard up an octave tap the right button.
//       To shift the keyboard down an octave tap the left button.
//
//    3) Arpology uses two separate U/I "frames." A U/I frame determines 
//       which parameters are controlled by the onboard pots. The two U/I
//       frames for Arpology are:
//
//                1) The Envelope Frame (BLUE LED is on)
//
//                     Top Pot controls envelope attack
//                     Bot Pot controls envelope decay/sustain. 
//
//                2) The Arpeggiator Frame (RED LED is on)
//
//                     Top Pot selects arpeggiation pattern.
//                     Bot Pot controls arpeggiation rate. 
//
//    4) To switch between U/I frames, press the left or right buttons.
//       Pressing the right button selects the Envelope Frame.
//       Pressing the left  button selects the Arpeggiator Frame.
//
//    5) To activate the automatic improvisor double-tap the left button.
//       Arpology now "plays itself." The LED for the current frame will begin
//       flashing, indicating that the auto-improvisor is on. While the auto-
//       improvisor is on, Arpology will not respond to key presses. To turn
//       off the auto-improviser double-tap the left button again.
//      
//    6) There are 10 presets for Arpology. These can be selected by double-
//       tapping the right button and then pressing one of the following keys:
//
//          C  (0)  - turn off all vibrato and panning effects
//          C# (1)  - add vibrato to high voices
//          D  (2)  - use a slow panning effect
//          D# (3)  - use a medium-speed panning effect
//          E  (4)  - use a fast panning effect
//          F  (5)  - play "Ambia", a slow moving improv 
//          F# (6)  - use a major tonality
//          G  (7)  - play "Streaker", a fast moving improv 
//          G# (8)  - use a minor tonality
//          A  (9)  - reset Arpology to its initial state
//
//  Programming Notes:
//
//    1) Arpology is meant to be compiled using the "Stand-alone" runtime model.
//       Otherwise, the LEDs will not operate. This runtime model can be set by
//       uncommenting the "#define __STNDLONE__" statement in Model.h of the 
//       ArduTouch library. 
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

about_program( Arpology, 1.08 )              // specify sketch name & version
set_baud_rate( 115200 )                      // specify serial baud-rate

// Specify whether the voices of Arpology can be panned in stereo by 
// uncommenting the following defines

#define ENABLE_PANNING                       // use a panning synth as superclass
#define DUAL_PANNING                         // and superclass = QuadDualPanSynth 
                                             // (otherwise     = QuadPanSynth)

// The following 2 statements insure that presets will work and be accessible
// from the ArduTouch keyboard

#ifndef INTERN_CONSOLE                       
   #error This sketch requires __STNDLONE__ runtime model or higher (Model.h)
#endif

#ifndef KEYBRD_MENUS
   #error This sketch requires KEYBRD_MENUS to be defined (Model.h)
#endif

/*----------------------------------------------------------------------------*
 *                                 presets
 *----------------------------------------------------------------------------*/      

define_preset( _, "" )                       // blank preset

define_preset( NoEff, "0g0\\V.``"            // no effects
                     "1g0\\V.``"
                     "2g0\\V.``"
                     "3g0\\V.``"
               #ifdef ENABLE_PANNING
                     "P.`"                
                  #ifdef DUAL_PANNING
                     "X.`"                
                  #endif
               #endif
              )

#ifdef ENABLE_PANNING

define_preset( Pan1, "Pf.5\\<`"              // slow pan
               #ifdef DUAL_PANNING
                     "Xf.2\\<`"
               #endif
              )

define_preset( Pan2, "Pf2\\<`"               // medium-speed pan
               #ifdef DUAL_PANNING
                     "Xf1.2\\<`"
               #endif
              )

define_preset( Pan3, "Pf5.6\\<`"             // fast pan
               #ifdef DUAL_PANNING
                     "Xf1.8\\<`"
               #endif
              )

#endif
                                             // vibrato
define_preset( Vib, "2V+<t5\\d.5\\``"
                    "3V+<f4.5\\t10\\d.625\\``"
              )

define_preset( Ambia, "!a120\\d100\\N6\\r33\\U"
                      "2V<-f3.6\\d.375\\t9\\``"
                      "3V<-f4.15\\d.375\\t15\\``"
                  #ifdef ENABLE_PANNING
                      "Pf.7\\d1\\<`"
                    #ifdef DUAL_PANNING
                      "Xf.4\\d1\\<`"
                    #endif
                  #endif
              )

define_preset( Streaker, "!-a9\\d55\\N7\\r213\\U"
                         "2g100\\V<f6\\d.375\\t6\\``"
                  #ifdef ENABLE_PANNING
                         "Pf3\\d1\\<`"
                     #ifdef DUAL_PANNING
                         "Xf1.5\\d1\\<`"
                     #endif
                  #endif
              )

define_preset( Maj, "+" )                    // select Major tonality
define_preset( Min, "-" )                    // select Minor tonality
define_preset( Reset, "!" )                  // reset synth


begin_bank( myPresets )                      // these presets will be loaded

   _preset( NoEff )
   _preset( Vib )

#ifdef ENABLE_PANNING
   _preset( Pan1 )
   _preset( Pan2 )
   _preset( Pan3 )
#else
   _preset( _ )
   _preset( _ )
   _preset( _ )
#endif

   _preset( Ambia )
   _preset( Maj )
   _preset( Streaker )
   _preset( Min )
   _preset( Reset )

end_bank()

/******************************************************************************
 *
 *                                 Tonality 
 *
 ******************************************************************************/

typedef const char Thirds[2];          // element 0 is interval between 3rd and root
                                       // element 1 is interval between 5th and root
class Tonality
{
   protected:

   // thirds pts to an array of Thirds in ROM (one entry per note in octave)
   // and defines the tonality.

   Thirds *thirds;                     // ptr to array of Thirds in ROM

   // buffer is used for constructing a 4-part voicing in RAM

   char buffer[4];                     // voicing buffer

   public:

   const char* name;                   // ptr to name of Tonality in ROM

   /*-------------------------------------------------------------------------*
   *
   *  Name:  Tonality::getVoicing
   *
   *  Desc:  Construct a 4-part voicing, given a note in the octave.
   *
   *  Args:  ith              - note in octave (0 is C, 1 is C#, etc.)
   *
   *  Rets:  &voicing         - address of constructed voicing
   *
   *  Memb: +buffer           - voicing buffer
   *         thirds           - ptr to array of Thirds in ROM
   *
   *  Note:  The constructed voicing will be:
   *
   *            root 
   *            5th  1 octave up
   *            3rd  2 octaves up
   *            root 3 octaves up
   *
   *-------------------------------------------------------------------------*/      

   virtual char* getVoicing( byte ith )
   {  
      Thirds  *ptrThird  = thirds;
               ptrThird += ith;
      Thirds  &ithThird  = *ptrThird;

      buffer[0] = 0;
      buffer[1] = 12 + pgm_read_byte_near( ithThird+1 );
      buffer[2] = 24 + pgm_read_byte_near( ithThird );
      buffer[3] = 36;

      return this->buffer;
   }

} ;

/******************************************************************************
 *
 *                              MajorTonality 
 *
 ******************************************************************************/

// majorThirds[] enumerates 3rd/5th pairs for each note in octave

PROGMEM Thirds majorThirds[12] = 
{ 
   { 4, 7 },
   { 4, 7 },
   { 3, 7 },
   { 3, 6 },
   { 3, 8 },
   { 4, 7 },
   { 3, 8 },
   { 4, 7 },
   { 3, 8 },
   { 3, 7 },
   { 3, 6 },
   { 3, 8 }
} ;

class MajorTonality : public Tonality
{
   public:

   MajorTonality()
   {
      name   = CONSTR( "Major" );
      thirds = &majorThirds[0];
   }

} ;

/******************************************************************************
 *
 *                              MinorTonality 
 *
 ******************************************************************************/

// minorThirds[] enumerates 3rd/5th pairs for each note in octave 

PROGMEM Thirds minorThirds[12] = 
{ 
   { 3, 7 },
   { 3, 8 },
   { 3, 7 },
   { 3, 7 },
   { 3, 8 },
   { 3, 7 },
   { 3, 8 },
   { 4, 7 },
   { 4, 7 },
   { 3, 7 },
   { 3, 7 },
   { 3, 8 }
} ;

class MinorTonality : public Tonality
{
   public:

   MinorTonality()
   {
      name   = CONSTR( "Minor" );
      thirds = &minorThirds[0];
   }

} ;

/******************************************************************************
 *
 *                               Improvisation  
 *
 ******************************************************************************/

#define LEN_HISTORY 3                  // length of improvisation history

class Improvisation
{
   protected:

   key history[ LEN_HISTORY ];

   void commitNote( key note )         // commit a note for output
   {
      // update history

      for ( byte i = LEN_HISTORY-1; i > 0; i-- )
         history[i] = history[i-1];
      history[0]  = curNote;

      curNote     = note;
      curDuration = computeDuration();
   }

   virtual byte computeDuration()      // compute duration of output note
   {
      return 1;
   }

   public:

   key   curNote;                      // current output note
   byte  curDuration;                  // duration of current note

   byte  maxOctave;                    // max octave for improvised notes
   byte  minOctave;                    // min octave for improvised notes

   Improvisation()
   {
      minOctave = 0;
      maxOctave = 7;

      for ( byte i = 0; i < LEN_HISTORY; i++ )
         history[i] = key( 15, 15 );   // impossible note
   }

   virtual void   start() {};          // start an improv
   virtual void   start( key ) {};     // start an improv with given note
   virtual void   nextNote() {};       // generate a new curNote/Duration
} ;

/******************************************************************************
 *
 *                          VoiceFollowingImprov  
 *
 ******************************************************************************/

Rand16 rand16;                         // random number generator

typedef const char FollowingVoices[8]; // possible next notes (offset from C)

class VoiceFollowingImprov : public Improvisation
{
   protected:

   FollowingVoices *followingVoices;

   boolean  cullRepeatPairs;  // do not allow sequential repetition of note pairs

   public:

   void start( key startNote )         // start an improv
   {
      commitNote( startNote );
      cullRepeatPairs = true;
   }

   byte computeDuration()              // compute duration of output note
   {
      rand16.next();
      byte doubleDur = (rand16.output._.lsb & 0b00000111) + 1;

      byte odds = 0;  // odds (out of 8) for double duration

      switch( curNote.position() )
      {
         case 0:  odds = 4;  break;

         case 4:
         case 7:  odds = 3;  break;

         case 2:
         case 5:
         case 9:  odds = 2;  break;

         case 11: odds = 1;  break;

         default: odds = 0;
      }
      return ( odds < doubleDur ? 1 : 2 );
   }

   void nextNote()                     // generate a new curNote/Duration
   {
      #define MAX_RETRYS 12

      key newNote = randNote();

      if ( cullRepeatPairs )
      {
         if ( newNote == history[0] && curNote == history[1] )
         {
            key badNote    = newNote;
            byte numRetrys = 0;
            while ( newNote == badNote && numRetrys < MAX_RETRYS )
            {
               newNote = randNote();
               ++numRetrys;
            }
         }
      }

      commitNote( newNote );     

      #undef MAX_RETRYS
   }       

   key randNote()                      // generate a random note
   {
      FollowingVoices *ptr  = followingVoices;
                       ptr += curNote.position();
      FollowingVoices &fv   = *ptr;

      // generate a random index from 0 to 7

      rand16.next();                   
      byte randIdx = rand16.output._.lsb & 0b00000111;

      // select a note position from followingVoices[ curNote.position() ]

      char notePos = (char )pgm_read_byte_near( fv + randIdx );

      // adjust for any implicit octave offset

      char octave = curNote.octave();
      if ( notePos > 11 )
      {
         ++octave;
         if ( octave > maxOctave )
            octave = maxOctave-1;
         notePos -= 12;
      }
      else if ( notePos < 0 )
      {
         --octave;
         if ( octave < minOctave )
            octave = minOctave+1;
         notePos += 12;
      }

      return key( notePos, octave );
   }

   void setFollowingVoices( FollowingVoices *follows )
   {
      followingVoices = follows;
   }

} ;

/******************************************************************************
 *
 *                               ArpImprov  
 *
 ******************************************************************************/

PROGMEM FollowingVoices majorFollows[12] =        // following voices: major tonality
{ 
   { 9-12, 3, 4, 4, 4, 5, 7, 7 },                 // for C
   { 11-12, 11-12, 11-12, 11-12, 0, 0, 0, 4 },    // for Db
   { 11-12, 11-12, 1, 3, 5, 5, 5, 7 },            // for D
   { 11-12, 11-12, 11-12, 2, 4, 4, 4, 7 },        // for Eb
   { 0, 3, 5, 5, 7, 8, 9, 11 },                   // for E
   { 0, 0, 4, 6, 6, 7, 7, 9 },                    // for F
   { 11-12, 4, 5, 5, 7, 7, 7, 7 },                // for Gb
   { 0, 3, 5, 5, 5, 6, 8, 0+12 },                 // for G
   { 3, 4, 4, 6, 7, 9, 9, 9 },                    // for Ab
   { 5, 8, 8, 8, 10, 10, 11, 2+12 },              // for A
   { 4, 7, 7, 9, 11, 11, 11, 2+12 },              // for Bb
   { 4, 7, 7, 7, 0+12, 0+12, 0+12, 4+12 }         // for B
} ;

PROGMEM FollowingVoices minorFollows[12] =        // following voices: minor tonality
{ 
   { 8-12, 3, 4, 4, 4, 5, 7, 7 },                 // for C
   { 6-12, 11-12, 11-12, 2, 2, 2, 4, 6 },         // for Db
   { 11-12, 11-12, 1, 4, 4, 4, 7, 7 },            // for D
   { 11-12, 11-12, 11-12, 0, 4, 4, 6, 6 },        // for Eb
   { 11-12, 1, 1, 5, 5, 7, 9, 11 },               // for E
   { 0, 0, 4, 6, 6, 7, 7, 8 },                    // for F
   { 11-12, 1, 1, 4, 7, 7, 7, 7 },                // for Gb
   { 4, 4, 6, 6, 6, 11, 0+12, 0+12 },             // for G
   { 5, 5, 5, 6, 7, 11, 12, 12 },                 // for Ab
   { 4, 4, 6, 6, 11, 11, 2+12, 2+12 },            // for A
   { 4, 7, 7, 8, 11, 11, 11, 3+12 },              // for Bb
   { 6, 7, 7, 7, 0+12, 0+12, 0+12, 4+12 }         // for B
} ;

class ArpImprov : public VoiceFollowingImprov
{
   public:

   ArpImprov()
   {
      setMajor();
   }

   void nextNote()                     // generate a new curNote/Duration
   {
      // occasionally shift up/down one octave 

      byte odds = 0;                   // odds (out of 255) for octave down

      switch( curNote.position() )
      {
         case 0:  odds = 26;  break;

         case 5:
         case 7:  odds = 13;  break;

         default: odds = 0;
      }

      rand16.next();
      if ( rand16.output._.lsb < odds )
      {
         byte curOct = curNote.octave();
         commitNote( key ( curNote.position(), 
                           curOct <= minOctave+1 ? curOct+1 : curOct-1 ) );
      }
      else
         VoiceFollowingImprov::nextNote();
   }

   void setMajor()
   {
      setFollowingVoices( &majorFollows[0] );
   }

   void setMinor()
   {
      setFollowingVoices( &minorFollows[0] );
   }

} ;

/******************************************************************************
 *
 *                                PeriodicCue 
 *
 ******************************************************************************/

class PeriodicCue : public LFO
{
   boolean issued;                  // a new cue has been issued

   public:

   boolean handled;                 // cue has been handled

   boolean charEv( char code )      // handle a character event
   {
      switch( code )
      {

         case '!':                  // reset

            LFO::charEv('!');
            issued   = false;
            handled = false;
            setDepth( 1.0 );
            break;

         default:

            return LFO::charEv( code );
      }
      return true;
   }

   void dynamics()                  // update object dynamics
   {
      if ( muted() )
         return;

      LFO::dynamics();

      if ( pos > .5 )
      {
         if ( ! handled )
            issued = true;
      }
      else
      {
         if ( handled )
         {
            issued = false;
            handled = false;
         }
      }
   }

   boolean pending()                // returns true if there is an unhandled cue 
   {
      return ( issued && ! handled );
   }


} ;

/******************************************************************************
 *       
 *                         Arpeggiation Patterns 
 *
 *  Arpeggiation patterns are byte strings. The 1st element specifies the 
 *  number of entries in the pattern. The remaining elements are voice
 *  ordinals (0 through 3) denoting one of the four parts in a voicing.
 *  Each ordinal must be specified at least once in the pattern, otherwise
 *  the voice correponding to the unspecified ordinal will keep sustaining
 *  from the prior pattern that was played.
 *
 ******************************************************************************/

#define NUM_PATTERNS 9

PROGMEM const byte pattern1[] = { 4,  0, 1, 2, 3 };
PROGMEM const byte pattern2[] = { 4,  0, 2, 1, 3 };
PROGMEM const byte pattern3[] = { 4,  3, 2, 1, 0 };
PROGMEM const byte pattern4[] = { 8,  0, 1, 2, 0, 2, 1, 2, 3 };
PROGMEM const byte pattern5[] = { 8,  0, 2, 3, 1, 2, 1, 2, 3 };
PROGMEM const byte pattern6[] = { 6,  0, 1, 2, 1, 2, 3 };
PROGMEM const byte pattern7[] = { 6,  0, 1, 0, 1, 2, 3 };
PROGMEM const byte pattern8[] = { 6,  0, 2, 1, 3, 1, 2 };
PROGMEM const byte pattern9[] = { 9,  0, 2, 3, 2, 1, 2, 3, 1, 2 };

PROGMEM const byte* const patterns[] = 
{
   NULL,                            // 1st element always NULL
   &pattern1[0],
   &pattern2[0],
   &pattern3[0],
   &pattern4[0],
   &pattern5[0],
   &pattern6[0],
   &pattern7[0],
   &pattern8[0],
   &pattern9[0]
} ;


/******************************************************************************
 *       
 *                                ArpSynth 
 *
 ******************************************************************************/

#ifdef ENABLE_PANNING
 #ifdef DUAL_PANNING
   #define ARP_BASE_CLASS  QuadDualPanSynth  // use QuadDualPanSynth as superclass
 #else
   #define ARP_BASE_CLASS  QuadPanSynth      // use QuadPanSynth as superclass
 #endif
#else
   #define ARP_BASE_CLASS  QuadSynth         // use QuadSynth as superclass
#endif

class ArpSynth : public ARP_BASE_CLASS
{
   typedef ARP_BASE_CLASS super;             // superclass is ARP_BASE_CLASS

   Tonality *tonality;                       // current tonality for synth
   
   Tonality *majorTonality;
   Tonality *minorTonality;

   ArpImprov  *improviser;                   // ptr to Improvisation object
   PeriodicCue   arpegCue;                   // cue for next arpegiating note

   byte     arpegRate;                       // (U/I number: 0-255)

   const byte *pattern;                      // ptr to arpeggiation pattern
   byte     arpegIdx;                        // current index into pattern

   byte     curPatNum;                       // pattern # being played
   byte     pendingPatNum;                   // pattern # to be loaded next

   boolean  autoPilot;                       // if true, autopilot is on

   byte     durationDC;                      // beat downcounter to next autoPilot note

   byte     patsPerBeat;                     // # patterns per beat

   boolean  blankSlate;                      // if true, a note has yet to be played

   typedef byte Frame ; enum                 // user-interface frame
   { 
      ARPEG_FRAME,      // arpeggiator frame
      ENV_FRAME         // envelope frame
   } ;

   Frame curFrame;

   public:

   void setup() 
   { 
      super::setup();

      flags |= RSTMUTE;

      improviser = new ArpImprov();

      // constrain keyboard to a top octave of 3

      improviser->maxOctave = 3;
      keybrd.setTopOct( 3 );
      keybrd.setDefOct( 2 );        

      // allocate tonalities 

      majorTonality = new MajorTonality();
      minorTonality = new MinorTonality();

      presets.load( myPresets );       // load bank of presets
   }

   boolean charEv( char code )         // handle a character event
   {
      switch( code )
      {
         #ifdef INTERN_CONSOLE         // compile cases needed by macros

         case 'A':                     // print info on ArduTouch library

            ardutouch_info();
            break;

         case 'a':                     // set attack (for all voices)
         {
            byte attack;
            if ( console.getByte( CONSTR("attack"), &attack ) )
               setAttack( attack );
            break;
         }

         case 'd':                     // set post-attack (for all voices)
         {
            byte postAttack;
            if ( console.getByte( CONSTR("decay"), &postAttack ) )
               setPostAttack( postAttack );
            break;
         }

         case 'N':                     // get pattern number
         
            console.getByte( CONSTR("pattern"), &pendingPatNum );
            break;   
      
         case 'r':                     // set arpeggiation rate
         {
            byte rate;
            if ( console.getByte( CONSTR("rate"), &rate ) )
               setRate( rate );
            break;
         }

         case 'U':                     // enable autoPilot

            enableAutoPilot( true );
            break;

         case 'u':                     // disable autoPilot

            enableAutoPilot( false );
            break;

         case '+':

            setMajorTonality();
            break;

         case '-':

            setMinorTonality();
            break;

         #endif

         case '!':                     // reset

            for ( byte i = 0; i < numVox; i++ )
               vox[i]->setFreq( 0.0 );

            super::charEv( code );

            pendingPatNum = 1;
            arpegCue.reset();

            setAttack( 39 );
            setPostAttack( 33 );
            setRelease( 20 );
            #ifdef ENABLE_PANNING
               #ifdef DUAL_PANNING
                  setXPanPos( 0 );
               #endif
               setPanPos( 80 );
            #endif
            setRate( 67 );                // arpeggiation tempo ~= 3.80

            selectFrame( ENV_FRAME );

            curPatNum  = 0;
            enableAutoPilot( false );
            blankSlate = true;

            lastNote.set( 0, 2 );         // initial (unplayed) note of C2

            setMajorTonality();
            refreshLEDs();

            break;

         #ifdef CONSOLE_OUTPUT         // compile cases that display to console 

         case chrInfo:
         {
            super::charEv( chrInfo );

            console.newlntab();
            console.infoByte( CONSTR("attack"), vox[0]->envAmp.getAttack() ); 
            console.infoByte( CONSTR("decay"),  vox[0]->envAmp.getDecay() ); 

            console.newlntab();
            console.infoByte( CONSTR("patterN"), curPatNum ); 
            console.infoByte( CONSTR("rate"),  arpegRate ); 
            console.newlntab();
            console.infoStr( CONSTR("tonality"), tonality->name ); 
            console.infoStr( CONSTR("aUtopilot"), autoPilot ? CONSTR("On")
                                                            : CONSTR("Off") ); 
            /* the following useful for monitoring:

            console.newlntab();
            console.infoByte( CONSTR("panPos"), panPos ); 

            */

            break;
         }
         #endif

         default:
            return super::charEv( code );
      }
      return true;
   }

   void dynamics()                           // perform a dynamic update
   {
      super::dynamics();

      arpegCue.dynamics();
      if ( arpegCue.pending() && ! deferVoc )
      {
         if ( pendingPatNum > 0 && arpegIdx == 1 ) 
         {
            if ( pendingPatNum <= NUM_PATTERNS )
            {
               curPatNum = pendingPatNum;

               // set pattern to address of arpeggiation pattern in ROM

               Word patAddr;
               word idxPats  = (word )patterns + ( curPatNum << 1 );
               patAddr._.lsb = pgm_read_byte_near( idxPats++ );
               patAddr._.msb = pgm_read_byte_near( idxPats );
               pattern       = (const byte*) patAddr.val;

               arpegIdx      = 1;

               // compute patsPerBeat
               
               rand16.next();
               byte rand0to3 = (rand16.output._.lsb & 0b00000011);

               if ( patLen() <= 6 )
               {
                  switch( rand0to3 )
                  {
                     case 0:  patsPerBeat = 1; break;
                     case 3:  patsPerBeat = 3; break;
                     default: patsPerBeat = 2;
                  }
               }
               else
               {
                  switch( rand0to3 )
                  {
                     case 0:  
                     case 1:  patsPerBeat = 1; break;
                     default: patsPerBeat = 2;
                  }
               }

            }
            pendingPatNum = 0;
         }

         if ( autoPilot && arpegIdx == 1 && --durationDC == 0 )
         {
            noteOn( improviser->curNote );
            durationDC = improviser->curDuration * patsPerBeat;
            improviser->nextNote();
         }
         
         vox[ pgm_read_byte_near( pattern+arpegIdx ) ]->noteOn( lastNote );
         if ( ++arpegIdx > patLen() )
            arpegIdx = 1;
         
         arpegCue.handled = true;
      }

   }

   void enableAutoPilot( boolean enable )
   {
      autoPilot = enable;
      keybrd.setMute( enable );               // disable keyboard when enabled
      if ( autoPilot )
      {
         rand16.reseed();
         improviser->start( lastNote );
         if ( blankSlate ) 
            kickStart();
         else
            pendingPatNum = curPatNum;       // force recomputation of patsPerBeat
         durationDC = 1;
      }
      refreshLEDs();
   }

   boolean evHandler( obEvent e )            // event handler
   {
      switch ( e.type() )                    // branch on event type
      {
         case KEY_UP:                        // key has been released

            return true;                     // all keys play through to sustain

         case POT0:                          
         
            switch ( curFrame )
            {
               case   ENV_FRAME:  setAttack( e.getPotVal() >> 1 ); break;
               case ARPEG_FRAME:
               {
                  // select an arpeggiation sequence

                  const double scalar = (double )NUM_PATTERNS / 255.0;

                  byte potVal = e.getPotVal();
                  if ( potVal == 255 ) 
                     potVal = 254;

                  double mapVal = 1.00 + scalar * potVal;
                  pendingPatNum = (byte) mapVal;

                  break;
               }
            } 
            break;

         case POT1:                          
         
            switch ( curFrame )
            {
               case   ENV_FRAME:  setPostAttack( e.getPotVal() >> 1 ); break;
               case ARPEG_FRAME:  setRate( e.getPotVal() );            break;
            } 
            break;
         
         case BUT0_PRESS:                    // select arpeggiator frame

            selectFrame( ARPEG_FRAME );
            break;

         case BUT1_PRESS:                    // select enveloping frame

            selectFrame( ENV_FRAME );
            break;

         case BUT0_DTAP:                     // toggle autopilot on/off

            enableAutoPilot( ! autoPilot );
            break;

         case BUT1_DTAP:                     // override "one-shot menu" 

            presets.choose();                // choose a preset
            break;

         default:                            // pass on unhandled events
                                             
            return super::evHandler(e);
      }
      return true;                     
   }

   void kickStart()
   {
      setMute( false );
      arpegCue.setMute( false );
      noteOff(0);                            
      arpegIdx = 1;
      blankSlate = false;
   }

   Osc *newOsc( byte nth )
   {
      FastWaveOsc *o = new FastWaveOsc();
      o->setTable( wavetable_named( Ether ) ); 
      return o;
   }

   void noteOn( key newNote )                // play a note
   {
      lastNote = newNote;
      setVoicing( tonality->getVoicing( newNote.position() ) );
      if ( blankSlate )  
         kickStart();

      /* following lines useful for monitoring from console:

      console.print('{');
      console.print( (int )newNote.octave() );
      console.print( ", " );
      console.print( (int )newNote.position() );
      console.print("} ");

      */
   }

   byte patLen()
   {
      return pgm_read_byte_near( pattern );
   }

   void refreshLEDs()
   {
      switch( curFrame )
      {
         case ARPEG_FRAME:

            offLED( BLUE_LED );
            if ( autoPilot )
               blinkLED( RED_LED );
            else
               onLED( RED_LED );
            break;

         case ENV_FRAME:

            offLED( RED_LED );
            if ( autoPilot )
               blinkLED( BLUE_LED );
            else
               onLED( BLUE_LED );
            break;
      }
   }

   void selectFrame( Frame frame )
   {
      switch( frame )
      {
         case ARPEG_FRAME:
         case ENV_FRAME:
            break;
         default:
            return;
      }
      
      curFrame = frame;
      refreshLEDs();
   }

   void setPostAttack( byte postAttack )
   {
      setDecay( postAttack );
      setSustain( postAttack );
   }

   void setRate( byte rate )                 // set arpeggiation rate
   {
      const byte   xLowMax   =  63;
      const byte   xSweetMax = 191;
      const byte   xHighMax  = 255;

      const double yLowMin   =  0.25;
      const double ySweetMin =  3.8;
      const double ySweetMax =  8.0;
      const double yHighMax  = 15.0;

      byte   xMin, xMax;
      double yMin, yMax;

      arpegRate = rate;

      // set rate via 3 logarithmic zones

      if ( rate <= xLowMax )
      {
         xMin = 0;
         xMax = xLowMax;
         yMin = yLowMin;
         yMax = ySweetMin;
      }
      else if ( rate <= xSweetMax )
      {
         xMin = xLowMax+1;
         xMax = xSweetMax;
         yMin = ySweetMin;
         yMax = ySweetMax;
      }
      else
      {
         xMin = xSweetMax+1;
         xMax = 255;
         yMin = ySweetMax;
         yMax = yHighMax;
      }

      byte x       = rate - xMin;
      byte xRange  = xMax - xMin;

      double xSquare = x * x;
      double rSquare = xRange * xRange;
 
      double newFreq = yMin + ( yMax - yMin ) * ( xSquare / rSquare );

      arpegCue.setFreq( newFreq );

      /* following lines useful for monitoring from console:
      
         console.newlntab();
         console.infoByte( CONSTR("rate"), rate );
         console.infoDouble( CONSTR("freq"), newFreq );

      */

   }

   void setMajorTonality()
   {
      setTonality( majorTonality );
      improviser->setMajor();
   }

   void setMinorTonality()
   {
      setTonality( minorTonality );
      improviser->setMinor();
   }

   void setTonality( Tonality *tonality )    // set tonality for voicings
   {
      this->tonality = tonality;
      setVoicing( tonality->getVoicing( lastNote.position() ) );
   }

} arpSynth;                                     

void setup()
{
   ardutouch_setup( &arpSynth );             // initialize ardutouch resources
}

void loop()
{
   ardutouch_loop();                         // perform ongoing ardutouch tasks   
}                                             



