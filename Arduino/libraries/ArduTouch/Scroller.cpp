/*
    Scroller.cpp  

    Implementation of the Scroller class.

    ---------------------------------------------------------------------------
 
    Copyright (C) 2016, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
   ---------------------------------------------------------------------------
*/

#include "Scroller.h"

byte Scroller::getScrollPos()          // return scroll position of top pot
{
   return scrollPos;
}

byte Scroller::getScrollable()         // return # of scrollable parameters
{
   return numScrollable;
}

void Scroller::setScrollable( byte n ) // set # of scrollable parameters
{
   numScrollable = n;
   if ( n ) setScrollPos(1);
}

void Scroller::setScrollPos( byte p )  // set scroll position of top pot
{
   byte maxNum = getScrollable();
   if ( p )
   {
      if ( maxNum-- )
         scrollPos = (p < maxNum) ? p : maxNum; 
   }
   else 
      scrollPos = maxNum ? 1 : 0;
}

void Scroller::scrollUp()              // scroll pots up by 1 parameter
{
   byte pos = getScrollPos();
   if ( pos > 1 )
      setScrollPos( --pos );
}

void Scroller::scrollDn()              // scroll pots down by 1 parameter
{
   setScrollPos( getScrollPos()+1 );
}

byte Scroller::getScrollParm( obEvent ev )
{
   if ( ev.genus() != evPOT )
      return 0;
   else
      return getScrollPos() + ( ev.type() - POT0 );
}


