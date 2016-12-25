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

/******************************************************************************
 *
 *                                 LPFilter 
 *
 * Low-pass filter using an exponential moving average.
 *
 ******************************************************************************/

#define LPF_DEF_CUTOFF  255            // default cutoff value at reset

LPFilter::LPFilter()
{
   shortcut = 'l';
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  LPFilter::calcWeight
 *
 *  Desc:  Calculate the weighting for the input term
 *
 *  Memb:  cutoff           - filter cutoff (n/255)
 *        +weight           - weighting applied to input term (n/255)
 *
 *----------------------------------------------------------------------------*/      

void LPFilter::calcWeight()
{
   weight = cutoff;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  LPFilter::charEv
 *
 *  Desc:  Process a character event.
 *
 *  Args:  code             - character to process
 *
 *  Memb: +cutoff           - filter cutoff value (n/255)
 *
 *  Rets:  status           - if true, character was handled
 *
 *----------------------------------------------------------------------------*/

boolean LPFilter::charEv( char code )
{
   switch ( code )
   {
      #ifdef INTERN_CONSOLE

      case 'c':                        // get new cutoff value 

         console.getByte( CONSTR("cutoff"), &this->cutoff );
         setCutoff( cutoff );
         break;

      #endif

      #ifdef CONSOLE_OUTPUT

      case chrInfo:                    // display object info to console

         Effect::charEv( chrInfo );
         console.infoByte( CONSTR("cutoff"), cutoff );
         break;

      #endif

      case '!':                        // perform a reset

         setCutoff( LPF_DEF_CUTOFF );

         // fall thru to Effect reset

      default:

         return Effect::charEv( code );
   }
   return true;
}


/*----------------------------------------------------------------------------*
 *
 *  Name:  LPFilter::evHandler
 *
 *  Desc:  Handle an onboard event.
 *
 *  Args:  ev               - onboard event
 *
 *  Rets:  status           - true if the event was handled
 *
 *----------------------------------------------------------------------------*/      

boolean LPFilter::evHandler( obEvent ev )
{
   switch ( ev.type() )
   {
      case POT0:       

         setCutoff( ev.getPotVal() );
         return true;
         break;

      default:

         return Effect::evHandler( ev );
   }
}

#ifdef KEYBRD_MENUS
char LPFilter::menu( key k )
{
   switch ( k.position() )
   {
      case  0: return 'c';                // set cutoff level
      default: return Effect::menu( k );
   }
}
#endif

#ifdef CONSOLE_OUTPUT
const char *LPFilter::prompt()
{
   return CONSTR("lpf");
}
#endif

/*----------------------------------------------------------------------------*
 *
 *  Name:  LPFilter::process
 *
 *  Desc:  Process an input buffer.
 *
 *  Args:  buf              - ptr to buffer to process
 *
 *  Glob:  audioBufSz       - size of input buffer
 *
 *  Memb: +last             - last output value
 *         weight           - weighting (n/255) applied to input term
 *
 *  Note: Approximate math is used in the equations involving weightings, by 
 *        considering 255 to be "1". This has proven to be good enough for 8 
 *        bit audio.
 *
 *----------------------------------------------------------------------------*/      

void LPFilter::process( char *buf )
{
   Int b;                        // working register for scaling buf value
   Int l;                        // working register for scaling last value

   byte _weight = 255-weight;    // _weight is weighting applied to last average
   byte icnt = audioBufSz;       // process this many ticks of input
   while ( icnt-- )              // while there are ticks to process ...
   {
      b.val = *buf * weight;     // exponentially scale terms
      l.val = last * _weight;    // ""

      if ( l._.lsb & 0b10000000 ) // round scaled terms ...
         ++l._.msb;
      if ( b._.lsb & 0b10000000 ) 
         ++b._.msb;

      last = l._.msb + b._.msb;
      *buf = last;
      ++buf;
   }
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  LPFilter::setCutoff
 *
 *  Desc:  Process an input buffer.
 *
 *  Args:  c                - cutoff level (n/255)
 *
 *  Memb: +cutoff           - filter cutoff (n/255)
 *
 *----------------------------------------------------------------------------*/      

void LPFilter::setCutoff( byte c )
{
   cutoff = c;
   calcWeight();
}

/******************************************************************************
 *
 *                                 BSFilter 
 *
 *  A "bit-shift" filter using an exponential moving average (powers of 2)
 *  with optional clipping of the shifted multiple (producing non-linear
 *  distortion).
 *                                                                            
 ******************************************************************************/

BSFilter::BSFilter() 
{ 
   shortcut = 'b';
}

boolean BSFilter::charEv( char code )
{
   char digit;

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
         console.infoByte( CONSTR("shift"), shift );
         console.infoByte( CONSTR("clip"), numClip );
         break;

      #endif

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

      case '-':                        // complement the filter output

         complement = true;
         break;

      #endif

      case '!':                        // reset

         setShift(1);                  // 50% exponential average
         setClip(0);                   // clip no bits

      case '+':                        // normal filter output

         complement = false;           // fall thru to parent handler

      default:

         return Effect::charEv( code );

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

         return Effect::evHandler( ev );
   }
}

#ifdef KEYBRD_MENUS
char BSFilter::menu( key k )
{
   switch ( k.position() )
   {
      case  2: return 's';
      case  4: return 'c';
      case  5: return '+';
      case  7: return '-';
      default: return Effect::menu( k );
   }
}
#endif

void BSFilter::process( char *buf )
{
   Int r;                        // working register for performing shifts

   byte icnt = audioBufSz;       // process this many ticks of input

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

/******************************************************************************
 *
 *                                  AutoWah 
 *
 ******************************************************************************/

AutoWah::AutoWah()
{
   shortcut = 'a';
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  AutoWah::calcWeight
 *
 *  Desc:  Calculate the weighting applied to the input term
 *
 *  Memb:  cutoff           - filter cutoff (n/255)
 *         lfo.val          - scaling coefficient for cutoff
 *        +weight           - weighting applied to input term (n/255)
 *
 *----------------------------------------------------------------------------*/      

void AutoWah::calcWeight()
{
   weight = cutoff * lfo.val;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  AutoWah::charEv
 *
 *  Desc:  Process a character event.
 *
 *  Args:  code             - character to process
 *
 *  Memb:  lfo              - LFO which scales the cutoff value
 *
 *  Rets:  status           - if true, character was handled
 *
 *----------------------------------------------------------------------------*/

boolean AutoWah::charEv( char code )
{
   switch ( code )
   {
      #ifdef INTERN_CONSOLE

      case 'l':                        // push LFO

         console.pushMode( &this->lfo );
         break;

      #endif

      #ifdef CONSOLE_OUTPUT

      case chrInfo:                    // display object info to console

         LPFilter::charEv( chrInfo );
         lfo.brief();
         break;

      #endif

      case '!':                        // perform a reset

         lfo.reset();                  // reset LFO
                                       // fall thru
      default:

         return LPFilter::charEv( code );
   }
   return true;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  AutoWah::dynamics
 *
 *  Desc:  Perform a dynamic update.
 *
 *  Memb:  lfo              - LFO which scales the cutoff value
 *
 *----------------------------------------------------------------------------*/      

void AutoWah::dynamics()
{
   lfo.dynamics();                     // update LFO which scales cutoff value
   calcWeight();                       // recalculate the weight for input term
}

#ifdef CONSOLE_OUTPUT
const char *AutoWah::prompt()
{
   return CONSTR("autowah");
}
#endif

/******************************************************************************
 *
 *                                  WahLFO 
 *
 ******************************************************************************/

/*----------------------------------------------------------------------------*
 *
 *  Name:  WahLFO::charEv
 *
 *  Desc:  Process a character event.
 *
 *  Args:  code             - character to process
 *
 *  Rets:  status           - if true, character was handled
 *
 *----------------------------------------------------------------------------*/

boolean WahLFO::charEv( char code )
{
   switch ( code )
   {
      case '!':                        // perform a reset

         LFO::charEv( code );
         setDepth( 1.0 );
         setMute( false );
         break;

      default:

         return LFO::charEv( code );
   }
   return true;
}

