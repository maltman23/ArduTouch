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
#include "Tuning.h"

/* ---------------------------------- Chnl ---------------------------------- */

Chnl::Chnl()
{
   flags &= ~MENU;            // do not use menu by default
}

boolean Chnl::amFreqLatching()
{
   return ( latched && ! arithmetic );
}

void Chnl::butEv( but b )
{
   if ( b.num() == 1 && b.type() == butDOUBLE_TAP )
      flags |= ONESHOT|MENU;
   else
      Voice::butEv( b );
}

void Chnl::charEv( char code )
{
   switch ( code )
   {
      case 'f':               // input a playback frequency

         Voice::charEv( 'f' );
         propFreq( getFreq() );
         break;

      case 'k':               // push console "piano keyboard" mode

         console.pushMode( &keybrd );
         break;

      case '+':               // latch freq arithmetically
      case '-':

         if ( promptDouble( code ) )
            setFreqDiff( code == '+' ? lastDouble : -lastDouble );
         break;

      case '*':               // latch freq geometrically
      case '/':

         if ( promptDouble( code ) )
            setFreqRatio( code == '*' ? lastDouble : 1.0/lastDouble );
         break;

      case '!':               // reset

         latched    = false;
         arithmetic = false;
         // fall thru to Voice handler

      default:

         Voice::charEv( code );
   }
}

boolean Chnl::keyEv( key k )
{
   propFreq( masterTuning->pitch( k ) );
   trigger();
   if ( latched ) other->trigger();
   return true;
}

char Chnl::latchIcon()
{
#ifdef CONSOLE_OUTPUT
   return ( latched ? ( arithmetic ? '-' : '/' ) : ' ' );
#else
   return ' ';
#endif
}

char Chnl::menu( key k )
{
   switch ( k.position() )
   {
      case  0: return 'v';
      case  1: return 'V';
      case  2: return 'w';
      case  3: return 'T';
      case  4: return 'e';
      default: return Control::menu( k );
   }
}

char* Chnl::prompt()
{
   return name;
}

boolean Chnl::promptDouble( char chrPrompt )
{
   static char strPrompt[2];
   strPrompt[0] = chrPrompt;
   return console.getDouble( strPrompt );
}

void Chnl::propFreq( double x )
{
   setFreq( x );
   if ( latched )
   {
      if ( arithmetic )
         other->setFreq( x - freqOffset );
      else
         other->setFreq( x / freqOffset );
   }
}

void Chnl::setFreqDiff( double x )
{
   freqOffset = x;
   setFreq( other->getFreq() + freqOffset );
   other->freqOffset = -x;
   setArithmetic( true );
}

void Chnl::setFreqRatio( double x )
{
   freqOffset = x;
   setFreq( other->getFreq() * freqOffset );
   other->freqOffset = 1.0 / x;
   setArithmetic( false );
}

void Chnl::setName( char* name )
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

void ChnlSqnc::butEv( but b )
{
   byte      butNum  = b.num();
   butEvType butType = b.type();

   if ( butNum == 1 && butType == butPRESS )

      StepSqnc::_charEv('S');             // push step programmer

   else

      Chnl::butEv( b );
}

void ChnlSqnc::charEv( char code )
{
   if ( ! StepSqnc::_charEv( code ) )  
      Chnl::charEv( code );
}

void ChnlSqnc::generate( char *buf )
{
   StepSqnc::cont();
   Chnl::generate( buf );
}

void ChnlSqnc::info()
{
   Chnl::info();
   console.newline();
   StepSqnc::info();
}

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


