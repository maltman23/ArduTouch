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

enum frameNum                          // enumerates all possible frame #'s
{
   FRAME00 =  0,                       // LED0 off    LED1 off
   FRAME01 =  2,                       // LED0 off    LED1 on
   FRAME02 =  4,                       // LED0 off    LED1 blink
   FRAME10 =  8,                       // LED0 on     LED1 off
   FRAME11 = 10,                       // LED0 on     LED1 on
   FRAME12 = 12,                       // LED0 on     LED1 blink
   FRAME20 = 16,                       // LED0 blink  LED1 off
   FRAME21 = 18,                       // LED0 blink  LED1 on
   FRAME22 = 20                        // LED0 blink  LED1 blink
} ;

/* --------------------------------------------------------------------------

      Onboard events are grouped into 3 genuses: 
      
         1: those originating from the musical keyboard
         2: those originating from the buttons
         3: those originating from the pots 

   -------------------------------------------------------------------------- */

typedef byte evGenus ; enum
{ 
   evKEY,            // a key event
   evBUT,            // a button event
   evPOT,            // a pot event
   evUNDEF           // an undefined event
} ;

/* --------------------------------------------------------------------------

      These are populated by specific types:

   -------------------------------------------------------------------------- */

typedef byte evType ; enum
{ 
   KEY_DOWN,         // a key is touched
   KEY_UP,           // a key is released
   BUT0_PRESS,       // left button is pressed
   BUT0_TAP,         // left button is tapped
   BUT0_DTAP,        // left button is double-tapped
   BUT0_TPRESS,      // left button is tapped-then-pressed
   BUT1_PRESS,       // right button is pressed
   BUT1_TAP,         // right button is tapped
   BUT1_DTAP,        // right button is double-tapped
   BUT1_TPRESS,      // right button is tapped-then-pressed
   POT0,             // top pot is moved
   POT1,             // bot pot is moved

   // "extended" pot events: pots are moved while in a particular frame 

   POT0_F00 = FRAME00+POT0,
   POT1_F00 = FRAME00+POT1,
   POT0_F01 = FRAME01+POT0,
   POT1_F01 = FRAME01+POT1,
   POT0_F02 = FRAME02+POT0,
   POT1_F02 = FRAME02+POT1,
   POT0_F10 = FRAME10+POT0,
   POT1_F10 = FRAME10+POT1,
   POT0_F11 = FRAME11+POT0,
   POT1_F11 = FRAME11+POT1,
   POT0_F12 = FRAME12+POT0,
   POT1_F12 = FRAME12+POT1,
   POT0_F20 = FRAME20+POT0,
   POT1_F20 = FRAME20+POT1,
   POT0_F21 = FRAME21+POT0,
   POT1_F21 = FRAME21+POT1,
   POT0_F22 = FRAME22+POT0,
   POT1_F22 = FRAME22+POT1

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

#define NULL_KEY 255

struct key                             
{
   byte val;

   key()
   {
      reset();
   }

   key( byte v )
   {
      val = v;
   }

   key( byte p, byte o )
   {
      val = p + (o << 4);
   }

   inline boolean operator== (key other) 
   {
      return (val == other.val);
   }

   boolean eq( key other ) 
   {
      return (val == other.val);
   }

   boolean null()
   {
      return (val == NULL_KEY);
   }
   
   byte octave()
   {
      return val >> 4;
   }

   byte physkey()                   // return physical key #
   {
      // on the ArduTouch, physKey() == position()
      // if this code is ported to a board with more than 1 physical octave
      // the key struct will have to be expanded to include physical key field
      return val & 0b00001111;      
   }

   byte position()
   {
      return val & 0b00001111;
   }

   void reset()
   {
      val = NULL_KEY;
   }

   void set( byte p, byte o )
   {
      val = p + (o << 4);
   }

   void setOctave( byte o )
   {
      val = position() + (o << 4);
   }

   void transpose( char xpose );

} ;


/* --------------------------------------------------------------------------

      The obEvent struct denotes an onboard event. 

      It contains a type field in the lower 5 bits of the high byte.

      Type-specific data is stored in the low byte and in the upper 3 bits 
      of the high byte.

   -------------------------------------------------------------------------- */

struct obEvent
{
   byte lo;
   byte hi;

   #define OB_OCTAVE 0b00100000    // if hi.OB_OCTAVE set, key has octave info

   evGenus genus()
   {
      evType t = type();
      if ( t <= KEY_UP )
         return evKEY;
      else if ( t <= BUT1_TPRESS )
         return evBUT;
      else if ( t <= POT1_F22 )
         return evPOT;
      else
         return evUNDEF;
   }

   bool amPot()
   {
      evType t = type();
      return ( t >= POT0_F00 && t <= POT1_F22 );
   }

   void clean()
   {
      hi = lo = 0;
   }

   byte get()
   {
      return lo;
   }

   byte getPotVal()             // returns a value between 0 and 255
   {
      return lo;
   }

   byte getPotVal128()          // returns a value between 0 and 128
   {
      word result = lo + 1;
      return result>>1;
   }

   key getKey()
   {
      return (key )lo;
   }

   void setOctave( byte o )
   {
      lo &= 0b00001111;
      lo |= o << 4;
   }

   boolean octOn()
   {
      return (hi & OB_OCTAVE);
   }

   void setOctOff()
   {
      hi &= ~OB_OCTAVE;
   }

   void setOctOn()
   {
      hi |= OB_OCTAVE;
   }

   void setKeyDn( key k )
   {
      lo = k.val;
      setType( KEY_DOWN );
   }

   void setKeyUp( key k )
   {
      lo = k.val;
      setType( KEY_UP );
   }

   void setPotVal( byte x )
   {
      lo = x;
   }

   void setType( evType t )
   {
      hi &= 0b11100000;
      hi |= (byte )t;
   }

   void stripFrame()
   {
      setType( POT0 + ( type() & 0b01 ) );
   }

   evType type()
   {
      return hi & 0b00011111;
   }

   #undef OB_OCTAVE

} ;


#endif   // ifndef ONBOARD_H_INCLUDED