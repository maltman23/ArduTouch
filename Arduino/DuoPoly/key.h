/*
    key.h  

    Declaration of the key data type.

    ---------------------------------------------------------------------------
 
    Copyright (C) 2016, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
   ---------------------------------------------------------------------------
*/

#ifndef KEY_H_INCLUDED
#define KEY_H_INCLUDED

#include "Arduino.h"

struct key                             
{
   byte val;

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

#endif   // ifndef KEY_H_INCLUDED