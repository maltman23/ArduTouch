/*
    Filter.cpp  

    Implementation of the Filter classes.

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
#include "Audio.h"
#include "Console_.h"
#include "Filter.h"

                        /*--------------------
                                 Filter
                         --------------------*/

void Filter::charEv( char code )
{
   switch ( code )
   {
      case '-':                        // complement the filter output

         complement = true;
         break;

      case '+':                        // normal filter output
      case '!':                        // reset

         complement = false;           // normal filter output
                                       // fall thru to parent handler
      default:

         Effect::charEv( code );
   }
}

void Filter::info()
{
   Effect::info();
   if ( complement )
      console.print( '-' );
   else
      console.print( '+' );
}

char Filter::menu( key k )
{
   switch ( k.position() )
   {
      case  5: return '+';
      case  7: return '-';
      default: return Effect::menu( k );
   }
}

                        /*--------------------
                                BSFilter
                         --------------------*/

BSFilter::BSFilter() 
{ 
   shortcut = 'b';
   reset();
}

void BSFilter::charEv( char code )
{
   char digit;

   switch ( code )
   {
      case 'c':                        // set # bits to clip 

         digit = console.getDigit( "clip", MaxClip );
         if ( digit )
            setClip( digit - '0' );
         break;

      case 's':                        // set # bits to shift 

         digit = console.getDigit( "shift", 7 );
         if ( digit )
         {
            if ( digit < '1' ) digit = '1';
            setShift( digit - '0' );
         }
         break;

      case '!':                        // reset

         setShift(1);                  // 50% exponential average
         setClip(0);                   // clip no bits
                                       // fall thru to parent handler
      default:

         Filter::charEv( code );
   }
}

void BSFilter::info()
{
   Filter::info();
   console.infoByte( "shift", shift );
   console.infoByte( "clip", numClip );
}

char BSFilter::menu( key k )
{
   switch ( k.position() )
   {
      case  2: return 's';
      case  4: return 'c';
      default: return Filter::menu( k );
   }
}

void BSFilter::process( char *buf )
{
   Int r;                        // working register for performing shifts

   byte icnt = audio::bufSz;     // process this many ticks of input

   while ( icnt-- )
   {
      r.val    = last * mltShift;
      r._.msb &= clipMask;
      r.val   -= last;
      r.val   += *buf;
      r.val  >>= shift;
      last     = r.val;

      if ( complement )
         r.val = (int )(*buf - last);

      *buf++ = r.val;
   }
}

char *BSFilter::prompt()
{
   return "bsf";
}

void BSFilter::setClip( byte n )
{
   Word m;                            // working register for clipMask
   numClip  = n;
   m.val    = (word )(1 << (8 - n));
   m.val   -= 1;
   clipMask = m._.lsb;
}

void BSFilter::setShift( byte n )
{
   shift = n;
   mltShift = 1;
   while ( n ) 
   {
      mltShift += mltShift;
      --n;
   }
}

                        /*--------------------
                                XMFilter
                         --------------------*/

XMFilter::XMFilter() 
{ 
   shortcut = 'x';
   reset();
}

void XMFilter::charEv( char code )
{
   switch ( code )
   {
      case 'w':                        // get weight value 

         console.getByte( "weight", &this->weight );
         break;

      default:

         Filter::charEv( code );
   }
}

void XMFilter::info()
{
   Filter::info();
   console.infoByte( "weight", weight );
}

void XMFilter::process( char *buf )
{
   #define ROUND_LSB             // when scaling, round least significant byte 

   Int b;                        // working register for scaling buf value
   Int l;                        // working register for scaling last value

   byte icnt = audio::bufSz;     // process this many ticks of input

   byte _weight = ~weight + 1;   // weight + ~weight == "1"

   while ( icnt-- )              // while there are ticks to process ...
   {
      b.val = *buf * weight;     // exponentially scale terms
      l.val = last * _weight;    // ""

      #ifdef ROUND_LSB           // and round them
      {
         if ( l._.lsb & 0b10000000 ) ++l._.msb;
         if ( b._.lsb & 0b10000000 ) ++b._.msb;
      }
      #endif

      last = l._.msb + b._.msb;
      *buf = complement ? *buf - last : last;
      ++buf;
   }
   #undef ROUND_LSB
}

char XMFilter::menu( key k )
{
   switch ( k.position() )
   {
      case  2: return 'w';
      default: return Filter::menu( k );
   }
}

char *XMFilter::prompt()
{
   return "xmf";
}

