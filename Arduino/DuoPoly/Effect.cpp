/*
    Effect.cpp  

    Implementation of the Effect and derived classes.

    ---------------------------------------------------------------------------
 
    Copyright (C) 2016, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
   ---------------------------------------------------------------------------
*/

#include "types.h"
#include "Effect.h"
#include "Console_.h"

                        /*--------------------
                              EffectsLoop
                         --------------------*/


void EffectsLoop::append( Effect *x )     // append an effect to the effects loop
{
   if ( loop )
   {
      Effect *e = loop;
      while ( e->next )
         e = e->next;
      e->next = x;
   }
   else
      loop = x;
}

boolean EffectsLoop::charEv( char code )
{
   boolean found = false;
   Effect *e = loop;

   #ifdef INTERN_CONSOLE
   if ( code >= '0' && code <= '8' )  // code is a 0-based ordinal
   {
      signed char nth = code - '0';
      while ( e && nth > 0 )
      {
         e = e->next;
         --nth;
      }
      if ( e ) 
      {
         found = true;
         console.pushMode( e );
      }
   }
   else                               // code may be a shortcut letter
   {
      while ( e )
      {
         if ( e->shortcut == code )
         {
            found = true;
            console.pushMode( e );
            break;
         }
         e = e->next;
      }
   }
   #endif

   if ( found ) 
      return true;
   else
   {
      switch ( code )
      {
         case '.':
         case '<':
         case '!':

         {
            Effect *e = loop;
            while ( e )
            {
               e->charEv( code );
               e = e->next;
            }
            break;
         }

         #ifdef CONSOLE_OUTPUT
         case chrBrief:
         {
            console.romprint( CONSTR(" effects ") );
            Effect *e = loop;
            while ( e )
            {
               e->brief();
               e = e->next;
            }
            break;
         }
         #endif

         #ifdef CONSOLE_OUTPUT
         case chrInfo:
         {
            console.rtab();
            Effect *e = loop;
            while ( e )
            {
               console.print( e->shortcut );
               console.print(':');
               e->brief();
               console.space();
               e = e->next;
            }
            break;
         }
         #endif

         default:

            return Mode::charEv( code );
      }
      return true;
   }

}

#ifdef KEYBRD_MENUS
char EffectsLoop::menu( key k )
{
   switch ( k.position() )
   {
      case 11: return '!';
      case 10: return '.';
      case  9: return '<';
      default: return ( '0' + k.position() );
   }
}
#endif

void EffectsLoop::process( char *buf )
{
   Effect *e = loop;
   while ( e )
   {
      if ( ! e->amMute() ) 
         e->process( buf );
      e = e->next;
   }
}

#ifdef CONSOLE_OUTPUT
const char *EffectsLoop::prompt() 
{ 
   return CONSTR("effects"); 
}
#endif


