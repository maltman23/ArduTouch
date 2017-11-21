/*
    Mode.cpp

    Implementation of the Mode class.

    ---------------------------------------------------------------------------
 
    Copyright (C) 2016, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
   ---------------------------------------------------------------------------
*/

#include "Console_.h"
#include "Mode.h"
#include "System.h"

/******************************************************************************
 *
 *                                Mode 
 *
 ******************************************************************************/

/*----------------------------------------------------------------------------*
 *
 *  Name:  Mode::charEv
 *
 *  Desc:  Process a character event.
 *
 *  Args:  code             - character to process
 *
 *  Rets:  status           - true if character was handled
 *
 *----------------------------------------------------------------------------*/

boolean Mode::charEv( char code )
{
   switch ( code )
   {
      #ifdef CONSOLE_OUTPUT

      case '?':

         inform();
         break;

      case chrBrief:

         console.romprint( prompt() );
         break;

      #endif

      case chrESC:

         console.popMode();
         break;

      default:

         return false;
   }
   return true;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Mode::evHandler
 *
 *  Desc:  Handle an onboard event.
 *
 *  Args:  ev               - onboard event
 *
 *  Rets:  status           - true if the event was handled
 *
 *----------------------------------------------------------------------------*/      

boolean Mode::evHandler( obEvent ev )
{
   switch ( ev.type() )
   {
      case BUT0_DTAP:

         charEv( chrESC );
         break;

      #ifdef KEYBRD_MENUS

      case BUT1_DTAP:

         console.oneShotMenu();     // menu() pre-empts KEY_DOWN for next key
         break;

      #endif

      default:

         return false;              // "event not handled"
   }
   return true;
}

void Mode::brief()
{
   #ifdef CONSOLE_OUTPUT
   console.print('[');
   charEv( chrBrief );
   console.romprint( CONSTR("] ") );
   #endif
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Mode::execute
 *
 *  Desc:  Execute a macro string, based in this mode. 
 *
 *  Args:  macStr           - ptr to macro string in ROM.
 *
 *----------------------------------------------------------------------------*/
       
void Mode::execute( const char *m )
{
   console.exeIn( m, this );
}

void Mode::inform()
{
   // define USAGE_INFO if either MONITOR_CPU or MONITOR_RAM is defined

   #ifdef MONITOR_CPU               
      #define USAGE_INFO
   #endif
   #ifdef MONITOR_RAM               
      #define USAGE_INFO
   #endif

   #ifdef CONSOLE_OUTPUT

      #ifdef USAGE_INFO

         console.newlntab();

         #ifdef MONITOR_CPU
            console.infoByte( CONSTR("cpu"), cpu );
         #endif

         #ifdef MONITOR_RAM
            console.infoInt( CONSTR("freeRAM"), freeRam() );
         #endif

      #endif

      console.newline();
      charEv( chrInfo );
      console.newprompt();

   #endif

   #undef USAGE_INFO
}

#ifdef KEYBRD_MENUS

/*----------------------------------------------------------------------------*
 *
 *  Name:  Mode::menu
 *
 *  Desc:  Given a key, return a character (to be processed via charEv()). 
 *
 *  Args:  k                - key
 *
 *  Rets:  c                - character (0 means "no character")
 *
 *  Note:  If a sketch is compiled with KEYBRD_MENUS defined, then this method 
 *         can be used to map the onboard keys to characters which the system 
 *         will automatically feed to the charEv() method.
 *
 *         This method is only called by the system if the MENU flag in this
 *         object is set, or if the keyboard is in a "oneShot menu selection" 
 *         state.
 *
 *----------------------------------------------------------------------------*/      

char Mode::menu( key )
{
   return 0;                  // "no character"
}

#endif

#ifdef CONSOLE_OUTPUT
const char *Mode::prompt()
{
   return CONSTR("");
}
#endif


