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

Mode::Mode() 
{ 
   flags = ECHO | MENU;          // echo char input; menu() pre-empts KEY_DOWN
}

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

boolean Mode::evHandler( obEvent ev )
{
   switch ( ev.type() )
   {
      case BUT0_DTAP:

         charEv( chrESC );
         return true;

      case META_OCTUP:

         upOctave();
         return true;

      case META_OCTDN:

         downOctave();
         return true;

      case META_ONESHOT:

         console.oneShotMenu();      // menu() pre-empts KEY_DOWN for next key
         return true;

      default:

         return false;              // "event not handled"
   }
}

void Mode::brief()
{
   #ifdef CONSOLE_OUTPUT
   console.print('[');
   charEv( chrBrief );
   console.romprint( CONSTR("] ") );
   #endif
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
char Mode::menu( key )
{
   return 0;                  // "no character was mapped"
}
#endif

#ifdef CONSOLE_OUTPUT
const char *Mode::prompt()
{
   return CONSTR("");
}
#endif


