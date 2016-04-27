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

void EffectsLoop::charEv( char code )
{
   boolean found = false;
   Effect *e = loop;

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

   if ( ! found ) 
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
         default:
            Mode::charEv( code );
      }
   }

}

void EffectsLoop::info()
{
   console.rtab();
   Effect *e = loop;
   while ( e )
   {
      console.print( e->shortcut );
      console.print( ": " );
      console.print( e->prompt() );
      console.space(2);
      e = e->next;
   }
}

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

char *EffectsLoop::prompt() 
{ 
   return CONSTR("effects"); 
}


