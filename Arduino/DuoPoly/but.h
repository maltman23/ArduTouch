/*
    but.h  

    Declaration of the button data type.

    ---------------------------------------------------------------------------
 
    Copyright (C) 2016, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
   ---------------------------------------------------------------------------
*/

#ifndef BUT_H_INCLUDED
#define BUT_H_INCLUDED

#include "Arduino.h"

enum butEvType { butPRESS = 01, butTAP = 02, butDOUBLE_TAP = 04 };

struct but                             
{
   byte val;

   but( byte n, butEvType typ )
   {
      val = ( n ? 1 : 0 ) + ( (byte )typ << 1 );
   }

   byte num()
   {
      return val & 0b00000001;
   }

   void set( byte num, butEvType typ )
   {
      val = ( num ? 1 : 0 ) + ( (byte )typ << 1 );
   }

   butEvType type()
   {
      return (butEvType )(val >> 1);
   }

} ;

#endif   // ifndef BUT_H_INCLUDED