/*
    Bank.cpp  

    Implementation of the Bank class.

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

boolean Bank::choose()
{
   chosen = false;
   console.runMode( this );
   if ( chosen )
      onChoice();
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
   const byte* cur = base + nth * sizeof( bankmem ) + sizeof( char* );
   dataSlot._.lsb  = pgm_read_byte_near( cur++ );
   dataSlot._.msb  = pgm_read_byte_near( cur );
   return (const void *)dataSlot.val;
}

void Bank::info()
{
   console.rtab();
   for ( byte i = 0; i < num; i++ )
   {
      console.print( (char )('0' + i) );   
      console.print( ": " );
      console.romprint( name(i) );
      console.print( "  " );
   }   
}

void Bank::charEv( char code )
{
   if ( code >= '0' && code < '0' + num )
   {
      idx    = code - '0';
      chosen = true;
      console.popMode();
   }
   else
      Mode::charEv( code );
}

void Bank::load( const bankmem *p )
{
   Word nameSlot;

   base = (byte *)p;

   /* determine number of members in null-terminated list */

   const byte* cur = base;                   
   nameSlot._.lsb  = pgm_read_byte_near( cur++ ); // set nameSlot = bankmem[0].name
   nameSlot._.msb  = pgm_read_byte_near( cur++ ); 

   num = 0;
   while ( num < Max && nameSlot.val )       // loop till null name ptr, or Max
   {
      cur += sizeof(void *);                 // skip over bankmem[num].data 
      num++;                                 // bump num

      nameSlot._.lsb = pgm_read_byte_near( cur++ ); // set nameSlot = bankmem[num].name
      nameSlot._.msb = pgm_read_byte_near( cur++ ); 
   }
}

char Bank::menu( key k )
{
   return ( k.position() <= 9 ? '0' + k.position() : 0 );
}

const char* Bank::name( byte ith )
{
   Word   nameSlot;
   const byte* cur = base + ith * sizeof( bankmem );
   nameSlot._.lsb  = pgm_read_byte_near( cur++ );
   nameSlot._.msb  = pgm_read_byte_near( cur );
   return (const char *)nameSlot.val;
}


