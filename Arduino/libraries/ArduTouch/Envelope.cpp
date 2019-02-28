/*
    Envelope.cpp

    Implementation of envelope classes.

    ---------------------------------------------------------------------------

    Copyright (C) 2016, Cornfield Electronics, Inc.

    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0
    Unported License.

    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/

    Created by Bill Alessi & Mitch Altman.

   ---------------------------------------------------------------------------
*/

#include "Console_.h"
#include "Envelope.h"

/*----------------------------------------------------------------------------*
 *
 *  Name:  ADSR::charEv
 *
 *  Desc:  Process a character event.
 *
 *  Args:  code             - character to process
 *
 *  Memb: +attack           - attack time
 *        +flags.DONE       - control is ready to be triggered
 *        +decay            - decay time
 *        +phase            - current phase of envelope
 *        +sustain          - sustain level
 *        +relTime          - release time
 *        +value            - current output value 
 *
 *  Rets:  status           - true if character was handled
 *
 *----------------------------------------------------------------------------*/      

boolean ADSR::charEv( char key )
{
   switch ( key )
   {
      case chrTrigger:                 // trigger the envelope

         flags &= ~DONE;
         phase = attPhase;
         if ( attack )
            value = 0.0;
         else
         {
            --phase;
            if ( decay )
               value = 1.0;
            else
            {
               --phase;
               value = susLevel;
            }
         }
         break;

      case chrRelease:                 // release the envelope

         phase = relPhase;
         break;

      #ifdef INTERN_CONSOLE
      case 'a':                        // set attack time

         console.getByte( CONSTR("attack"), &this->attack );
         setAttack( attack );
         break;

      case 'd':                        // set decay time

         console.getByte( CONSTR("decay"), &this->decay );
         setDecay( decay );
         break;

      case 'r':                        // set release time

         console.getByte( CONSTR("release"), &this->relTime );
         setRelease( relTime );
         break;

      case 's':                        // set sustain level

         console.getByte( CONSTR("sustain"), &this->sustain );
         setSustain( sustain );
         break;

      #endif

      #ifdef CONSOLE_OUTPUT
      case chrInfo:                    // display object info to console

         super::charEv( chrInfo );
         console.infoByte( CONSTR("attack"), attack );
         console.infoByte( CONSTR("decay"), decay );
         console.infoByte( CONSTR("sustain"), sustain );
         console.infoByte( CONSTR("release"), relTime );
         break;
      #endif

      case '.':                        // mute
      case '<':                        // unMute

         value = 1.0;
         finish();
         super::charEv( key );
         break;

      case '!':                        // reset

         super::charEv( key );
         finish();
         setRelease(0);
         setSustain(255);
         setDecay(0);
         setAttack(0);
         setMute( false );             // enable envelope by default
         break;

      default:

         return super::charEv( key );
   }
   return true;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  ADSR::dynamics
 *
 *  Desc:  Perform a dynamic update.
 *
 *  Memb:  attStep          - change in attack per dynamic update
 *         decay            - decay time
 *         decStep          - change in decay per dynamic update
 *        +phase            - current phase of envelope
 *         relStep          - change in release per dynamic update
 *         relTime          - release time
 *         susLevel         - sustain level (0.0-1.0)
 *        +value            - current output value 
 *
 *----------------------------------------------------------------------------*/      

void ADSR::dynamics()
{
   switch ( phase )
   {
      case finPhase:

         return;         

      case attPhase:

         value += attStep;
         if ( value > 1.0 )
         {
            --phase;
            if ( decay )
               value = 1.0 - decStep;
            else
            {
               --phase;
               value = susLevel;
            }
         }
         break;

      case decPhase:

         value -= decStep;
         if ( value < susLevel )
         {
            --phase;
            value = susLevel;
         }
         break;

      case relPhase:

         if ( relTime )
         {
            value -= relStep;
            if ( value < 0.0 )
               value = 0.0;
            else
               break;
         }
         finish();
   }
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  ADSR::evHandler
 *
 *  Desc:  Handle an onboard event.
 *
 *  Args:  ev               - onboard event
 *
 *  Memb:  -- none -- 
 *
 *  Rets:  status           - true if the event was handled
 *
 *----------------------------------------------------------------------------*/      

boolean ADSR::evHandler( obEvent ev )
{
   if ( byte param = getScrollParm( ev ) )
   {
      byte val = ev.getPotVal();
      switch ( param )
      {
         case 1:  setAttack( val );   break;
         case 2:  setDecay( val );    break;
         case 3:  setSustain( val );  break;
         case 4:  setRelease( val );  break;
      }
     return true;
   }

   switch ( ev.type() )
   {
      case BUT0_PRESS:

         scrollUp();
         return true;

      case BUT1_PRESS:

         scrollDn();
         return true;

      default:

         return super::evHandler(ev);
   }
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  ADSR::exptime
 *
 *  Desc:  Given an 8-bit number indicating a relative duration of time,
 *         translate this into an absolution duration, as measured in  
 *         dynamic update periods.
 *
 *  Args:  time             - relative duration of time (0=none, 255=longest)
 *
 *  Rets:  num_of_updates   - absolute duration as measured in dynamic updates
 *
 *----------------------------------------------------------------------------*/

word ADSR::exptime( byte t )
{
   // expands t { 1..255 } pseudo-exponentially to { 1 .. 2411 }

   word nbufs = t;

   if ( t > 96 )
   {
      nbufs += ( t - 96 ) << 2;
      if ( t > 160 )
         nbufs += ( t - 160 ) << 4;
   }

   return nbufs;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  ADSR::finish
 *
 *  Desc:  Mark an envelope cycle as having been completed.
 *
 *  Memb: +flags.DONE       - control is ready to be triggered
 *        +phase            - current phase of envelope
 *
 *----------------------------------------------------------------------------*/      

void ADSR::finish()
{
   phase = finPhase;
   flags |= DONE;
}

#ifdef KEYBRD_MENUS

/*----------------------------------------------------------------------------*
 *
 *  Name:  ADSR::menu
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
 *           |   |   |   |   |   |   |   |   | ' |   | . |   | 
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |    ___     ___    |    ___     ___     ___    | 
 *           |     |       |     |     |       |       |     |
 *           |     |       |     |     |       |       |     |
 *           |  a  |   d   |  s  |  r  |   ~   |   <   |  !  |
 *           |     |       |     |     |       |       |     |
 *           |     |       |     |     |       |       |     |
 *           -------------------------------------------------
 *
 *----------------------------------------------------------------------------*/      

char ADSR::menu( key k )
{
   switch ( k.position() )
   {
      case  0: return 'a';
      case  2: return 'd';
      case  4: return 's';
      case  5: return 'r';
      default: return super::menu( k );
   }
}

#endif

#ifdef CONSOLE_OUTPUT
const char *ADSR::prompt()                     
{
   return CONSTR("envelope");
}
#endif

byte ADSR::getAttack()  { return attack; }
byte ADSR::getDecay()   { return decay; }
byte ADSR::getRelease() { return relTime; }
byte ADSR::getSustain() { return sustain; }

void ADSR::setAttack( byte val )
{
   attack = val;
   if ( val )
      attStep = 1.0 / exptime( val );
}

void ADSR::setDecay( byte val )
{
   decay = val;
   if ( val )
      decStep = (1.0 - susLevel) / exptime( val );
}

void ADSR::setRelease( byte val )
{
   relTime = val;
   if ( val )
      relStep = 1.0 / exptime( val );
}

void ADSR::setSustain( byte val )
{
   #define ONE_256TH  .00390625

   sustain = val;
   if ( val == 255 )
      susLevel = 1.0;
   else
      susLevel = (double )val * ONE_256TH;
   setDecay( decay );

   #undef ONE_256TH
}