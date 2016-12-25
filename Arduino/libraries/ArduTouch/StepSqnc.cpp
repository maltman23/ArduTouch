/*
    StepSqnc.cpp  

    Implementation of the Step Sequencer.

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
#include "ConKeyBrd.h"
#include "StepSqnc.h"
#include "System.h"

#define DefTempo     108.00      // default tempo

/* sequence record tokens */

#define tokenRest    253         // rest token (# beats follows in next byte)
#define tokenUnMute  254         // unMute subject, depending on context
#define tokenEOS     255         // end-of-sequence token

class StepProgrammer : public ConKeyBrd
{
   public:

   byte    bufLen;               // length of sequence buffer
   byte   *sqnc;                 // buffer for compiled sequence records
   byte    idx;                  // current index into sqnc[] 
   byte    duration;             // duration of note/rest in progress
   boolean fullHouse;            // if true, sequence buffer is full

   boolean charEv( char code )   // handle a character event
   {
      switch ( code )
      {
         #ifdef INTERN_CONSOLE
         case '.':               // bump duration
         case ' ':

            ++duration;
            break;
         #endif

         case focusPOP: 
      
            if ( idx > 0 && ! fullHouse )  // compile duration for current note
               sqnc[ idx++ ] = duration; 
            sqnc[ idx ] = tokenEOS;
            break;
      
         default:                         
         
            return ConKeyBrd::charEv( code );
      }
      return true;
   }

   boolean evHandler( obEvent ev ) 
   {
      switch ( ev.genus() )
      {
         case evKEY:

         {
            key k = ev.getKey();

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

            if ( idx > 0 )             // compile duration for note in progress
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
            }
            else
            {
               ConKeyBrd::evHandler( ev );  // echo key to console
               sqnc[ idx++ ] = k.val;  // compile 1st byte of new note record
               duration      = 1;      // set its preliminary duration to 1
            }

            return fullHouse;
            break; 
         }

         case evBUT:

            switch ( ev.type() )
            {
               case BUT0_TAP:       

                  downOctave();
                  break;

               case BUT1_TAP:       

                  upOctave();
                  break;

               case BUT1_PRESS:          

                  ++duration;
                  break;

               default:                 

                  ConKeyBrd::evHandler( ev );
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
   const char *prompt()  { return CONSTR("sqnc"); }
   #endif

   void push( byte *bufptr, byte len )
   {
      bufLen    = len;
      sqnc      = bufptr;
      idx       = 0;
      duration  = 0;
      fullHouse = false;
      console.pushMode( this );
   }

} programmer;


bool StepSqnc::_charEv( char code )
{
   bool handled = true;

   switch ( code )
   {
      #ifdef INTERN_CONSOLE
      case '[':                  // start sequencer playback

         start();
         break;

      case ']':                  // stop sequencer playback

         stop();
         break;

      case '|':                  // pause-resume sequencer playback

         if ( playback != pbOFF ) playback ^= pbPAUSED;
         break;

      case 'S':                  // step-program a new sequence

         stop();                 // insure sequencer is not playing
         programmer.push( &this->sqnc[0], MaxLen ); // run the programmer
         break;

      case 't':                  // set sequencer tempo

         if ( console.getDouble( CONSTR("tempo"), &tempo ) )
            setTempo( tempo );
         break;

      case 'k':                  // swallow "keybrd" event if sequence is playing

         if ( playback == pbON  ) break;
      #endif

      case '!':

         playback = pbOFF;
         sqnc[0] = tokenEOS;
         setTempo( DefTempo );   // fall thru to "not handled"

      default:                   // event has not been handled

         handled = false;
   }

   return handled;
}

void StepSqnc::exeRec()
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
            subject->setMute( false );
            unMute = false;
         }
         exeRec();               // execute next rec after unMute
         break;

      case tokenEOS:

         if ( idx > 1 )          // loop back to 1st rec (if non-null sqnc)
         {
            idx = 0;
            exeRec();
         }
         else                    // otherwise, turn off playback (null sqnc)
            stop();
         break;

      default:                   // process note record
      {
         obEvent o;
         o.setKeyDn( (key )token );
         subject->evHandler( o );
         exeDC = sqnc[ idx++ ];
      }
   }
}

void StepSqnc::info()
{
   #ifdef CONSOLE_OUTPUT
   console.rtab();
   console.romprint( CONSTR("sqnc:") );
   console.romprint( playback == pbOFF ? CONSTR("Off ") :
                   ( playback&pbPAUSED ? CONSTR("Paused ") : CONSTR("On ")) );
   console.infoDouble( CONSTR("tempo"), tempo );
   #endif
}

void StepSqnc::cont()
{
   if ( playback == pbON )                      // continue sequencer playback
   {
      if ( beatDC <= audioBufSz )               // process next beat
      {
         beatDC += ticksPerBeat - audioBufSz;   // reload beat downcounter
         if ( --exeDC == 0 )                    // if next record has arrived
            exeRec();                           // execute it
      }
      else                                      // next beat not yet arrived
         beatDC -= audioBufSz;                  // update beat downcounter
   }
}

void StepSqnc::setTempo( double t )
{
   const double minTempo = (audioRate * 60.0) / (double )0xFFFE;  

   if ( t < minTempo )  // make sure ticksPerBeat fits in a word
      t = minTempo;

   tempo        = t;
   ticksPerBeat = ((double )ticksPerSec * 60.0) / t;
   
}

void StepSqnc::start()
{
   playback = pbON;              // set playback on

   if ( sqnc[0] == tokenRest && ! subject->amMute() )
   {
      subject->setMute( true );
      unMute = true;
   }
   else
      unMute = false;

   idx = 0;                      // rewind to 1st record
   exeRec();                     // execute it

   beatDC = ticksPerBeat;        // set beatDC for next beat
}

void StepSqnc::stop()
{
   playback = pbOFF;
}

