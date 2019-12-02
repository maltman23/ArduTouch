/*
    Pan.cpp  

    Implementation of the Panning control.

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
#include "Pan.h"

/******************************************************************************
 *       
 *                                PanControl
 *
 ******************************************************************************/

/*----------------------------------------------------------------------------*
 *
 *  Name:  PanControl::calcExtent
 *
 *  Desc:  Compute the panning extent.
 *
 *  Memb: +center           - panning center
 *        +radius           - panning radius
 *         pinned           - if true, pan is centered around restPos
 *         restPos          - position when at rest
 *
 *----------------------------------------------------------------------------*/      

void PanControl::calcExtent()
{
   center = pinned ? restPos : 128;
   radius = center < 128 ? center : 255-center;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  PanControl::charEv
 *
 *  Desc:  Process a character event.
 *
 *  Args:  code             - character to process
 *
 *  Ext:  +*panPos          - current pan position
 *
 *  Memb:  pinned           - if true, pan is centered around restPos
 *         restPos          - position when at rest
 *
 *  Rets:  status           - true if character was handled
 *
 *----------------------------------------------------------------------------*/      

boolean PanControl::charEv( char code )
{
   switch( code )
   {
      #ifdef INTERN_CONSOLE               // compile cases needed by macros

      case 'p':                           // pin panning center to rest position

         setPinned( true );
         break;

      case 'r':                           // set rest position
      case 'P':
      {
         byte newRestPos;
         if ( console.getByte( CONSTR("restPos"), &newRestPos ) )
            setRestPos( newRestPos );
         break;
      }
      case 'u':                           // unpin panning center from rest position

         setPinned( false );
         break;

      #endif

      case '.':                           // mute panning effect

         super::charEv( code );
         *panPos = restPos;
         break;

      case '!':                           // reset panning effect

         super::charEv( code );
         setSigned( true );
         setPinned( false );
         setDepth( 128 );
         break;

      #ifdef CONSOLE_OUTPUT   // compile cases that display to console 

      case chrInfo:

         super::charEv( chrInfo );
         console.newlntab();
         console.infoByte( CONSTR("restPos"), restPos );
         console.print('.');
         if ( ! pinned )
            console.romprint( CONSTR("un") );
         console.romprint( CONSTR("pinned.") );
         break;

      #endif

      default:

         return super::charEv( code );
   }
   return true;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  PanControl::evaluate
 *
 *  Desc:  Compute the pan position given current osc and pan parameters.
 *
 *  Ext:  +*panPos          - current pan position
 *
 *  Memb:  center           - panning center
 *         radius           - panning radius
 *
 *----------------------------------------------------------------------------*/      

void PanControl::evaluate()  
{
   super::evaluate();
   *panPos = center + (char )((double )radius*value);
}

#ifdef KEYBRD_MENUS

/*----------------------------------------------------------------------------*
 *
 *  Name:  PanControl::menu
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
 *         object is set (in the ::flags byte inherited from Mode), or if the
 *         keyboard is in a "oneShot menu selection" state.
 *
 *----------------------------------------------------------------------------*/      

char PanControl::menu( key k )
{
   switch ( k.position() )
   {
      case  1: return 'p';                   // pin panning center to rest pos
      case  3: return 'u';                   // unpin panning center from rest pos
      case  4: return 'r';                   // set rest position
      default: return super::menu( k );
   }
}

#endif

/*----------------------------------------------------------------------------*
 *
 *  Name:  PanControl::setPinned
 *
 *  Desc:  Pin or unpin the panning center to the rest position.
 *
 *  Args:  pinned           - if true, pin panning center to the rest position.
 *
 *  Memb:  pinned           - if true, pan is centered around restPos
 *
 *  Note:  If pinning == false, then panning center is 128, regardless of what
 *         the rest position is. 
 *
 *----------------------------------------------------------------------------*/      

void PanControl::setPinned( bool pinned )
{
   this->pinned = pinned;
   calcExtent();
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  PanControl::setRestPos
 *
 *  Desc:  Set the panning position when at rest.
 *
 *  Args:  restPos          - panning position when at rest
 *
 *  Ext:  +*panPos          - current pan position
 *
 *  Memb: +restPos          - position when at rest
 *
 *  Note: The rest position is relevant in 2 cases:
 *
 *    1) Panning is muted
 *    2) Panning is unmuted, but pinned ( restPos is panning center )
 *
 *----------------------------------------------------------------------------*/      

void PanControl::setRestPos( byte restPos )
{
   this->restPos = restPos;
   *panPos       = restPos;
   calcExtent();
}




