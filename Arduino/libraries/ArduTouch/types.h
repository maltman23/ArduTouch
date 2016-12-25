/*
    types.h  

    Declarations for custom types.

    ---------------------------------------------------------------------------
 
    Copyright (C) 2016, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
   ---------------------------------------------------------------------------
*/

#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED

#include "Arduino.h"

/*-----------------------------------------------------------------------------
 *
 *  STRU:  Word
 *
 *  DESC:  The Word type allows for independent manipulation of the lsb and 
 *         msb of a 16-bit word, without explicit type casting.
 *
 *         Code example:  
 *
 *           Word x, y;
 *  
 *           x._.lsb = 20;
 *           x._.msb = 1;
 *
 *           y.val = 256 + 256;
 *
 *           printf( "%u %u %u %u\n", x._.lsb, x._.msb, x.val, y._.msb );
 * 
 *         this code when run will output:
 * 
 *           20 1 276 2
 *
 *----------------------------------------------------------------------------*/

typedef struct {

   union {

      struct  {

         byte lsb;
         byte msb;

         } _;

      word  val;

      };

   } Word;


/*-----------------------------------------------------------------------------
 *
 *  STRU:  Int
 *
 *  DESC:  The Int type allows for independent manipulation of the lsb and 
 *         msb of an integer, without explicit type casting.
 *
 *----------------------------------------------------------------------------*/

typedef struct {

   union {

      struct  {

         byte lsb;
         char msb;

         } _;

      int  val;

      };

   } Int;


/*-----------------------------------------------------------------------------
 *
 *  STRU:  DWord
 *
 *  DESC:  The Dword type allows for independent manipulation of the lsw and 
 *         msw of an unsigned long, without explicit type casting.
 *
 *         Code example:  
 *
 *           Dword x, y;
 *
 *           x._.lsw.val = 20;
 *           x._.msw.val = 1;
 *
 *           y.val = 65536 + 65536;
 *
 *           printf( "%u %u %lu %u\n", x._.lsw.val, x._.msw.val,
 *                                     x.val, y._.msw.val );
 *
 *         this code when run will output:
 * 
 *           20 1 65556 2
 *
 *----------------------------------------------------------------------------*/

typedef struct {

   union {

      struct  {

         Word lsw;
         Word msw;

         } _;

      unsigned long val;

      };

   } DWord;


#endif   // ifndef TYPES_H_INCLUDED
