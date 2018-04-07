/*
    Sequencer.h  

    Declaration of Sequencer classes and symbolic constants.

    ---------------------------------------------------------------------------
 
    Copyright (C) 2016, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
   ---------------------------------------------------------------------------
*/

#ifndef SEQUENCER_H_INCLUDED
#define SEQUENCER_H_INCLUDED

#include "Control.h"
#include "Instrument.h"

/******************************************************************************
 *
 *                               Jiffs  
 *
 *  A jiff ("in a jiffy") is the smallest unit of time kept by the 
 *  sequencer. Beats are denominated in jiffs, as are duration values
 *  for the note and rest records that make up a sequence.
 *
 *  When recording using the RealTimeSequencer, events are quantized to
 *  the nearest jiff.
 *
 *  Note that a jiff is not an absolute measure of time, but is dependent
 *  on the tempo.
 *  
 ******************************************************************************/

#define  jiffsPerBeat 24            // # of jiffs per beat

/******************************************************************************
 *
 *                          The Sequence Buffer
 *
 *  Sequences are compiled in byte buffers which can be up to 255 bytes in 
 *  length. The first byte of a sequence buffer contains the buffer size, 
 *  the remaining bytes are available for compiling records. 
 *
 *  For example, if we are using a 6 byte array sqnc[] for our sequence
 *  buffer then:
 *
 *    sqnc[0] = 6  (buffer size)
 *    sqnc[1] \
 *    sqnc[2]  \
 *    sqnc[3]    (these bytes can contain records)
 *    sqnc[4]  /
 *    sqnc[5] /
 *
 *  There are 4 types of records in a sequence:
 *
 *          NoteOn
 *          NoteOff 
 *          Rest 
 *          EOS      "End-of-Sequence"
 *
 *                      ------  EOS record  -----
 *
 *  The EOS record is one byte long and has a unique value ("tokenEOS",
 *  defined below). It marks the end of the sequence.
 *
 *  For example, using our 6 byte buffer, a null sequence (containing no 
 *  notes or rests), would look like this:
 *  
 *    sqnc[0] = 6          
 *    sqnc[1] = tokenEOS
 *    sqnc[2] ... 
 *    sqnc[3] ... (random garbage)
 *    sqnc[4] ...   
 *    sqnc[5] ...
 *
 *  Notice that a sequence does not necessarily fill its whole buffer.
 *
 *                     ------  NoteOn record  -----
 *
 *  The NoteOn record is 2 bytes long. The 1st byte contains the note ID,
 *  The 2nd byte contains the duration, in jiffs.
 *
 *  The Note ID byte for a NoteOn record is comprised of 3 bit fields:
 *
 *          bit #  7  654  3210
 *                [0][oct][key#]
 *
 *  bit  7    is always 0 and indicates that the Note is being turned On
 *  bits 4:6  contain the octave # of the note (0 to 7)
 *  bits 0:3  contain the key # within the octave (0 to 12: 0 = C, 1 = C#, 
 *            2 = D, ... , 11 = B, 12 = high C)
 *
 *  For example, using our 6 byte buffer, a sequence containing a single
 *  NoteOn for a D in octave 1, with a duration of 40 jiffs would look 
 *  like this:
 *  
 *    sqnc[0] = 6          
 *    sqnc[1] = 00010010b = 34
 *    sqnc[2] = 40 
 *    sqnc[3] = tokenEOS
 *    sqnc[4] ...   
 *    sqnc[5] ...
 *
 *                     ------  NoteOff record  -----
 *
 *  The NoteOff record is identical to the NoteOn record, except that bit 7
 *  of the Note ID byte is set:
 *
 *          bit #  7  654  3210
 *                [1][oct][key#]
 *
 *
 *                     ------  Rest record  -----
 *
 *  The Rest record is 2 bytes long. The 1st byte contains the unique value
 *  "tokenRest", defined below). The 2nd byte contains the duration, in jiffs.
 *
 *
 *                    -- Records with long durations --
 *
 *  NoteOn, NoteOff, and Rest records with durations that exceed 254 jiffs
 *  can be arbitrarily extended by concatenating Rest records to them.
 *
 *  
 ******************************************************************************/

/* sequence record tokens */

#define tokenEOS     255            // end-of-sequence token
#define tokenRest    254            // rest token (# jiffs follows in next byte)

/******************************************************************************
 *
 *                       Symbolic Note Durations
 *
 *  The following defines provide a comvenient way to specify durations
 *  in hand-coded sequences.
 *
 ******************************************************************************/

   // durations in simple time:

#define  WHOLE_   jiffsPerBeat*4    // whole note
#define  HALF_    jiffsPerBeat*2    // half note
#define  QUART_   jiffsPerBeat      // quarter note
#define  EIGHT_   jiffsPerBeat/2    // eight note
#define  SIXT_    jiffsPerBeat/4    // sixteenth note
#define  WHOLE__  WHOLE_ + HALF_    // dotted whole note
#define  HALF__   HALF_  + QUART_   // dotted half note
#define  QUART__  QUART_ + EIGHT_   // dotted quarter note
#define  EIGHT__  EIGHT_ + SIXT_    // dotted eight note

   // durations in compound time:

#define  TRIPL_   jiffsPerBeat/3    // "triplet" -- an 8th note in compound time

/******************************************************************************
 *
 *                                 Sequencer
 *
 ******************************************************************************/

class Sequencer : public Mode    
{
   typedef Mode super;           // superclass is Mode

   public:
   
   Instrument *target;           // ptr to instrument being sequenced 

   static Sequencer *sender;     // ptr to Sequencer object sending message 

   bool   charEv( char );        // process a character event
   double getTempo();            // get sequencer playback tempo 
   byte   length();              // returns length of compiled sequence
   void   record();              // record a new sequence
   void   rewind();              // rewind sequence to first record
   void   setTempo( double );    // set sequencer playback tempo 
   void   start();               // start sequencer playback
   void   stop();                // stop sequencer playback 
   void   _pause_();             // toggle pausing of sequencer playback

   virtual void  dynamics();     // perform a dynamic update 

   virtual bool load(const byte*)=0;  // load a sequence from ROM

   #ifdef CONSOLE_OUTPUT
   const char *prompt();
   #endif

   bool   playing()              // returns true if sequencer is playing
   { 
      return ( playback == pbON ); 
   }

   protected:

   static  const byte pbOFF    = 0;  // playback status values
   static  const byte pbON     = 1; 
   static  const byte pbPAUSED = 2; 
   
   byte    playback;             // playback status

   word    ticksPerJiff;         // # ticks per jiff
   word    ticksPerHalf;         // # ticks per 1/2 jiff
   double  tempo;                // sequencer tempo (beats per minute)

   word    jiffDC;               // downcounter (in audio ticks) to next jiff
   byte    exeDC;                // downcounter (in jiffs) to next record execution

   // sqnc will be declared in SequencerRAM as byte*
   // sqnc will be declared in SequencerROM as const byte*

   byte    idx;                  // current index for sqnc[] 

   virtual byte at(byte ith)=0;  // fetch byte at sqnc[ith]
   byte    atIdxBump();          // fetch byte at sqnc[idx], then bump idx
   void    exeRec();             // execute next record in sqnc[]

   void    msgAll( char msg );   // message the synth and the target instrument
   void    msgSynth( char msg ); // message the synth

} ;


/******************************************************************************
 *
 *                             SequencerRAM
 *
 ******************************************************************************/

class SequencerRAM : public Sequencer    
{
   typedef Sequencer super;       // superclass is Sequencer

   public:

   SequencerRAM( byte bufSz = 132 )
   {
      sqnc = (byte *)malloc( bufSz );
      sqnc[0] = bufSz;
   }

   byte   at( byte );            // fetch byte at sqnc[ith]
   bool   charEv( char );        // process a character event
   bool   load( const byte* );   // load a sequence from ROM
   bool   read( word );          // read a sequence from EEPROM
   void   write( word );         // write sequence to EEPROM

   protected:

   byte    *sqnc;                // buffer for sequence records

} ;

/******************************************************************************
 *
 *                             SequencerROM
 *
 ******************************************************************************/

class SequencerROM : public Sequencer    
{
   typedef Sequencer super;       // superclass is Sequencer

   public:

   byte   at( byte );            // fetch byte at sqnc[ith]
   bool   charEv( char );        // process a character event
   bool   load( const byte* );   // load a sequence from ROM

   protected:

   const byte *sqnc;             // ptr to sequence records in ROM

} ;

/******************************************************************************
 *
 *                           RealTimeSequencer
 *
 ******************************************************************************/

class RealTimeSequencer : public SequencerRAM
{
   typedef Sequencer super;            // superclass is Sequencer

   using SequencerRAM::SequencerRAM;   // use SequencerRAM constructor

   protected:

   // recording phases:

   static  const byte recOFF   = 0;    // not recording 
   static  const byte recCUE   = 2;    // cueing before recording
   static  const byte recTRANS = 3;    // transitioning from cueing to recording
   static  const byte recON    = 4;    // recording

   byte  recPhase;                     // current phase in the recording process

   byte  beatDC;                       // downcounter (in jiffs) to next beat
   byte  transDC;                      // downcounter (in jiffs) to transition phase
   byte  measureDC;                    // downcounter (in beats) to next measure
   byte  endDC;                        // downcounter (in measures) to end of recording

   byte  jiffsToTrans;                 // # jiffs from start of cueing to transition phase
   byte  beatsPerMeasure;              // # beats in one measure
   byte  numMeasures;                  // # measures in sequence

   byte  duration;                     // ongoing duration of prior entry being recorded
   bool  quantized;                    // if true, prior entry was quantized to next jiff
   bool  fullHouse;                    // if true, sequence buffer is full

   void  done();                       // executed at end of recording

   public:

   bool  ignoreKeyUp;                  // if true, do not record KEY_UP events
   bool  charEv( char code );          // handle a character event
   void  dynamics();                   // perform a dynamic update 
   bool  evHandler( obEvent );         // handle an onboard event
   void  setBeats( byte );             // set the number of beats per measure
   void  setMeasures( byte );          // set the number of measures in sqnc

   bool  recording()                   // returns true if recording is in process
   { 
      return recPhase; 
   }

} ;

/******************************************************************************
 *
 *                                StepSqnc
 *
 ******************************************************************************/

class StepSqnc : public SequencerRAM   // a step-sequencer
{
   typedef SequencerRAM super;         // superclass is Sequencer

   using SequencerRAM::SequencerRAM;

   public:

   bool   charEv( char );              // process a character event

} ;

#endif   // ifndef SEQUENCER_H_INCLUDED
