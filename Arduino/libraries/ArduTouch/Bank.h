/*
    Bank.h  

    Declaration of the Bank and MacroBank classes.

    ---------------------------------------------------------------------------
 
    Copyright (C) 2016, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
   ---------------------------------------------------------------------------
*/

#ifndef BANK_H_INCLUDED
#define BANK_H_INCLUDED

#include "Arduino.h"
#include "Mode.h"

#define begin_bank(x)     const bankmem x[] PROGMEM = {

#ifdef CONSOLE_OUTPUT

   #define _member(x,y)      { x, y },
   #define end_bank()        { NULL, NULL } };

   struct bankmem
   {
      PROGMEM const void *data;        // ptr to member data  
      PROGMEM const char *name;        // ptr to member name 
   } ;

#else

   #define _member(x,y)      x,
   #define end_bank()        NULL };

   struct bankmem
   {
      PROGMEM const void *data;        // ptr to member data  
   } ;

#endif


class Bank : public Mode
{
   public:

   Bank() 
   { 
      flags |= MENU;                   // persistent keybrd menu
   }
   
   boolean choose();                   // wait for user to choose a member
                                       // (return false if no member chosen)
   byte    choice();                   // returns index of chosen member

   const void* dataPtr();              // return addr of chosen member's data
   const void* dataPtr( byte );        // return addr of nth member's data

   boolean charEv( char );             // handle a character event
   void    load( const bankmem *p );   // load a list of members
   const char* name( byte );           // return name of nth member
   virtual void onChoice() {};         // execute this when member is chosen

   bool    select( byte );             // select nth member of bank

   #ifdef KEYBRD_MENUS
   char    menu( key );                // given a key, return a character 
   #endif

   protected:

   static  const byte Max = 10;        // max permitted # of members
   byte    num;                        // # of members in bank

   private:

   const byte*  base;                  // pts to 1st byte of member list

   byte    idx;                        // idx # of chosen member
   boolean chosen;                     // if true, a member was chosen

} ;


class MacroBank : public Bank
{
   public:

   void   onChoice();                  // execute this when member is chosen

   #ifdef CONSOLE_OUTPUT
   const  char *prompt();              // return object's prompt string
   #endif
} ;

#endif   // ifndef BANK_H_INCLUDED