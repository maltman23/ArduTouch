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

/******************************************************************************
 *
 *                             EffectsLoop 
 *
 ******************************************************************************/

/*----------------------------------------------------------------------------*
 *
 *  Name:  EffectsLoop::append
 *
 *  Desc:  Append an effect to the effects loop.
 *
 *  Args:  effect           - ptr to effect to append
 *
 *  Memb:  loop             - pts to 1st effect in loop
 *
 *----------------------------------------------------------------------------*/      

void EffectsLoop::append( Effect *x )     
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

/*----------------------------------------------------------------------------*
 *
 *  Name:  EffectsLoop::clear
 *
 *  Desc:  Remove all effects from the effects loop.
 *
 *  Memb:  loop             - pts to 1st effect in loop
 *
 *----------------------------------------------------------------------------*/      

void EffectsLoop::clear()
{
   loop = NULL;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  EffectsLoop::dynamics
 *
 *  Desc:  Perform a dynamic update on all effects in the effects loop.
 *
 *  Memb:  loop             - pts to 1st effect in loop
 *
 *----------------------------------------------------------------------------*/      

void EffectsLoop::dynamics()
{
   Effect *e = loop;
   while ( e )
   {
      e->dynamics();
      e = e->next;
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

/*----------------------------------------------------------------------------*
 *
 *  Name:  EffectsLoop::process
 *
 *  Desc:  Process an audio buffer through the effects loop.
 *
 *  Args:  buf              - ptr to audio buffer
 *
 *  Memb:  loop             - pts to 1st effect in loop
 *
 *----------------------------------------------------------------------------*/      

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

/*----------------------------------------------------------------------------*
 *
 *  Name:  EffectsLoop::release
 *
 *  Desc:  Perform a trigger release on all effects in the effects loop.
 *
 *  Memb:  loop             - pts to 1st effect in loop
 *
 *----------------------------------------------------------------------------*/      

void EffectsLoop::release()
{
   Effect *e = loop;
   while ( e )
   {
      e->release();
      e = e->next;
   }
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  EffectsLoop::trigger
 *
 *  Desc:  Trigger all effects in the effects loop.
 *
 *  Memb:  loop             - pts to 1st effect in loop
 *
 *----------------------------------------------------------------------------*/      

void EffectsLoop::trigger()
{
   Effect *e = loop;
   while ( e )
   {
      e->trigger();
      e = e->next;
   }
}


