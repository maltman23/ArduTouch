/*
    Instrument.cpp  

    Implementation of the Instrument class.

    ---------------------------------------------------------------------------
 
    Copyright (C) 2016, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
   ---------------------------------------------------------------------------
*/

#include "Instrument.h"
#include "Sequencer.h"     
#include "System.h"     

/******************************************************************************
 *
 *                               Instrument 
 *                                                                            
 ******************************************************************************/

/*----------------------------------------------------------------------------*
 *
 *  Name:  Instrument::charEv
 *
 *  Desc:  Process a character event.
 *
 *  Args:  code             - character to process
 *
 *  Memb:  keybrd           - resident keyboard 
 *        +xpose            - transpose notes by this many intervals
 *
 *  Rets:  status           - true if character was handled
 *
 *----------------------------------------------------------------------------*/      

boolean Instrument::charEv( char code )    
{
   switch ( code )
   {
      #ifdef INTERN_CONSOLE

      case 'k':                     // push console (piano) keyboard mode

         console.pushMode( &this->keybrd );
         break;

      case 'x':                     // set transposition amount

         console.getSByte( CONSTR("xpose"), &this->xpose );
         break;

      #ifdef IMPLICIT_SEQUENCER

      case 'S':                     // push sequencer

         if ( sqnc ) 
            console.pushMode( sqnc );  
         break;

      case '|':                     // propagate transport commands
      case '[':
      case ']':

         if ( sqnc ) 
            sqnc->charEv( code );
         break;

      #endif   // IMPLICIT_SEQUENCER

      #endif   // INTERN_CONSOLE

      #ifdef CONSOLE_OUTPUT

      case chrInfo:              

         super::charEv( chrInfo );

         keybrd.brief();
         console.infoInt( CONSTR("xpose"), xpose );

         #ifdef IMPLICIT_SEQUENCER
            if ( sqnc ) sqnc->brief();
         #endif

         break;

      #endif  // CONSOLE_OUTPUT

      case '!':

       #ifdef IMPLICIT_SEQUENCER
         if ( sqnc ) sqnc->charEv( code );
       #endif

         keybrd.reset();
         xpose = 0;                 // no transposition

         // fall thru

      default:

          return super::charEv( code );
   }
   return true;
}

#ifdef IMPLICIT_SEQUENCER
/*----------------------------------------------------------------------------*
 *
 *  Name:  Instrument::dynamics
 *
 *  Desc:  Perform a dynamic update.
 *
 *  Memb:  sqnc             - resident sequencer (optional)
 *
 *----------------------------------------------------------------------------*/

void Instrument::dynamics()
{
   if ( sqnc ) sqnc->dynamics();
}
#endif

/*----------------------------------------------------------------------------*
 *
 *  Name:  Instrument::evHandler
 *
 *  Desc:  Handle an onboard event.
 *
 *  Args:  ev               - onboard event
 *
 *  Memb:  keybrd           - resident keyboard 
 *
 *  Rets:  status           - true if the event was handled
 *
 *----------------------------------------------------------------------------*/      

boolean Instrument::evHandler( obEvent ev )
{
   evType evtype = ev.type();

   switch ( evtype )
   {
      case KEY_DOWN:                // note On  
      case KEY_UP:                  // note Off 
      {
         if ( ! keybrd.muted() )
         {
            key k = ev.getKey(); 

            if ( ! ev.octOn() )
               k.setOctave( keybrd.octave );

            if ( evtype == KEY_DOWN )
               noteOn( k );
            else
               noteOff( k );
         }
         else
            return false;

         break;
      }

      case BUT0_TAP:                // octave down (if keybrd active)

         if ( ! keybrd.muted() )
            keybrd.downOctave();
         else
            return false;
         break;

      case BUT1_TAP:                // octave up (if keybrd active)

         if ( ! keybrd.muted() )
            keybrd.upOctave();
         else
            return false;
         break;

      default:

         return super::evHandler( ev );
   }
   return true;
}

#ifdef KEYBRD_MENUS

/*----------------------------------------------------------------------------*
 *
 *  Name:  Instrument::menu
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
 *           |   |   |   |   |   |   |   |   | k |   | . |   | 
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |    ___     ___    |    ___     ___     ___    | 
 *           |     |       |     |     |       |       |     |
 *           |     |       |     |     |       |       |     |
 *           |     |       |     |     |   x   |   <   |  !  |
 *           |     |       |     |     |       |       |     |
 *           |     |       |     |     |       |       |     |
 *           -------------------------------------------------
 *
 *----------------------------------------------------------------------------*/      

char Instrument::menu( key k )
{
   switch ( k.position() )
   {
      case 7:  return 'x';
      case 8:  return 'k';
      default: return super::menu( k );
   }
}

#endif

#ifdef IMPLICIT_SEQUENCER
/*----------------------------------------------------------------------------*
 *
 *  Name:  Instrument::useSequencer
 *
 *  Desc:  Assign a sequencer to the instrument.
 *
 *  Args:  sqnc             - ptr to resident sequencer (optional)
 *
 *  Memb: +sqnc             - ptr to resident sequencer (optional)
 *
 *----------------------------------------------------------------------------*/

void Instrument::useSequencer( Sequencer *sqnc )
{
   this->sqnc = sqnc;
   sqnc->target = this;
}
#endif

/******************************************************************************
 *
 *                            StereoInstrument 
 *                                                                            
 ******************************************************************************/

/*----------------------------------------------------------------------------*
 *
 *  Name:  StereoInstrument::output
 *
 *  Desc:  Write output to a pair of audio buffers.
 *
 *  Args:  bufL             - ptr to left audio buffer  
 *         bufR             - ptr to right audio buffer  
 *
 *  Glob:  audioBufSz       - size of system audio buffers
 *
 *  Note:  If not overridden, this method will call the mono output() method
 *         for the left audio buffer, and then copy the left audio buffer to 
 *         the right audio buffer.
 *
 *----------------------------------------------------------------------------*/      

void StereoInstrument::output( char *bufL, char *bufR )
{
   output( bufL );
   for ( byte i = 0; i < audioBufSz; i++ )
      bufR[i] = bufL[i];
}

