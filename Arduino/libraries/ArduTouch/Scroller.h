/*
    Scroller.h  

    Declaration of the Scroller class.

    ---------------------------------------------------------------------------
 
    Copyright (C) 2016, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
   ---------------------------------------------------------------------------
*/

#ifndef SCROLLER_H_INCLUDED
#define SCROLLER_H_INCLUDED

#include "Onboard.h"

class Scroller 
{
   public:

   byte getScrollable();         // return # of scrollable parameters
   byte getScrollPos();          // return scroll position of top pot
   void setScrollable( byte );   // set # of scrollable parameters
   void setScrollPos( byte );    // set scroll position of top pot
   void scrollUp();              // scroll pots up by 1 parameter
   void scrollDn();              // scroll pots down by 1 parameter
   byte getScrollParm(obEvent);  // return associated parameter # for a
                                 // (scrolling pot) event
   private:

   byte scrollPos;               // scroll position of top pot
   byte numScrollable;           // # of scrollable parameters

} ;


#endif   // ifndef SCROLLER_H_INCLUDED
