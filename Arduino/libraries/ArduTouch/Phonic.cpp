/*
    Phonic.cpp  

    Implementation of the Phonic class.

    ---------------------------------------------------------------------------
 
    Copyright (C) 2016, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
   ---------------------------------------------------------------------------
*/


#include "Console_.h"                        // declarations for console module
#include "Phonic.h"

boolean Phonic::mute_at_reset;               // object should be muted at reset


/*----------------------------------------------------------------------------*
 *
 *  Name:  Phonic::charEv
 *
 *  Desc:  Process a character event.
 *
 *  Args:  code             - character to process
 *
 *  Memb: +vol              - volume level 
 *
 *----------------------------------------------------------------------------*/      

boolean Phonic::charEv( char code )    
{
   switch ( code )
   {
      #ifdef INTERN_CONSOLE
      case 'v':                     // input a volume level

         console.getByte( CONSTR("vol"), &this->vol );
         setVol( vol );
         break;
      #endif

      #ifdef CONSOLE_OUTPUT
      case chrInfo:                 // display object info to console

         Control::charEv( chrInfo );
         console.infoByte( CONSTR("vol"), vol );
         break;
      #endif

      case '!':                     // reset

         Control::charEv('!');      // perform generic reset 1st
         setVol( 255 );             // full volume
         setMute( false );          // by definition Phonic not muted at reset   
         break;                     

      default:

         return Control::charEv( code );
   }
   return true;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Phonic::evHandler
 *
 *  Desc:  Handle an onboard event.
 *
 *  Args:  ev               - onboard event
 *
 *  Rets:  status           - true if the event was handled
 *
 *----------------------------------------------------------------------------*/      

boolean Phonic::evHandler( obEvent ev )
{
   switch ( ev.type() )
   {
      case POT0:       

         setVol( ev.getPotVal() );
         return true;

      default:

         return Control::evHandler( ev );
   }
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Phonic::setVol
 *
 *  Desc:  Set the volume level.
 *
 *  Args:  vol              - volume level
 *
 *  Memb: +vol              - volume level (0:255)
 *
 *----------------------------------------------------------------------------*/      

void Phonic::setVol( byte x )
{
   vol = x;
}

