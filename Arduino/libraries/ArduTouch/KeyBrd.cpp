/*
    KeyBrd.cpp  

    Implementation of the KeyBrd class.

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
#include "KeyBrd.h"
#include "System.h"

const char KeyBrd::keycode_[] PROGMEM = 
{ 
   'z', 's', 'x',
   'd', 'c', 'v',
   'g', 'b', 'h',
   'n', 'j', 'm', 
   ',' 
} ;

const char KeyBrd::letter_[] PROGMEM = 
{ 
   'C', 'C', 'D',
   'D', 'E', 'F',
   'F', 'G', 'G',
   'A', 'A', 'B', 
   'C' 
} ;

const char KeyBrd::adental_[] PROGMEM = 
{ 
   ' ', '#', ' ',
   '#', ' ', ' ',
   '#', ' ', '#',
   ' ', '#', ' ', 
   '+' 
} ;


/*----------------------------------------------------------------------------*
 *
 *  Name:  KeyBrd::charEv
 *
 *  Desc:  Process a character event.
 *
 *  Args:  code             - character to process
 *
 *  Memb: +octave           - current octave number 
 *
 *  Rets:  status           - true if character was handled
 *
 *  Note:  The following ascii characters are mapped to a musical octave,
 *         and create corresponding KEY_DOWN events:
 *
 *                      |   |   |   |   |   |   |   |  
 *                      | s | d |   | g | h | j |   |    
 *                      |   |   |   |   |   |   |   |     
 *                    ----------------------------------
 *                    |   |   |   |   |   |   |   |   |
 *                    | z | x | c | v | b | n | m | n |
 *                    |   |   |   |   |   |   |   |   | 
 *                    ----------------------------------
 *
 *----------------------------------------------------------------------------*/      

boolean KeyBrd::charEv( char code )
{
   #ifdef INTERN_CONSOLE

   // if character maps to a musical key, create a KEY_DOWN event

   for ( int i = 0; i < NumNotes; i++ )
      if ( code == pgm_read_byte_near( keycode_ + i )  )
      {
         console.postKeyDn( i, octave );
         return true;
      }

   // if character is a digit, set the octave to it

   if ( isDigit( code ) )
   {
      setOctave( code - '0' );
      return true;
   }

   #endif

   switch ( code )
   {
      #ifdef CONSOLE_OUTPUT

      case chrInfo:
   
         super::charEv( chrInfo );
         console.infoByte( CONSTR("octave"), octave );
         break;
   
      #endif

      case '!':

         octave = defOctave;

         // fall thru to superclass reset

      default:

         return super::charEv( code );
   }
   return true;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  KeyBrd::evHandler
 *
 *  Desc:  Handle an onboard event.
 *
 *  Args:  ev               - onboard event
 *
 *  Rets:  status           - true if the event was handled
 *
 *  Note:  Key events are echoed to the console and passed thru.
 *
 *----------------------------------------------------------------------------*/      

boolean KeyBrd::evHandler( obEvent ev )
{
   switch ( ev.type() )
   {
      case KEY_DOWN:                   // print key and then pass event thru

         #ifdef CONSOLE_OUTPUT
         {
            key k = ev.getKey();
            
            // echo the key name to console

            byte pos = k.position();
            console.print( (char )pgm_read_byte_near( letter_ + pos ) );
            console.print( (char )pgm_read_byte_near( adental_ + pos ) );
            console.newprompt();
         }
         #endif

         // fall thru to "return false";

      case BUT1_DTAP:

         return false;                 // intercept "one-shot menu"
      
      default:                 

         return super::evHandler( ev );
   }
}

void KeyBrd::downOctave()
{
   if ( octave > 0 ) setOctave( octave-1 );
}

void KeyBrd::upOctave()
{
   setOctave( octave+1 );
}

byte KeyBrd::getOctave()
{
   return octave;
}

void KeyBrd::setBotOct( byte o )
{
   if ( o <= MaxOctave ) botOctave = o;
}

void KeyBrd::setDefOct( byte o )
{
   if ( o <= MaxOctave ) defOctave = o;
}

void KeyBrd::setTopOct( byte o )
{
   if ( o <= MaxOctave ) topOctave = o;
}

void KeyBrd::setOctave( byte o )
{
   if ( o > topOctave ) o = topOctave;
   if ( o < botOctave ) o = botOctave;
   octave = o;
}


