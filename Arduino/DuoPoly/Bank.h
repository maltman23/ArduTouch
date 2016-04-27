/*
    Bank.h  

    Declaration of the Bank class.

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

struct bankmem
{
   PROGMEM const char *name;           // ptr to member name 
   PROGMEM const void *data;           // ptr to member data  
} ;

class Bank : public Mode
{
   public:

   boolean choose();                   // wait for user to choose a member
                                       // (return false if no member chosen)
   byte    choice();                   // returns index of chosen member

   const void* dataPtr();              // return addr of chosen member's data
   const void* dataPtr( byte );        // return addr of nth member's data

   void    charEv( char );             // handle a character event
   void    info();                     // display object info to console
   void    load( const bankmem *p );   // load a list of members
   char    menu( key );                // map key event to character 
   const char* name( byte );           // return name of nth member
   virtual void onChoice() {};         // execute this when member is chosen

   protected:

   static  const byte Max = 8;         // max permitted # of members
   byte    num;                        // # of members in bank

   private:

   const byte*  base;                  // pts to 1st byte of member list

   byte    idx;                        // idx # of chosen member
   boolean chosen;                     // if true, a member was chosen

} ;

#endif   // ifndef BANK_H_INCLUDED