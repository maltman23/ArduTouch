/*
    Onboard.cpp

    Implementation of onboard methods.

    ---------------------------------------------------------------------------
 
    Copyright (C) 2016, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
   ---------------------------------------------------------------------------
*/

#include "Onboard.h"

/******************************************************************************
 *
 *                                  key 
 *
 ******************************************************************************/

void key::transpose( char xpose )
{
   const int MaxKeyNum = 12*15;        // 12 notes per octave, 15 octaves max

   byte pos, oct;                      // transposed key position, octave
   int  keyNum;                        // transposed key number

   // translate untransposed key octave/position into keyNum

   keyNum  = position() + octave() * 12;

   // transpose keyNum

   keyNum += xpose;

   // range-check transposed keyNum

   while ( keyNum < 0 )
      keyNum += 12;
         
   while ( keyNum > MaxKeyNum )
      keyNum -= 12;

   // translate transposed keyNum back into octave/position
      
   oct = keyNum / 12;
   pos = keyNum % 12;
   set( pos, oct );
}
