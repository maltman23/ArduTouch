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

Control::Control()
{
   flags |= PLAYTHRU;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Control::charEv
 *
 *  Desc:  Process a character event.
 *
 *  Args:  code             - character to process
 *
 *  Rets:  status           - if true, event was handled
 *
 *  Memb: +flags.MUTE       - if set, control is muted 
 *
 *----------------------------------------------------------------------------*/      

boolean Control::charEv( char code )
{
   switch ( code )
   {
      case '<':                  // unmute control

         flags &= ~MUTE;
         break;

      case '.':                  // mute control

         flags |= MUTE;
         break;

      case '!':                  // reset control

         setMute( true );        // by definition, control is mute at reset   
         break;

      #ifdef CONSOLE_OUTPUT
      case chrBrief:             // display control brief to console

         Mode::charEv( chrBrief );
         console.space();
         console.print( (char)(amMute() ? '.' : '<') );
         break;
      #endif

      #ifdef CONSOLE_OUTPUT
      case chrInfo:              // display control info to console

         console.rtab();
         console.print( (char)(amMute() ? '.' : '<') );
         console.space();
         break;
      #endif

      default:

         return Mode::charEv( code );
   }
   return true;
}

#ifdef KEYBRD_MENUS
char Control::menu( key k )
{
   switch ( k.position() )
   {
      case  9: return '<';
      case 10: return '.';
      case 11: return '!';
      default: return 0;
   }
}
#endif

void Control::reset()
{
   charEv('!');
}

void Control::setMute( boolean mute )
{
   charEv( mute ? '.' : '<' );
}

boolean TControl::charEv( char code )
{
   switch ( code )
   {
      #ifdef INTERN_CONSOLE
      case '~':                        // set legato triggering

         flags   |= LEGATO;            
         break;

      case '\'':                       // set stacato triggering

         flags   &= ~LEGATO;           
         break;
      #endif

      #ifdef CONSOLE_OUTPUT
      case chrBrief:

         Control::charEv( chrBrief );
         console.print( ( flags & LEGATO ) ? '~' : '\'' );
         break;

      case chrInfo:                    // display object info to console

         Control::charEv( chrInfo );
         console.print( ( flags & LEGATO ) ? '~' : '\'' );
         console.space();
         break;
      #endif

      case '!':                        // reset

         flags &= ~LEGATO;             // "always trigger"

         // fall thru to parent 

      default:

         return Control::charEv( code );
   }
   return true;
}

#ifdef KEYBRD_MENUS
char TControl::menu( key k )
{
   switch ( k.position() )
   {
      case  7: return '~';
      case  8: return '\'';
      default: return Control::menu( k );
   }
}
#endif

boolean TControl::ready()
{
   return true;
}

void TControl::trigger()
{
   if ( ! amMute() )
   {
      if ( ready() )
         charEv( chrTrigger );
      else
         dynamics();
   }
}


