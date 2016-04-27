/*
    Control.cpp

    Implementation of the Control class.

    ---------------------------------------------------------------------------
 
    Copyright (C) 2016, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
   ---------------------------------------------------------------------------
*/

#include "Control.h"
#include "Console_.h"

void Control::charEv( char code )
{
   switch ( code )
   {
      case '<':                  // unmute object

         flags &= ~MUTE;
         break;

      case '.':                  // mute object

         flags |= MUTE;
         break;

      case '!':                  // reset object state

         setMute( true );
         break;

      default:

         Mode::charEv( code );
   }
}

void Control::info()
{
   console.rtab();
   console.print( (char *)(amMute() ? ". " : "< ") );
}

char Control::menu( key k )
{
   switch ( k.position() )
   {
      case 11: return '!';
      case 10: return '.';
      case  9: return '<';
      default: return 0;
   }
}

void Control::reset()
{
   charEv('!');
}

void Control::setMute( boolean mute )
{
   charEv( mute ? '.' : '<' );
}

void Control::trigger()
{
   charEv( cmdTrigger );
}


