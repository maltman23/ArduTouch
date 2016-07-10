/*
    Chnl.cpp  

    Implementation of the Chnl, ChnlSqnc classes.

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
#include "Chnl.h"
#include "System.h"

/* ---------------------------------- Chnl ---------------------------------- */

boolean Chnl::amFreqLatching()
{
   return ( latched && ! arithmetic );
}

boolean Chnl::charEv( char code )
{
   switch ( code )
   {
      case chrTrigger:              // perform a trigger

         flags |= TRIG;             // set flag (to be processed in dynamics())      
         if ( latched )             // trigger latched voice
            other->flags |= TRIG;                
         break;

      case 'f':                     // input a playback frequency

         Voice::charEv( 'f' );
         if ( latched ) propFreq();
         break;

      case 'k':                     // push console "piano keyboard" mode

         console.pushMode( &keybrd );
         break;

      case '+':                     // latch freq arithmetically

         if ( console.getDouble( CONSTR("+") ) )
            setFreqDiff( lastDouble );
         break;

      case '-':

         if ( console.getDouble( CONSTR("-") ) )
            setFreqDiff( -lastDouble );
         break;

      case '*':                     // latch freq geometrically

         if ( console.getDouble( CONSTR("*") ) )
            setFreqRatio( lastDouble );
         break;

      case '/':

         if ( console.getDouble( CONSTR("*") ) )
            setFreqRatio( 1.0 / lastDouble );
         break;

      case '!':                     // reset

         latched    = false;
         arithmetic = false;
         // fall thru to Voice handler

      default:

         return Voice::charEv( code );
   }
   return true;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Chnl::dynamics
 *
 *  Desc:  Update channel dynamics.
 *
 *  Args:  -- none --
 *
 *  Memb:   latched          - if true, channel frequency is latched
 *          pendNote         - pending note
 *
 *----------------------------------------------------------------------------*/      

void Chnl::dynamics()
{
   boolean newNote = pendNote.val ? true : false;

   Voice::dynamics();

   if ( newNote & latched )
      propFreq();
}

boolean Chnl::evHandler( obEvent ev )
{
   switch ( ev.type() )
   {
      case BUT1_DTAP:
               
         console.oneShotMenu();
         return true;

      default:

         return Voice::evHandler( ev );
   }
}

char Chnl::latchIcon()
{
#ifdef CONSOLE_OUTPUT
   return ( latched ? ( arithmetic ? '-' : '/' ) : ' ' );
#else
   return ' ';
#endif
}

void Chnl::onFocus( focus f )
{
   switch ( f )
   {
      case focusPUSH:
      case focusRESTORE:
         system::onLED( led );
   }
}

#ifdef CONSOLE_OUTPUT
const char* Chnl::prompt()
{
   return name;
}
#endif

void Chnl::propFreq()
{
   double x = osc->getFreq();
   if ( arithmetic )
      other->setFreq( x - freqOffset );
   else
      other->setFreq( x / freqOffset );
}

void Chnl::setFreqDiff( double x )
{
   freqOffset = x;
   setFreq( other->osc->getFreq() + freqOffset );
   other->freqOffset = -x;
   setArithmetic( true );
}

void Chnl::setFreqRatio( double x )
{
   freqOffset = x;
   setFreq( other->osc->getFreq() * freqOffset );
   other->freqOffset = 1.0 / x;
   setArithmetic( false );
}

void Chnl::setName( const char* name )
{
#ifdef CONSOLE_OUTPUT
   this->name = name;
#endif
}

void Chnl::setArithmetic( boolean state )
{
   arithmetic = state;
   other->arithmetic = state;
   setFreqLatch( true );
}

void Chnl::setFreqLatch( boolean state )
{
   latched = state;
   other->latched = state;
}

/* ------------------------------- ChnlSqnc --------------------------------- */

ChnlSqnc::ChnlSqnc()
{
   subject = this;
}

boolean ChnlSqnc::charEv( char code )
{
   #ifdef CONSOLE_OUTPUT
   if ( code == chrInfo )
   {
      Chnl::charEv( chrInfo );
      console.newline();
      StepSqnc::info();
      return true;
   }
   #endif

   if ( StepSqnc::_charEv( code ) )  
      return true;
   else
      return Chnl::charEv( code );
}

boolean ChnlSqnc::evHandler( obEvent ev )
{
   switch ( ev.type() )
   {
      case BUT1_TPRESS:                 // push step programmer

         system::blinkLED( led );
         StepSqnc::_charEv('S');       
         return true;

      default:

         return Chnl::evHandler( ev );
   }
}

void ChnlSqnc::output( char *buf )
{
   StepSqnc::cont();
   Chnl::output( buf );
}

#ifdef KEYBRD_MENUS
char ChnlSqnc::menu( key k )
{
   switch ( k.position() )
   {
      case  5: return 't';
      case  6: return '|';
      case  7: return '[';
      case  8: return ']';
      default: return Chnl::menu( k );
   }
}
#endif

