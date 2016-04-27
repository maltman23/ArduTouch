/*
    ConKeyBrd.cpp  

    Implementation of the ConKeyBrd class.

    ---------------------------------------------------------------------------
 
    Copyright (C) 2016, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
   ---------------------------------------------------------------------------
*/

#include "Arduino.h"
#include "Console_.h"
#include "ConKeyBrd.h"

ConKeyBrd keybrd;

const char ConKeyBrd::keycode_[] PROGMEM = 
{ 
   'z', 's', 'x',
   'd', 'c', 'v',
   'g', 'b', 'h',
   'n', 'j', 'm', 
   ',' 
} ;

const char ConKeyBrd::letter_[] PROGMEM = 
{ 
   'C', 'C', 'D',
   'D', 'E', 'F',
   'F', 'G', 'G',
   'A', 'A', 'B', 
   'C' 
} ;

const char ConKeyBrd::adental_[] PROGMEM = 
{ 
   ' ', '#', ' ',
   '#', ' ', ' ',
   '#', ' ', '#',
   ' ', '#', ' ', 
   '+' 
} ;


ConKeyBrd::ConKeyBrd()
{
   flags  &= ~(ECHO|MENU);
   octave  = 4;
}

void ConKeyBrd::bumpOctave( boolean up )
{
   byte newOctave = getOctave();
   if ( up )
      ++newOctave;
   else if ( newOctave )
      -- newOctave;
   setOctave( newOctave );
}

void ConKeyBrd::butEv( but b )
{
   byte      butNum  = b.num();
   butEvType butType = b.type();

   switch ( butType )
   {
      case butTAP:               // bump keybrd up(but 1) - down(but 0) 1 octave 

         bumpOctave( butNum );
         console.newprompt();
         break;

      default:                 

         Mode::butEv( b );
   }
}

void ConKeyBrd::charEv( char code )
{
   if ( whichNote( code ) )            // if code maps to a key, create key event
   {
      console.postKey( position, octave );
      return;
   }

   if ( isDigit( code ) )
   {
      setOctave( code - '0' );
      return;
   }

   Mode::charEv( code );
}

void ConKeyBrd::info()
{
   console.rtab();
   console.print( (int )octave );
}

boolean ConKeyBrd::keyEv( key k )
{
#ifdef CONSOLE_OUTPUT

   // echo the key to console

   byte pos = k.position();
   console.print( (char )pgm_read_byte_near( letter_ + pos ) );
   console.print( (char )pgm_read_byte_near( adental_ + pos ) );
   console.newprompt();

#endif

   return false;                       // allow parent mode(s) to respond
}

char *ConKeyBrd::prompt()
{
   return CONSTR("kybd");
}

void ConKeyBrd::setOctave( byte o )
{
   if ( o > maxOctave ) o = maxOctave;
   if ( o < minOctave ) o = minOctave;
   octave = o;
}

boolean ConKeyBrd::whichNote( char code )
{
   for ( int i = 0; i < numNotes; i++ )
      if ( code == pgm_read_byte_near( keycode_ + i )  )
      {
         position = i;
         return true;
      }
   return false;
}
