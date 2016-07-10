/*
    Onboard.h  

    Declaration of Onboard event types.

    ---------------------------------------------------------------------------
 
    Copyright (C) 2016, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
   ---------------------------------------------------------------------------
*/

#ifndef ONBOARD_H_INCLUDED
#define ONBOARD_H_INCLUDED

#include "Arduino.h"

/* --------------------------------------------------------------------------

      Onboard events are grouped into 3 genuses: 
      
         1: those originating from the musical keyboard
         2: those originating from the buttons
       ( 3: those originating from the pots )

   -------------------------------------------------------------------------- */

typedef byte evGenus ; enum
{ 
   evKEY,            // a key event
   evBUT,            // a button event
   evPOT,            // a pot event
   evMETA,           // a meta event
   evUNDEF           // an undefined event
} ;

/* --------------------------------------------------------------------------

      These are populated by specific types:

   -------------------------------------------------------------------------- */

typedef byte evType ; enum
{ 
   KEY_DOWN,         // a key is touched
   KEY_UP,           // a key is released
   BUT0_PRESS,       // top button is pressed
   BUT0_TAP,         // top button is tapped
   BUT0_DTAP,        // top button is double-tapped
   BUT0_TPRESS,      // top button is tapped-then-pressed
   BUT1_PRESS,       // bot button is pressed
   BUT1_TAP,         // bot button is tapped
   BUT1_DTAP,        // bot button is double-tapped
   BUT1_TPRESS,      // bot button is tapped-then-pressed
   POT0,             // top pot is moved
   POT1,             // bot pot is moved
   META_OCTUP,       // octave up
   META_OCTDN,       // octave down
   META_ONESHOT      // initiate one-shot menu selection
} ;

/* --------------------------------------------------------------------------

      The following actions can be applied to a button (and coincide with
      the enumerated orderings within evType):

   -------------------------------------------------------------------------- */

typedef byte butAction ; enum 
{ 
   butPRESS, 
   butTAP,         
   butDTAP,
   butTPRESS 
} ;

/* --------------------------------------------------------------------------

      The key struct denotes a key position within an octave.

   -------------------------------------------------------------------------- */

struct key                             
{
   byte val;

   key()
   {
      val = 0;
   }

   key( byte v )
   {
      val = v;
   }

   key( byte p, byte o )
   {
      val = p + (o << 4);
   }

   byte octave()
   {
      return val >> 4;
   }

   byte position()
   {
      return val & 0b00001111;
   }

   void set( byte p, byte o )
   {
      val = p + (o << 4);
   }

} ;


/* --------------------------------------------------------------------------

      The obEvent struct denotes an onboard event. 

      It contains a type field in the upper nibble of the high byte.

      Type-specific data is stored in the low byte and in the low nibble 
      of the high byte.

   -------------------------------------------------------------------------- */

typedef struct 
{
   byte lo;
   byte hi;

   evGenus genus()
   {
      evType t = type();
      if ( t >= KEY_DOWN && t <= KEY_UP )
         return evKEY;
      else if ( t >= BUT0_PRESS && t <= BUT1_TPRESS )
         return evBUT;
      else if ( t >= POT0 && t <= POT1 )
         return evPOT;
      else if ( t >= META_OCTUP && t <= META_ONESHOT )
         return evMETA;
      else
         return evUNDEF;
   }

   byte get()
   {
      return lo;
   }

   byte getPotVal()
   {
      return lo;
   }

   key getKey()
   {
      return (key )lo;
   }

   void setKeyDn( key k )
   {
      lo = k.val;
      hi = 0;
      setType( KEY_DOWN );
   }

   void setKeyUp( key k )
   {
      lo = k.val;
      hi = 0;
      setType( KEY_UP );
   }

   void setPotVal( byte x )
   {
      lo = x;
      hi &= 0b11110000;
   }

   void setType( evType t )
   {
      hi &= 0b00001111;
      hi |= ((byte )t << 4);
   }

   evType type()
   {
      return (hi >> 4);
   }

} obEvent ;


#endif   // ifndef ONBOARD_H_INCLUDED