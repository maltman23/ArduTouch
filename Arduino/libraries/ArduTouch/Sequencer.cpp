/*
    Sequencer.cpp  

    Implementation of Sequencer classes.

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

/******************************************************************************
 *
 *                                 Sequencer
 *
 ******************************************************************************/

#define DefTempo     108.00      // default tempo (jiffs per minute)

bool Sequencer::charEv( char code )
{
   switch ( code )
   {
      #ifdef INTERN_CONSOLE

      case '[':                        // start sequencer playback

         if ( targetMuting && sqnc[1] == tokenRest && ! target->muted() )
         {
            target->setMute( true );
            unMute = true;
         }
         else
            unMute = false;

         playback = pbON;              
         idx = 1;                      // rewind to 1st record
         exeRec();                     // execute it

         jiffDC = ticksPerJiff;        // set jiffDC for next jiffy
         break;

      case ']':                        // stop sequencer playback

         playback = pbOFF;
         break;

      case '|':                        // pause-resume sequencer playback

         if ( playback != pbOFF ) playback ^= pbPAUSED;
         break;

      case 'r':                        // record a new sequence

         stop();                       // insure sequencer is not playing
         break;

      case 't':                        // set sequencer tempo

         if ( console.getDouble( CONSTR("tempo"), &tempo ) )
            setTempo( tempo );
         break;

      #ifdef DEVELOPER

      case 'D':                        // dump sequence to console
      {
         #define elePerLn 8

         byte togo = length();
         byte ith = 1;

         // sequence length gets separate line

         console.newlntab();
         console.print( (int )togo );
         console.romprint( CONSTR(", ") );

         while ( togo )
         {
            byte eleThisLn = togo > elePerLn ? elePerLn : togo;
            console.newlntab();
            for ( byte k = eleThisLn; k > 0; k-- )
            {
               console.print( (int )sqnc[ith++] );
               if ( k > 1 || togo > eleThisLn )
                  console.romprint( CONSTR(", ") );
            }
            togo -= eleThisLn;
         }
         console.newprompt();
         break;
         #undef elePerLn
      }

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
            console.newlntab();
            console.infoByte( CONSTR("bufSz"), bufSize() );
            console.infoByte( CONSTR("sqncLen"), length() );

            // print sum of all durations in sqnc

            word sumJiffs = 0;
            idx = 1;

            while ( sqnc[idx] != tokenEOS )
            {
               byte token = sqnc[ idx++ ];
               switch ( token )
               {
                  case tokenUnMute:

                     break;

                  case tokenRest:
                  default:                   // interpret record as either keyUp or KeyDn

                     sumJiffs += sqnc[ idx++ ];
               }
            }

            console.infoInt( CONSTR("sumJiffs"), sumJiffs );
         }

         #endif // DEVELOPER

         break;
      }

      #endif   // CONSOLE_OUTPUT

      case '!':

         playback = pbOFF;
         sqnc[1] = tokenEOS;           // clear sequence
         setTargetMuting( true );      // mute target on beginning rest
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
 *  Name:  Sequence::exeRec
 *
 *  Desc:  Execute the next record in the sequence buffer.
 *
 *  Memb: +exeDC            - downcounter in jiffs to next record execution
 *        +idx              - indexes record in sqnc[]
 *         sqnc[]           - buffer for compiled sequence records
 *         target           - ptr to instrument being sequenced 
 *         unMute           - if true, unmute the target on tokenUnMute 
 *
 *----------------------------------------------------------------------------*/

void Sequencer::exeRec()
{
   byte token = sqnc[ idx++ ];
   switch ( token )
   {
      case tokenRest:

         exeDC = sqnc[ idx++ ];
         break;

      case tokenUnMute:

         if ( unMute )           // process unMute record on 1st pass only
         {
            target->setMute( false );
            unMute = false;
         }
         exeRec();               // execute next rec after unMute
         break;

      case tokenEOS:

         if ( idx > 2 )          // loop back to 1st rec (if non-null sqnc)
         {
            idx = 1;
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
         exeDC = sqnc[ idx++ ];
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
 *  Memb:  sqnc[]           - buffer of compiled sequence records
 *
 *  Rets:  length           - length of compiled sequence 
 *
 *----------------------------------------------------------------------------*/      

byte Sequencer::length()
{
   byte i = 0; 
   while ( sqnc[ ++i ] != tokenEOS ) ;
   return i;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Sequencer::load
 *
 *  Desc:  Load a sequence from ROM.
 *
 *  Args:  addr             - address in ROM 
 *
 *  Rets:  status           - if true, read was successful 
 *
 *  Memb: +sqnc[]           - buffer of compiled sequence records
 *
 *----------------------------------------------------------------------------*/

bool Sequencer::load( const byte *addr )
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

#ifdef CONSOLE_OUTPUT
const char *Sequencer::prompt()
{ 
   return CONSTR("Sqnc");
}
#endif

/*----------------------------------------------------------------------------*
 *
 *  Name:  Sequencer::read
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

bool Sequencer::read( word addr )
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
 *  Name:  Sequencer::setTargetMuting
 *
 *  Desc:  Enable or disable the ability of the sequencer to mute and unmute
 *         the target instrument.
 *
 *  Args:  enable           - if true, enable target muting; otherwise disable 
 *
 *  Memb: +targetMuting     - if true, target muting/unmuting is enabled
 *
 *  Note:  When a sequence begins with a rest, and the target instrument is
 *         employing a sound that has infinite release, it may be desireable
 *         when playing back a sequence to mute the target until the fist note
 *         sounds. 
 *
 *----------------------------------------------------------------------------*/

void Sequencer::setTargetMuting( bool enable )
{
   targetMuting = enable;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Sequencer::setTempo
 *
 *  Desc:  Set the sequencer tempo.
 *
 *  Args:  tempo            - sequencer tempo (jiffs per minute) 
 *
 *  Memb: +tempo            - sequencer tempo (jiffs per minute) 
 *        +ticksPerJiff     - # audio ticks per jiff 
 *        +ticksPerHalf     - # audio ticks per 1/2 jiff 
 *
 *  Note:  The tempo is measured in jiffs ("in a jiffy") per minute.
 *
 *         A jiff is the smallest unit of time kept by the sequencer.
 * 
 *         The duration of notes and rests in a sequence are quantized
 *         in jiffs.
 *
 *----------------------------------------------------------------------------*/      

void Sequencer::setTempo( double t )
{
   const double minTempo = (audioRate * 60.0) / (double )0xFFFE;     // ~ 14.36
   const double maxTempo = (audioRate * 60.0) / (ticksPerDyna+1);    // ~ 9138.82

   if ( t < minTempo )        // make sure ticksPerJiff fits in a word
      t = minTempo;
   else if ( t > maxTempo )   // make sure jiff lasts more than 1 dynamic update 
      t = maxTempo;

   tempo        = t;
   ticksPerJiff = ((double )ticksPerSec * 60.0) / t;
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
 *  Name:  Sequencer::write
 *
 *  Desc:  Write sequence to EEPROM.
 *
 *  Args:  addr             - address in EEPROM 
 *
 *  Memb:  sqnc[]           - buffer of compiled sequence records
 *
 *----------------------------------------------------------------------------*/

void Sequencer::write( word addr )
{
   writeNVS( addr, &sqnc[0], sqnc[0] );
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

         case 'V':               // set time-value of each increment in duration

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
            else if ( duration > 0 )   // compile a Rest-duration-UnMute sequence
            {
               sqnc[ idx++ ] = tokenRest;  
               sqnc[ idx++ ] = duration;  
               sqnc[ idx++ ] = tokenUnMute;  
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
      noteValue = 1;
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

