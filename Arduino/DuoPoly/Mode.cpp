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

Mode::Mode() 
{ 
   flags = ECHO | MENU;          // echo char input; menu() pre-empts keyEv()
}

void Mode::butEv( but b )
{
   if ( b.num() == 0 && b.type() == butDOUBLE_TAP ) 
      charEv( chrESC );
}

void Mode::charEv( char code )
{
   switch ( code )
   {
      case '?':

         newline_info_prompt();
         break;

      case chrESC:

         console.popMode();
         break;
   }
}

boolean Mode::keyEv( key )
{
   return false;              // "event not handled"
}

char Mode::menu( key )
{
   return 0;                  // "no character"
}

void Mode::newline_info_prompt()
{
   console.newline();
   info();
   console.newprompt();
}

char *Mode::prompt()
{
   return "";
}


