/*
    StdEffects.cpp  

    Implementation of the standard effect classes.

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
#include "StdEffects.h"

                        /*--------------------
                                 Filter
                         --------------------*/

boolean Filter::charEv( char code )
{
   switch ( code )
   {
      #ifdef CONSOLE_OUTPUT
      case chrInfo:                    // display object info to console

         Effect::charEv( chrInfo );
         if ( complement )
            console.print( '-' );
         else
            console.print( '+' );
         console.space();
         break;
      #endif

      #ifdef INTERN_CONSOLE
      case '-':                        // complement the filter output

         complement = true;
         break;

      case '+':                        // normal filter output
      #endif
      case '!':                        // reset

         complement = false;           // normal filter output
                                       // fall thru to parent handler
      default:

         return Effect::charEv( code );
   }
   return true;
}

#ifdef KEYBRD_MENUS
char Filter::menu( key k )
{
   switch ( k.position() )
   {
      case  5: return '+';
      case  7: return '-';
      default: return Effect::menu( k );
   }
}
#endif

                        /*--------------------
                                BSFilter
                         --------------------*/

BSFilter::BSFilter() 
{ 
   shortcut = 'b';
   reset();
}

boolean BSFilter::charEv( char code )
{
   char digit;

   switch ( code )
   {
      #ifdef INTERN_CONSOLE
      case 'c':                        // set # bits to clip 

         digit = console.getDigit( CONSTR("clip"), MaxClip );
         if ( digit )
            setClip( digit - '0' );
         break;

      case 's':                        // set # bits to shift 

         digit = console.getDigit( CONSTR("shift"), 7 );
         if ( digit )
         {
            if ( digit < '1' ) digit = '1';
            setShift( digit - '0' );
         }
         break;
      #endif

      #ifdef CONSOLE_OUTPUT
      case chrInfo:                    // display object info to console

         Filter::charEv( chrInfo );
         console.infoByte( CONSTR("shift"), shift );
         console.infoByte( CONSTR("clip"), numClip );
         break;
      #endif

      case '!':                        // reset

         setShift(1);                  // 50% exponential average
         setClip(0);                   // clip no bits
                                       // fall thru to parent handler
      default:

         return Filter::charEv( code );
   }
   return true;
}

boolean BSFilter::evHandler( obEvent ev )
{
   switch ( ev.type() )
   {
      case POT0:       

         setShift( ev.getPotVal() >> 5 );
         return true;
         break;

      case POT1:       

         setClip( ev.getPotVal() >> 5 );
         return true;
         break;

      default:

         return Filter::evHandler( ev );
   }
}

#ifdef KEYBRD_MENUS
char BSFilter::menu( key k )
{
   switch ( k.position() )
   {
      case  2: return 's';
      case  4: return 'c';
      default: return Filter::menu( k );
   }
}
#endif

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

#ifdef CONSOLE_OUTPUT
const char *BSFilter::prompt()
{
   return CONSTR("bsf");
}
#endif

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

#define XMF_DEF_WEIGHTING  64          // default weighting at reset

XMFilter::XMFilter() 
{ 
   shortcut = 'x';
   reset();
}

boolean XMFilter::charEv( char code )
{
   switch ( code )
   {
      #ifdef INTERN_CONSOLE
      case 'w':                        // get weight value 

         console.getByte( CONSTR("weight"), &this->weight );
         setWeight( weight );
         break;
      #endif

      #ifdef CONSOLE_OUTPUT
      case chrInfo:                    // display object info to console

         Filter::charEv( chrInfo );
         console.infoByte( CONSTR("weight"), weight );
         break;
      #endif

      case '!':                        // perform a reset

         setWeight( XMF_DEF_WEIGHTING );

         // fall thru to Filter reset

      default:

         return Filter::charEv( code );
   }
   return true;
}

boolean XMFilter::evHandler( obEvent ev )
{
   switch ( ev.type() )
   {
      case POT0:       

         setWeight( ev.getPotVal() );
         return true;
         break;

      default:

         return Filter::evHandler( ev );
   }
}

void XMFilter::process( char *buf )
{
   #define ROUND_LSB             // when scaling, round least significant byte 

   Int b;                        // working register for scaling buf value
   Int l;                        // working register for scaling last value

   byte icnt = audio::bufSz;     // process this many ticks of input

   byte _weight = 255-weight;    // weight + ~weight = "1"

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
      if ( complement )
      {
         b.val = *buf - last;    
         b.val *= norm.val;       
         *buf = b._.msb;
      }
      else
      {
         *buf = last;
      }
      ++buf;
   }
   #undef ROUND_LSB
}

#ifdef KEYBRD_MENUS
char XMFilter::menu( key k )
{
   switch ( k.position() )
   {
      case  2: return 'w';
      default: return Filter::menu( k );
   }
}
#endif

#ifdef CONSOLE_OUTPUT
const char *XMFilter::prompt()
{
   return CONSTR("xmf");
}
#endif

void XMFilter::setWeight( byte w )
{
   weight = w;
   if ( w < 128 )
   {
      norm.val = weight;
      norm.val += 128;
   }
   else
      norm.val = 256;
}
