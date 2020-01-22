/*
    Sequencer.cpp  

    Implementation of Sequencer classes.

    See comments in Sequencer.h for a discussion of jiffs and sequence buffers.

    ---------------------------------------------------------------------------
 
    Copyright (C) 2016, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
   ---------------------------------------------------------------------------
*/

#include "Audio.h"
#include "Sequencer.h"
#include "System.h"

// uncomment the following line if you want to use developer functions

// #define DEVELOPER

Sequencer* Sequencer::sender = NULL;

/******************************************************************************
 *
 *                                 Sequencer
 *
 ******************************************************************************/

#define DefTempo  108.00               // default tempo 

byte Sequencer::atIdxBump()
{
   return at(idx++);
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Sequencer::charEv
 *
 *  Desc:  Process a character event.
 *
 *  Args:  code             - character to process
 *
 *  Memb: +jiffDC           - downcounter (in audio ticks) to next jiff
 *        +playback         - playback status { pbON/OFF/PAUSED }
 *         target           - ptr to instrument being sequenced 
 *        +tempo            - sequencer tempo (beats per minute) 
 *         ticksPerJiff     - # audio ticks per jiff 
 *
 *  Rets:  status           - true if character was handled
 *
 *----------------------------------------------------------------------------*/

bool Sequencer::charEv( char code )
{
   switch ( code )
   {
      #ifdef INTERN_CONSOLE

      case '[':                        // start sequencer playback

         msgAll( sqncPLAYON );
         playback = pbON;              
         rewind();                     // rewind to 1st record
         exeRec();                     // execute it

         jiffDC = ticksPerJiff;        // set jiffDC for next jiffy
         break;

      case ']':                        // stop sequencer playback

         msgAll( sqncPLAYOFF );
         playback = pbOFF;
         break;

      case '|':                        // pause-resume sequencer playback

         if ( playback != pbOFF ) 
         {
            playback ^= pbPAUSED;
            msgSynth( sqncPLAYON );
         }
         break;

      case 't':                        // set sequencer tempo

         if ( console.getDouble( CONSTR("tempo"), &tempo ) )
            setTempo( tempo );
         break;

      #ifdef DEVELOPER
      case 'D':                        // dump sequence to console
      {
         #define elePerLn 8
         rewind();
         byte togo = length();
         while ( togo )
         {
            byte eleThisLn = togo > elePerLn ? elePerLn : togo;
            console.newlntab();
            for ( byte k = eleThisLn; k > 0; k-- )
            {
               console.print( (int )atIdxBump() );
               if ( k > 1 || togo > eleThisLn )
                  console.romprint( CONSTR(", ") );
            }
            togo -= eleThisLn;
         }
         console.newprompt();
         break;
         #undef elePerLn
      }
      #endif   // DEVELOPER

      #endif   // INTERN_CONSOLE

      #ifdef CONSOLE_OUTPUT

      case chrInfo:
      {
         console.rtab();

         // print playback status character ( [ = on, ] = off, | = paused )

         char icon;                    
         if ( playback == pbOFF )
            icon = ']';
         else if ( playback == pbON )
            icon = '[';
         else                          // paused
            icon = '|';
         console.print( icon );
         console.space();
         
         console.infoDouble( CONSTR("tempo"), tempo );

         #ifdef DEVELOPER  
         {
            console.infoInt( CONSTR("self"), (int )this );
            console.infoInt( CONSTR("sender"), (int )sender );

            console.newlntab();
            console.infoByte( CONSTR("bufSz"), at(0) );
            console.infoByte( CONSTR("sqncLen"), length() );

            // print sum of all durations in sqnc

            word sumJiffs = 0;

            rewind();
            byte token = atIdxBump();
            while ( token != tokenEOS )
            {
               switch ( token )
               {
                  default:                   // keyUp, KeyDn, Rest
                     sumJiffs += atIdxBump();
               }
               token = atIdxBump();
            }

            console.infoInt( CONSTR("sumJiffs"), sumJiffs );
         }
         #endif // DEVELOPER

         break;
      }

      #endif   // CONSOLE_OUTPUT

      case '!':

         playback = pbOFF;
         setTempo( DefTempo );         // fall thru 

      default:

         return super::charEv( code );
   }

   return true;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Sequencer::dynamics
 *
 *  Desc:  Perform a dynamic update.
 *
 *  Memb:  ticksPerDyna     - # audio ticks per dynamic update 
 *         ticksPerJiff     - # audio ticks per jiff 
 *        +exeDC            - downcounter (in jiffs) to next record execution
 *        +jiffDC           - downcounter (in audio ticks) to next jiff
 *         playback         - playback status { pbON/OFF/PAUSED }
 *
 *  Note:  This method is to be called once per system dynamic update.       
 *
 *----------------------------------------------------------------------------*/

void Sequencer::dynamics()
{
   if ( playback == pbON )                      // continue sequencer playback
   {
      if ( jiffDC <= ticksPerDyna )             // process next jiff
      {
         jiffDC += ticksPerJiff - ticksPerDyna; // reload jiff downcounter
         if ( --exeDC == 0 )                    // if next record has arrived
            exeRec();                           // execute it
      }
      else                                      // next jiff not yet arrived
         jiffDC -= ticksPerDyna;                // update jiff downcounter
   }
}


/*----------------------------------------------------------------------------*
 *
 *  Name:  Sequencer::exeRec
 *
 *  Desc:  Execute the next record in the sequence buffer.
 *
 *  Memb: +exeDC            - downcounter in jiffs to next record execution
 *         idx              - indexes record in sqnc[]
 *         target           - ptr to instrument being sequenced 
 *
 *----------------------------------------------------------------------------*/

void Sequencer::exeRec()
{
   byte token = atIdxBump();
   switch ( token )
   {
      case tokenRest:

         exeDC = atIdxBump();
         break;

      case tokenEOS:

         if ( idx > 2 )          // loop back to 1st rec (if non-null sqnc)
         {
            rewind();
            exeRec();
         }
         else                    // otherwise, turn off playback (null sqnc)
            stop();
         break;

      default:                   // interpret record as either keyUp or KeyDn
      {
         obEvent o;

         if ( token < 128 )
            o.setKeyDn( (key )token );
         else
            o.setKeyUp( (key )token );
         o.setOctOn();          // flag as octave-specific

         target->evHandler( o );
         exeDC = atIdxBump();
      }
   }
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Sequencer::getTempo
 *
 *  Desc:  Get the sequencer tempo.
 *
 *  Memb:  tempo            - sequencer tempo (jiffs per minute) 
 *
 *  Rets:  tempo            - sequencer tempo (jiffs per minute) 
 *
 *  Note:  The tempo is measured in jiffs ("in a jiffy") per minute.
 *
 *         A jiff is the smallest unit of time kept by the sequencer.
 * 
 *         The duration of notes and rests in a sequence are quantized
 *         in jiffs.
 *
 *----------------------------------------------------------------------------*/      

double Sequencer::getTempo()
{
   return tempo;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Sequencer::length
 *
 *  Desc:  Returns length of compiled sequence.
 *
 *  Rets:  length           - length of compiled sequence 
 *
 *----------------------------------------------------------------------------*/      

byte Sequencer::length()
{
   byte i = 0; 
   while ( at( ++i ) != tokenEOS ) ;
   return i;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Sequencer::record
 *
 *  Desc:  Record a new sequence.
 *
 *  Note:  This method is a wrapper for charEv('r'), and thus a defacto 
 *         virtual routine. A derived class can modify the behaviour of
 *         record() by handling the 'r' character event.
 *
 *         Any existing sequence is overwritten.
 *
 *----------------------------------------------------------------------------*/      

void Sequencer::record()
{
   charEv('r');
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Sequencer::rewind
 *
 *  Desc:  Rewind the sequence to the 1st record.
 *
 *  Memb: +idx              - index into sqnc[] 
 *
 *----------------------------------------------------------------------------*/      

void Sequencer::rewind()
{
   idx = 1;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Sequencer::setTempo
 *
 *  Desc:  Set the sequencer tempo.
 *
 *  Args:  tempo            - sequencer tempo (beats per minute) 
 *
 *  Memb: +tempo            - sequencer tempo (beats per minute) 
 *        +ticksPerJiff     - # audio ticks per jiff 
 *        +ticksPerHalf     - # audio ticks per 1/2 jiff 
 *
 *  Note:  The tempo can range from 0.6 to 380.78 Hz.
 *
 *         The duration of notes and rests in a sequence are quantized
 *         in jiffs.
 *
 *----------------------------------------------------------------------------*/      

void Sequencer::setTempo( double bpm )
{
   const double TPM    = audioRate * 60.0;   // ticksPerMinute
      
   // minimum ticks-per-jiff must exceed ticks-per-dynamic-update
   // maximum ticks-per-jiff must fit in 16 bits

   const double minTPJ = ticksPerDyna + 1;
   const double maxTPJ = (double )0xFFFE;

   // ticksPerBeat   = ticksPerJiff * jiffsPerBeat;

   const double minTPB = minTPJ * jiffsPerBeat;
   const double maxTPB = maxTPJ * jiffsPerBeat;

   // beatsPerMinute = ticksPerMinute / ticksPerBeat;
    
   const double minBPM = TPM / maxTPB;    // evaluates to 000.60
   const double maxBPM = TPM / minTPB;    // evaluates to 380.78

   if ( bpm < minBPM )
      bpm = minBPM;
   if ( bpm > maxBPM )
      bpm = maxBPM;

   tempo        = bpm;
   double jpm   = bpm*jiffsPerBeat;

   ticksPerJiff = TPM / jpm;
   ticksPerHalf = ticksPerJiff >> 1;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Sequencer::start
 *
 *  Desc:  Start sequencer playback (from the beginning of sequence).
 *
 *  Note:  This method is a wrapper for charEv('['), and thus a defacto 
 *         virtual routine. A derived class can modify the behaviour of
 *         start() by handling the '[' character event.
 *
 *----------------------------------------------------------------------------*/      

void Sequencer::start()
{
   charEv('[');
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Sequencer::stop
 *
 *  Desc:  Stop sequencer playback.
 *
 *  Note:  This method is a wrapper for charEv(']'), and thus a defacto 
 *         virtual routine. A derived class can modify the behaviour of
 *         stop() by handling the ']' character event.
 *
 *----------------------------------------------------------------------------*/      

void Sequencer::stop()
{
   charEv(']');
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Sequencer::_pause_
 *
 *  Desc:  Toggle pausing of sequencer playback.
 *
 *  Note:  If the sequencer is currently playing back, then a call to this 
 *         method will pause the playback at the current point in the sequence.
 *         If sequencer playback is currently paused, then a call to this
 *         method will resume the playback from the point in the sequence at
 *         which it was paused. If sequencer playback is currently stopped,
 *         then a call to this method has no effect.
 *
 *         This method is a wrapper for charEv('|'), and thus a defacto 
 *         virtual routine. A derived class can modify the behaviour of
 *         _pause_() by handling the '|' character event.
 *
 *----------------------------------------------------------------------------*/      

void Sequencer::_pause_()
{
   charEv('|');
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Sequencer::msgAll
 *
 *  Desc:  Send a message to both the target and the synth.
 *
 *  Args:  msg              - message
 *
 *  Glob:  target           - ptr to instrument being sequenced 
 *
 *  Memb:  +sender          - self ptr to Sequencer object sending message
 *
 *  Note:  Sequencers can send "messages" to other objects via their charEv() 
 *         method.
 * 
 *----------------------------------------------------------------------------*/

void Sequencer::msgAll( char msg )
{
   target->charEv( msg );
   if ( synth != target )
      msgSynth( msg );
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Sequencer::msgSynth
 *
 *  Desc:  Send a message to the synth.
 *
 *  Args:  msg              - message
 *
 *  Glob:  synth            - ptr to runtime synth 
 *
 *  Memb:  +sender          - self ptr to Sequencer object sending message
 *
 *  Note:  Sequencers can send "messages" to the synth via its charEv() method.
 * 
 *----------------------------------------------------------------------------*/

void Sequencer::msgSynth( char msg )
{
   sender = this;
   synth->charEv( msg );
}

/******************************************************************************
 *
 *                             SequencerRAM
 *
 ******************************************************************************/

/*----------------------------------------------------------------------------*
 *
 *  Name:  SequencerRAM::at
 *
 *  Desc:  Return value at a given index in sqnc[]
 *
 *  Args:  ith              - index into sqnc[] 
 *
 *  Memb:  sqnc[]           - buffer of compiled sequence records
 *
 *----------------------------------------------------------------------------*/

byte SequencerRAM::at( byte ith )
{
   return sqnc[ith];
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  SequencerRAM::charEv
 *
 *  Desc:  Process a character event.
 *
 *  Args:  code             - character to process
 *
 *  Rets:  status           - true if character was handled
 *
 *----------------------------------------------------------------------------*/

bool SequencerRAM::charEv( char code )
{
   switch ( code )
   {
      #ifdef INTERN_CONSOLE
      #ifdef DEVELOPER

      case 'R':                        // read sequence from 00h in EEPROM

         if ( ! read( 0 ) )
         {
            console.romprint( CONSTR("Sqnc too long") );
            console.newprompt();
         }
         break;

      case 'W':                        // write sequence to 00h in EEPROM
      
         write( 0 );
         break;

      #endif   // DEVELOPER
      #endif   // INTERN_CONSOLE

      case 'r':                        // record a new sequence

         stop();                       // insure sequencer is not playing
         break;

      case '!':

         sqnc[1] = tokenEOS;           // clear sequence

      default:

         return super::charEv( code );
   }

   return true;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  SequencerRAM::load
 *
 *  Desc:  Load a sequence from ROM.
 *
 *  Args:  addr             - address in ROM 
 *
 *  Rets:  status           - if true, load was successful 
 *
 *  Memb: +sqnc[]           - buffer of compiled sequence records
 *
 *----------------------------------------------------------------------------*/

bool SequencerRAM::load( const byte *addr )
{
   byte size = pgm_read_byte_near( addr++ );
   if ( size <= sqnc[0] )
   {
      for ( byte i = 1; i <= size; i++ )
         sqnc[i] = pgm_read_byte_near( addr++ );
      return true;
   }
   else
      return false;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  SequencerRAM::read
 *
 *  Desc:  Read a sequence from EEPROM.
 *
 *  Args:  addr             - address in EEPROM 
 *
 *  Rets:  status           - if true, read was successful 
 *
 *  Memb: +sqnc[]           - buffer of compiled sequence records
 *
 *----------------------------------------------------------------------------*/

bool SequencerRAM::read( word addr )
{
   byte size = readNVS( addr++ );
   if ( size <= sqnc[0] )
   {
      readNVS( addr, &sqnc[1], size );
      return true;
   }
   else
      return false;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  SequencerRAM::write
 *
 *  Desc:  Write sequence to EEPROM.
 *
 *  Args:  addr             - address in EEPROM 
 *
 *  Memb:  sqnc[]           - buffer of compiled sequence records
 *
 *----------------------------------------------------------------------------*/

void SequencerRAM::write( word addr )
{
   writeNVS( addr, &sqnc[0], sqnc[0] );
}

/******************************************************************************
 *
 *                             SequencerROM
 *
 ******************************************************************************/

/*----------------------------------------------------------------------------*
 *
 *  Name:  SequencerROM::at
 *
 *  Desc:  Return value at a given index in sqnc[]
 *
 *  Args:  ith              - index into sqnc[] 
 *
 *  Memb:  sqnc             - ptr to sequence records in ROM
 *
 *----------------------------------------------------------------------------*/

byte SequencerROM::at( byte ith )
{
   return pgm_read_byte_near( sqnc+ith );
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  SequencerROM::charEv
 *
 *  Desc:  Process a character event.
 *
 *  Args:  code             - character to process
 *
 *  Rets:  status           - true if character was handled
 *
 *----------------------------------------------------------------------------*/

const byte nullSqnc[] PROGMEM = { 2, tokenEOS };

bool SequencerROM::charEv( char code )
{
   switch ( code )
   {
      case '!':

         sqnc = &nullSqnc[0];          // clear sequence

      default:

         return super::charEv( code );
   }
   return true;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  SequencerROM::load
 *
 *  Desc:  Load a sequence from ROM.
 *
 *  Args:  addr             - address in ROM 
 *
 *  Rets:  status           - if true, load was successful 
 *
 *  Memb:  sqnc             - ptr to sequence records in ROM
 *
 *----------------------------------------------------------------------------*/

bool SequencerROM::load( const byte *addr )
{
   sqnc = addr;
   return true;
}

/******************************************************************************
 *
 *                           RealTimeSequencer
 *
 ******************************************************************************/

#define  midTempo   96.0               // default tempo (a medium gait)

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
 *        +ignoreKeyUp      - if true, do not record KEY_UP events
 *        +jiffDC           - downcounter (in audio ticks) to next jiff
 *        +measureDC        - downcounter (in beats) to next measure
 *        +numMeasures      - # measures in sequence
 *        +quantized        - if true, prior entry was quantized to next jiff
 *        +recPhase         - current phase in the recording process
 *        +transDC          - downcounter (in jiffs) to transition phase
 *         ticksPerDyna     - # audio ticks per dynamic update 
 *         ticksPerJiff     - # audio ticks per jiff 
 *        +token0          - token to be inserted when recPhase goes ON 
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

         recPhase = recCUE;
         msgSynth( sqncCUE );

         rewind();                     

         fullHouse = false;
         quantized = false;
         token0    = tokenRest;         // "no key pressed during transition"

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

         ignoreKeyUp = false;          // KEY_UP events recorded by default

         recPhase = recOFF;
         msgSynth( sqncRECOFF );

         setTempo( midTempo );
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
 *        +idx              - current index for sqnc[]
 *         fullHouse        - if true, sequence buffer is full
 *        +recPhase         - recording status
 *        +sqnc[]           - buffer for compiled sequence records
 *
 *----------------------------------------------------------------------------*/

void RealTimeSequencer::done()
{
   if ( recPhase )
   {
      if ( recPhase != recCUE )
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
      recPhase = recOFF;                  // turn recording off
      msgSynth( sqncRECOFF );             // "I'm through recording"
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
 *        +idx              - index of next record in sqnc[] 
 *        +jiffDC           - downcounter (in audio ticks) to next jiff
 *         jiffsPerBeat     - # jiffs in one beat
 *        +quantized        - if true, prior entry was quantized to next jiff
 *        +recPhase         - current phase in the recording process
 *        +sqnc[]           - buffer for compiled sequence records
 *         ticksPerDyna     - # audio ticks per dynamic update 
 *         ticksPerJiff     - # audio ticks per jiff 
 *         token0           - token to be inserted when recPhase goes ON 
 *        +transDC          - downcounter (in jiffs) to transition phase
 *
 *  Note:  This method is to be called once per system dynamic update.       
 *
 *----------------------------------------------------------------------------*/

void RealTimeSequencer::dynamics()
{
   if ( recPhase )
   {
      if ( jiffDC <= ticksPerDyna )             // process next jiff
      {
         jiffDC += ticksPerJiff - ticksPerDyna; // reload jiff downcounter

         if ( transDC && (--transDC == 0) )
         {
            // transition phase has arrived

            msgSynth( sqncRECON );              // "I'm beginning to recording"
            recPhase = recTRANS;                // set recording phase
         }
         else if ( recPhase == recON )
         {
            if ( duration == 254 )              // concatenate a new rest
            {
               sqnc[ idx++ ] = 254;  
               sqnc[ idx++ ] = tokenRest;  
               duration = 0;
            }
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
               if ( recPhase == recTRANS )      // true recording phase has arrived
               {
                  recPhase = recON;             // set recording phase to ON
                  sqnc[ idx++ ] = token0;       // insert token0 
                  duration = 0;                 // reset duration

               }
               measureDC = beatsPerMeasure;
               if ( --endDC == 0 )              // end of sqnc has arrived
                  done();
               else
                  msgSynth( sqncDNBEAT );       // "I'm at a downbeat"
            }
            else
               msgSynth( sqncUPBEAT );          // "I'm at an upbeat"
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
 *         ignoreKeyUp      - if true, do not record KEY_UP events
 *         jiffDC           - downcounter (in audio ticks) to next jiff
 *         numMeasures      - # measures in sequence
 *        +quantized        - if true, prior entry was quantized to next jiff
 *         recPhase         - current phase in the recording process
 *        +sqnc[]           - buffer for compiled sequence records
 *         target           - ptr to instrument being sequenced 
 *         ticksPerHalf     - # ticks per 1/2 jiff
 *         token0           - token to be inserted when recPhase goes ON 
 *
 *  Rets:  status           - true if the event was handled
 *
 *----------------------------------------------------------------------------*/      

bool RealTimeSequencer::evHandler( obEvent ev )
{
   evType evtype = ev.type();

   switch ( evtype )
   {
      case KEY_DOWN:
      case KEY_UP:
      {
         target->evHandler( ev );   // handle normally on target

         if ( ignoreKeyUp && (evtype == KEY_UP) ) 
            break;

         if ( recPhase < recTRANS ) 
            break;

         // we are either in transition or true recording phase

         if ( fullHouse )           // if sequencer buffer is full, eat key
            break;            
      
         if ( jiffDC < ticksPerHalf ) // quantize to next Jiff
         {
            duration++;
            quantized = true;
         }

         // extract key info from event record and encode it in keyval

         byte pos;                           // key position
         byte oct;                           // key octave
         byte keyval;                        // encoded key value

         pos = ev.getKey().position();      // get key position 
         oct = target->keybrd.getOctave();  // get current octave on target

         keyval = pos + (oct<<4);   

         if ( evtype == KEY_UP ) 
            keyval += 128;

         // if we're in the transition phase and event is a KEY_DOWN, set token0

         if ( recPhase == recTRANS )
         {
            if ( evtype == KEY_DOWN ) 
               token0 = keyval;
         }

         // else we are in the true recording phase: append key to sequence buffer

         else  // ( recPhase == recON )
         {
            // compile duration field for prior note/rest

            if ( idx > 1 )             
            {
               if ( duration )
                  sqnc[ idx++ ] = duration;
               else                    // cull prior entry on 0 duration
                  --idx;
            }

            // append key to sequence buffer, space permitting 
      
            if ( idx > sqnc[0] - 5 )   // space for ON+duration+OFF+duration+EOS
               fullHouse = true;
            else
            {
               sqnc[ idx++ ] = keyval; // store 1st byte of new key record
               duration      = 0;      // set initial duration
            }
         }

         break;
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
   jiffsToTrans    = (jiffsPerBeat * beatsPerMeasure) - SIXT_;
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

/******************************************************************************
 *
 *                             StepProgrammer
 *
 ******************************************************************************/

class StepProgrammer : public KeyBrd
{
   typedef KeyBrd super;         // superclass is KeyBrd

   Instrument *target;           // instrument being step-programmed

   boolean  targMute;            // target keybrd's mute status on entry

   public:

   byte    bufLen;               // length of sequence buffer
   byte   *sqnc;                 // buffer for compiled sequence records
   byte    idx;                  // current index into sqnc[] 
   byte    duration;             // duration of note/rest in progress
   byte    noteValue;            // time-value of each increment in duration
   boolean fullHouse;            // if true, sequence buffer is full

   boolean charEv( char code )   // handle a character event
   {
      switch ( code )
      {
         #ifdef INTERN_CONSOLE

         case '.':               // bump duration of ongoing note/rest
         case ' ':

            #ifdef CONSOLE_OUTPUT
               console.print( '.' );
            #endif
            duration += noteValue;
            break;

         case 'Q':              // set default time value to quarter note

            noteValue = QUART_;
            break;

         case 'E':               // set default time value to eigth note

            noteValue = EIGHT_;
            break;

         case 'S':

            noteValue = SIXT_;   // set default time value to sixteenth note
            break;

         case 'V':               // set default time value (# of jiffs)

            console.getByte( CONSTR("noteVal"), &this->noteValue );
            break;

         #endif // INTERN_CONSOLE

         case focusPOP: 
      
            if ( idx > 1 && ! fullHouse )  // compile duration for current note
               sqnc[ idx++ ] = duration; 
            sqnc[ idx ] = tokenEOS;
            target->keybrd.setMute( targMute );   // restore target keybrd's mute status
            break;
      
         case chrInfo:

            super::charEv( code );
            console.infoByte( CONSTR("noteVal"), noteValue );
            break;

         default:                         
         
            return super::charEv( code );
      }
      return true;
   }

   boolean evHandler( obEvent ev ) 
   {
      switch ( ev.genus() )
      {
         case evKEY:

         {
            // ignore KEY_UP events

            if ( ev.type() == KEY_UP )
            {
               target->evHandler( ev );  // play on target instrument
               break;
            }
            
            if ( fullHouse )     // if sequencer buffer is full, eat key
            {
               informFull();
               return true;            
            }
      
            /*  --- Complete the compilation of any note/rest in progress ---
      
               If records exist in the buffer, then by definition a note is in
               process.

               If no records exist in the buffer, and the duration is nonzero, then 
               by definition a (initial) rest is in progress.

            */    

            if ( idx > 1 )             // compile duration for note in progress
            {
               sqnc[ idx++ ] = duration;  
            }
            else if ( duration > 0 )   // compile a Rest with duration
            {
               sqnc[ idx++ ] = tokenRest;  
               sqnc[ idx++ ] = duration;  
            }

            /*  --- Begin the compilation of a new note, space permitting --- */
      
            if ( idx > bufLen - 3 )    // no space for another note + EOS
            {
               fullHouse = true;
               informFull();
               return true;
            }
            else
            {
               ev.setOctave( octave );    // set octave info in event
               key k = ev.getKey();

               sqnc[ idx++ ] = k.val;     // compile 1st byte of new note record
               duration      = noteValue; // set initial duration

               ev.setOctOn();             // flag as containing octave info

               KeyBrd::evHandler( ev );  // echo key to console
               target->evHandler( ev );  // and play on target instrument

               return true;
            }

            break; 
         }

         case evBUT:

            switch ( ev.type() )
            {
               case BUT0_TAP:           // lower keybrd by one octave

                  downOctave();
                  break;

               case BUT1_TAP:           // raise keybrd by one octave

                  upOctave();
                  break;

               case BUT1_PRESS:         // bump duration of ongoing note/rest

                  charEv('.');
                  break;

               default:                 

                  super::evHandler( ev );
            }

            return true;               // "button event handled" in all cases
            break;

         default:

            return false;

      }  // switch ( ev.genus() )

   }

   void informFull()
   {
      console.romprint( CONSTR("full!") );
      console.newprompt();
   }
   
   #ifdef CONSOLE_OUTPUT
   const char *prompt()  { return CONSTR("step"); }
   #endif

   void push( Instrument *targ, byte *bufptr )
   {
      target    = targ;
      bufLen    = bufptr[0];
      sqnc      = bufptr;
      idx       = 1;
      duration  = 0;
      noteValue = EIGHT_;
      fullHouse = false;
      setOctave( target->keybrd.getOctave() ); 
      targMute  = target->keybrd.muted();
      target->keybrd.setMute( false );
      console.pushMode( this );
   }

} programmer;


/******************************************************************************
 *
 *                                StepSqnc
 *
 ******************************************************************************/

bool StepSqnc::charEv( char code )
{
   switch ( code )
   {
      #ifdef INTERN_CONSOLE

      case 'r':                        // record a new sequence using step-programmer

         super::charEv( code );
         programmer.push( target, &this->sqnc[0] ); 
         break;

      #endif

      default:

         return super::charEv( code );
   }

   return true;
}

