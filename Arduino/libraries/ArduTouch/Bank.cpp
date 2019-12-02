/*
    Bank.cpp  

    Implementation of the Bank and MacroBank classes.

    ---------------------------------------------------------------------------
 
    Copyright (C) 2016, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
   ---------------------------------------------------------------------------
*/

#include "types.h"
#include "Bank.h"
#include "Console_.h"

/* ------------------------      Bank class      ---------------------------- */

boolean Bank::choose()
{
   chosen = false;

   #ifdef INTERN_CONSOLE
   console.runMode( this );
   if ( chosen )
      onChoice();
   #endif

   return chosen;
}

byte Bank::choice()
{
   return idx;
}

const void* Bank::dataPtr()
{
   return dataPtr( idx );
}

const void* Bank::dataPtr( byte nth )
{
   Word   dataSlot;
   const byte* cur = base + nth * sizeof( bankmem );
   dataSlot._.lsb  = pgm_read_byte_near( cur++ );
   dataSlot._.msb  = pgm_read_byte_near( cur );
   return (const void *)dataSlot.val;
}

#ifdef INTERN_CONSOLE

boolean Bank::charEv( char code )
{
   if ( code >= '0' && code < '0' + Max )
   {
      idx    = code - '0';
      chosen = idx < num;
      console.popMode();
   }
   #ifdef CONSOLE_OUTPUT
   else if ( code == chrInfo )
   {
      for ( byte i = 0; i < num; i++ )
      {
         if ( i > 0 )
            console.newline();
         console.rtab();
         console.print( (char )('0' + i) );   
         console.romprint( CONSTR(": ") );
         console.romprint( name(i) );
      }
   }
   #endif      
   else
      return Mode::charEv( code );

   return true;
}

#else

boolean Bank::charEv( char code )
{
   return Mode::charEv( code );
}

#endif

void Bank::load( const bankmem *p )
{
   Word slot;

   base = (byte *)p;

   /* determine number of members in null-terminated list */

   const byte* cur = base;                   
   slot._.lsb  = pgm_read_byte_near( cur++ ); // set slot = bankmem[0].data
   slot._.msb  = pgm_read_byte_near( cur++ ); 

   num = 0;
   while ( num < Max && slot.val )           // loop till null data ptr, or Max
   {
      num++;                                 // bump num

      #ifdef CONSOLE_OUTPUT
      cur += sizeof(char *);                 // skip over bankmem[num].name 
      #endif

      slot._.lsb = pgm_read_byte_near( cur++ ); // set slot = bankmem[num].name
      slot._.msb = pgm_read_byte_near( cur++ ); 
   }
}

#ifdef KEYBRD_MENUS

/*----------------------------------------------------------------------------*
 *
 *  Name:  Bank::menu
 *
 *  Desc:  Given a key, return a character (to be processed via charEv()). 
 *
 *  Args:  k                - key
 *
 *  Rets:  c                - character (0 means "no character")
 *
 *  Note:  If a sketch is compiled with KEYBRD_MENUS defined, then this method 
 *         can be used to map the onboard keys to characters which the system 
 *         will automatically feed to the charEv() method.
 *
 *         This method is only called by the system if the MENU flag in this
 *         object is set (in the ::flags byte inherited from Mode), or if the
 *         keyboard is in a "oneShot menu selection" state.
 *
 *         The key mapping is as follows:
 *
 *           -------------------------------------------------
 *           |   |   |   |   |   |   |   |   |   |   |   |   |
 *           |   |   |   |   |   |   |   |   |   |   |   |   |
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |   | 1 |   | 3 |   |   | 6 |   | 8 |   |   |   | 
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |    ___     ___    |    ___     ___     ___    | 
 *           |     |       |     |     |       |       |     |
 *           |     |       |     |     |       |       |     |
 *           |  0  |   2   |  4  |  5  |   7   |   9   |     |
 *           |     |       |     |     |       |       |     |
 *           |     |       |     |     |       |       |     |
 *           -------------------------------------------------
 *
 *----------------------------------------------------------------------------*/      

char Bank::menu( key k )
{
   return ( k.position() <= 9 ? '0' + k.position() : 0 );
}

#endif

const char* Bank::name( byte ith )
{
   #ifdef CONSOLE_OUTPUT
   if ( base )
   {
      Word  nameSlot;
      const byte* cur = base + ith * sizeof( bankmem ) + sizeof(void *);
      nameSlot._.lsb  = pgm_read_byte_near( cur++ );
      nameSlot._.msb  = pgm_read_byte_near( cur );
      return (const char *)nameSlot.val;
   }
   else
   #endif
      return CONSTR("");
}

bool Bank::select( byte ith )
{
   if ( ith < num ) 
   {
      idx = ith;
      onChoice();
      return true;
   }
   else
      return false;
}

/* ----------------------     MacroBank class      -------------------------- */

void MacroBank::onChoice()
{
   console.exe( (const char *)dataPtr() );
}

