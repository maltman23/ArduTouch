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
         last = 0;

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

/*----------------------------------------------------------------------------*
 *
 *  Name:  LPFilter::menu
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
 *         The key mapping (inclusive of super class) is as follows:
 *
 *           -------------------------------------------------
 *           |   |   |   |   |   |   |   |   |   |   |   |   |
 *           |   |   |   |   |   |   |   |   |   |   |   |   |
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |   |   |   |   |   |   |   |   | ' |   | . |   | 
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |    ___     ___    |    ___     ___     ___    | 
 *           |     |       |     |     |       |       |     |
 *           |     |       |     |     |       |       |     |
 *           |  c  |       |     |     |   ~   |   <   |  !  |
 *           |     |       |     |     |       |       |     |
 *           |     |       |     |     |       |       |     |
 *           -------------------------------------------------
 *
 *----------------------------------------------------------------------------*/      

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
         if ( digit >= 0 )
            setClip( digit );
         break;

      case 's':                        // set # bits to shift 

         digit = console.getDigit( CONSTR("shift"), 7 );
         if ( digit >= 0 )
         {
            if ( digit < 1 ) digit = 1;
            setShift( digit );
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
/*----------------------------------------------------------------------------*
 *
 *  Name:  BSFilter::menu
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
 *         The key mapping (inclusive of super class) is as follows:
 *
 *           -------------------------------------------------
 *           |   |   |   |   |   |   |   |   |   |   |   |   |
 *           |   |   |   |   |   |   |   |   |   |   |   |   |
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |   |   |   |   |   |   |   |   | ' |   | . |   | 
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |    ___     ___    |    ___     ___     ___    | 
 *           |     |       |     |     |       |       |     |
 *           |     |       |     |     |       |       |     |
 *           |  c  |   s   |  +  |  -  |   ~   |   <   |  !  |
 *           |     |       |     |     |       |       |     |
 *           |     |       |     |     |       |       |     |
 *           -------------------------------------------------
 *
 *----------------------------------------------------------------------------*/      

char BSFilter::menu( key k )
{
   switch ( k.position() )
   {
      case  0: return 'c';
      case  2: return 's';
      case  4: return '+';
      case  5: return '-';
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
   weight = cutoff * lfo.value;
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

/******************************************************************************
 *
 *                               Tremolo 
 *                                                                            
 ******************************************************************************/

/*----------------------------------------------------------------------------*
 *
 *  Name:  Tremolo::charEv
 *
 *  Desc:  Process a character event.
 *
 *  Args:  code             - character to process
 *
 *  Memb:  +value           - current output value
 *
 *  Rets:  status           - if true, character was handled
 *
 *----------------------------------------------------------------------------*/

boolean Tremolo::charEv( char code )
{
   switch ( code )
   {
      case '.':                        // mute

         TermLFO::charEv('.');
         value = 1.0;
         break;

      case '!':                        // reset

        iniOsc( .4, 2.5 );  
        // fall thru

      default:

         return TermLFO::charEv( code );
   }
   return true;
}

#ifdef CONSOLE_OUTPUT
const char* Tremolo::prompt()
{
   return CONSTR("tremolo");
}
#endif


/******************************************************************************
 *
 *                               Vibrato 
 *                                                                            
 ******************************************************************************/

#define RATIO_SEMITONE   1.059463   // frequency ratio between adjacent pitches
#define INVERT_SEMITONE   .943874   // 1 / RATIO_SEMITONE

/*----------------------------------------------------------------------------*
 *
 *  Name:  Vibrato::evaluate
 *
 *  Desc:  Compute output value based on oscillator position.
 *
 *  Memb:  depth            - oscillation depth (0.0 - 1.0)
 *        +fader            - current fader value
 *         pos              - current position within oscillation range
 *        +value            - current output value
 *
 *----------------------------------------------------------------------------*/

void Vibrato::evaluate()
{
   double spos = fader * ((2.0 * pos) - depth);
   if ( spos >= 0 )
      value = 1.0 + (spos * (RATIO_SEMITONE-1.0) );
   else
      value = 1.0 + (spos * (1.0 - INVERT_SEMITONE));
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Vibrato::iniPos
 *
 *  Desc:  Set initial oscillator position.
 *
 *  Memb:  depth            - oscillation depth (0.0 - 1.0)
 *        +pos              - cur position within oscillation range
 *
 *----------------------------------------------------------------------------*/
       
void Vibrato::iniPos()
{
   pos = depth * 0.5;
}

#ifdef CONSOLE_OUTPUT
const char* Vibrato::prompt()
{
   return CONSTR("vibrato");
}
#endif
