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
#include "System.h"

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
   flags &= ~(ECHO|MENU);
   flags |= PLAYTHRU;
}

boolean ConKeyBrd::charEv( char code )
{
   #ifdef INTERN_CONSOLE
   if ( whichNote( code ) )            // if code maps to a key, create key event
   {
      console.postKeyDn( position, system::getOctave() );
      return true;
   }

   if ( isDigit( code ) )
   {
      system::setOctave( code - '0' );
      return true;
   }
   #endif

   #ifdef CONSOLE_OUTPUT
   if ( code == chrInfo )
   {
      console.rtab();
      console.print( (int )system::getOctave() );
      return true;
   }
   else
   #endif
      return Mode::charEv( code );
}

boolean ConKeyBrd::evHandler( obEvent ev )
{
   switch ( ev.type() )
   {
      case KEY_DOWN:

         #ifdef CONSOLE_OUTPUT
         {
            key k = ev.getKey();
            
            // echo the key to console

            byte pos = k.position();
            console.print( (char )pgm_read_byte_near( letter_ + pos ) );
            console.print( (char )pgm_read_byte_near( adental_ + pos ) );
            console.newprompt();
         }
         #endif

         return false;

      case META_ONESHOT:

         return false;
      
      default:                 

         return Mode::evHandler( ev );
   }
}

#ifdef CONSOLE_OUTPUT
const char *ConKeyBrd::prompt()
{
   return CONSTR("kybd");
}
#endif

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
