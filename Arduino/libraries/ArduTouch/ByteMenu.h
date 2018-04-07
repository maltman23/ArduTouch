/*
    ByteMenu.h  

    Definition of the ByteMenu base class.

    ---------------------------------------------------------------------------
 
    Copyright (C) 2018, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
   ---------------------------------------------------------------------------
*/

#ifndef BYTEMENU_H_INCLUDED
#define BYTEMENU_H_INCLUDED

#include "Console_.h"
#include "Mode.h"

/******************************************************************************
 *
 *                             ByteMenu 
 *
 *  This class offers a quick way to choose a byte value by pressing a key
 *  on the ArduTouch board.
 *
 *  Example:  Select a value x from the set {0, 2, 4 ... 22}, 
 *            based on which key the user presses.
 *
 *  class MyMenu : public ByteMenu
 *  {
 *    public: 
 *
 *    byte evaluate( byte keypos ) { return 2*keypos; }  
 *
 *  }  myMenu ;
 *
 *  byte x;
 *  myMenu.waitKey();
 *  x = myMenu.value;
 *
 ******************************************************************************/

class ByteMenu : public Mode
{
   typedef Mode super;                 // superclass is Mode

   bool   waiting;                     // if true, waiting for key press
   
   public:

   byte   value;                       // "return" value for the menu

   void waitKey()
   {
      waiting = true;
      console.runModeWhile( this, &this->waiting );
   }

   virtual byte evaluate( byte keypos )
   {
      return keypos;
   }

   bool evHandler( obEvent ev )
   {
      switch ( ev.type() )
      {
         case KEY_DOWN:
      
            value = evaluate( ev.getKey().position() ); 
            break;
      
         case KEY_UP:

            waiting = false;
            break;

         default:                 

            return super::evHandler( ev );
      }
      return true;                 
   }

} ;

#endif   // ifndef BYTEMENU_H_INCLUDED
