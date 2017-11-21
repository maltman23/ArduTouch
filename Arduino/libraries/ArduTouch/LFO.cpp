/*
    LFO.cpp

    Implementation of the LFO class.

    ---------------------------------------------------------------------------

    Copyright (C) 2016, Cornfield Electronics, Inc.

    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0
    Unported License.

    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/

    Created by Bill Alessi & Mitch Altman.

   ---------------------------------------------------------------------------
*/

#include "Console_.h"
#include "Audio.h"
#include "LFO.h"

#define DEF_DEPTH       0.25     // default depth of oscillation

#define MAX_LOW_FREQ   20.0      // maximum freq  of oscillation
#define MIN_LOW_FREQ    0.01     // minimum freq  of oscillation
#define DEF_LOW_FREQ    3.8      // default freq  of oscillation


/******************************************************************************
 *
 *                                 LFO 
 *                                                                            
 ******************************************************************************/

/*----------------------------------------------------------------------------*
 *
 *  Name:  LFO::calcStep
 *
 *  Desc:  Calculate the change in oscillator position per dynamic update.
 *
 *  Glob:  dynaRate         - dynamic udpate rate
 *
 *  Memb:  depth            - oscillation depth (0.0 - 1.0)
 *         freq             - oscillation frequency
 *        +step             - change in osc position per dynamic update
 *
 *----------------------------------------------------------------------------*/
       
void LFO::calcStep()
{
   /* calculate the step, preserving the current direction */

   boolean decreasing = ( step < 0.0 );
   step = 2.0 * depth * freq / dynaRate;
   if ( decreasing )
      step = -step;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  LFO::charEv
 *
 *  Desc:  Process a character event.
 *
 *  Args:  code             - character to process
 *
 *  Rets:  status           - if true, character was handled
 *
 *----------------------------------------------------------------------------*/
       
boolean LFO::charEv( char key )
{
   double tempDouble;                  // return value from getDouble()

   switch ( key )
   {
      #ifdef INTERN_CONSOLE
      case 'd':                        // set depth of oscillation

         if ( console.getDouble( CONSTR("depth"), &tempDouble ) )
            setDepth( tempDouble );
         break;

      case 'f':                        // set frequency of oscillation

         if ( console.getDouble( CONSTR("freq"), &tempDouble ) )
            setFreq( tempDouble );
         break;
      #endif

      #ifdef CONSOLE_OUTPUT
      case chrInfo:                    // display object info to console

         super::charEv( chrInfo );
         console.infoDouble( CONSTR("freq"), freq );
         console.infoDouble( CONSTR("depth"), depth );
         break;
      #endif

      case '.':                        // mute

         iniVal();                     // set to initial position
         super::charEv( key );
         break;

      case '!':                        // reset

         iniOsc( DEF_DEPTH, DEF_LOW_FREQ );

         // fall thru to superclass handler

      default:

         return super::charEv( key );
   }
   return true;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  LFO::dynamics
 *
 *  Desc:  Perform a dynamic update by bumping the LFO position.
 *
 *  Memb:  depth            - oscillation depth (0.0 - 1.0)
 *         flags.MUTE       - LFO is muted
 *        +pos              - cur position within oscillation range
 *         step             - change in osc position per dynamic update
 *
 *----------------------------------------------------------------------------*/
       
void LFO::dynamics()
{
   if ( ! muted() )
   {
      pos += step;                     // bump oscillator position

      // turn oscillator back if bounds exceeded

      if ( pos > depth )
      {
         pos  = 2 * depth - pos;
         step = -step;
      }
      else if ( pos < 0.0 )
      {
         pos  = - pos;
         step = -step;
      }
      evaluate();                      // set output value
   }
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  LFO::evaluate
 *
 *  Desc:  Compute output value based on oscillator position.
 *
 *  Memb:  pos              - current position within oscillation range
 *        +val              - current output value
 *
 *----------------------------------------------------------------------------*/
       
void LFO::evaluate()
{
   value = 1.0 - pos;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  LFO:evHandler
 *
 *  Desc:  Handle an onboard event.
 *
 *  Args:  ev               - onboard event
 *
 *  Rets:  status           - true if the event was handled
 *
 *----------------------------------------------------------------------------*/      

boolean LFO::evHandler( obEvent ev )
{
   switch ( ev.genus() )
   {
      case evPOT:
      {
         byte val = ev.getPotVal();

         switch ( ev.type() )
         {
            case POT0:       
            
               setFreq( MAX_LOW_FREQ * val * val / 65025.0 );
               break;
            
            case POT1:       

               setDepth( val ? (double )val / 255.0 : 0.0 );
               break;
          }

         return true;
         break;
      }
      default:

         return super::evHandler(ev);
   }
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  LFO::getDepth
 *
 *  Desc:  Get the oscillation depth.
 *
 *  Args:  d                - oscillation depth 
 *
 *  Memb:  depth            - oscillation depth (0.0 - 1.0)
 *
 *----------------------------------------------------------------------------*/

double LFO::getDepth()
{
   return depth;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  LFO::getFreq
 *
 *  Desc:  Get the oscillation frequency.
 *
 *  Rets:  f                - oscillation frequency 
 *
 *  Memb:  freq             - oscillation frequency
 *
 *----------------------------------------------------------------------------*/

double LFO::getFreq()
{
   return freq;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  LFO::iniOsc
 *
 *  Desc:  Initialize the LFO.
 *
 *  Args:  d                - oscillation depth 
 *         f                - oscillation frequency 
 *
 *  Memb: +freq             - oscillation frequency
 *
 *----------------------------------------------------------------------------*/

void LFO::iniOsc( double d, double f )
{
   freq = f;
   setDepth( d );
   iniPos();
   evaluate();
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  LFO::iniPos
 *
 *  Desc:  Set initial oscillator position.
 *
 *  Memb: +pos              - cur position within oscillation range
 *
 *----------------------------------------------------------------------------*/
       
void LFO::iniPos()
{
   pos = 0.0;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  LFO::iniVal
 *
 *  Desc:  Set initial oscillator position and evaluate.
 *
 *----------------------------------------------------------------------------*/
       
void LFO::iniVal()
{
   iniPos();
   evaluate();
}

#ifdef KEYBRD_MENUS

/*----------------------------------------------------------------------------*
 *
 *  Name:  LFO::menu
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
 *           |  f  |   d   |     |     |   ~   |   <   |  !  |
 *           |     |       |     |     |       |       |     |
 *           |     |       |     |     |       |       |     |
 *           -------------------------------------------------
 *
 *----------------------------------------------------------------------------*/      

char LFO::menu( key k )
{
   switch ( k.position() )
   {
      case  0: return 'f';                   // set oscillator frequency
      case  2: return 'd';                   // set oscillator depth
      default: return super::menu( k );
   }
}

#endif

/*----------------------------------------------------------------------------*
 *
 *  Name:  LFO::setDepth
 *
 *  Desc:  Set the oscillation depth.
 *
 *  Args:  d                - oscillation depth 
 *
 *  Memb: +depth            - oscillation depth (0.0 - 1.0)
 *
 *----------------------------------------------------------------------------*/

void LFO::setDepth( double d )
{
   if ( d > 1.0 )
      d = 1.0;
   else if ( d < 0.0 )
      d = 0.0;
   depth = d;
   if ( d == 0.0 )
      iniPos();
   calcStep();
   charEv( lfoOnDepth );
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  LFO::setFreq
 *
 *  Desc:  Set the oscillation frequency.
 *
 *  Args:  f                - oscillation frequency 
 *
 *  Memb: +freq             - oscillation frequency
 *
 *----------------------------------------------------------------------------*/

void LFO::setFreq( double f )
{
   if ( f > MAX_LOW_FREQ )
      f = MAX_LOW_FREQ;
   else if ( f < MIN_LOW_FREQ )
      f = MIN_LOW_FREQ;
   freq = f;
   calcStep();
}

#ifdef CONSOLE_OUTPUT
const char *LFO::prompt()
{
   return CONSTR("lfo");
}
#endif

/******************************************************************************
 *
 *                               FadeLFO 
 *
 *   An LFO which can be configured to fade in or out over a specified period
 *   of time once triggered.
 *
 ******************************************************************************/

/*----------------------------------------------------------------------------*
 *
 *  Name:  FadeLFO::charEv
 *
 *  Desc:  Process a character event.
 *
 *  Args:  code             - character to process
 *
 *  Glob:  dynaRate         - dynamic udpate rate
 *
 *  Memb: +fadeStep         - increment fader this much per dynamic update
 *        +flags.DONE       - LFO is done fading
 *        +flags.FADEOUT    - LFO fades out (if not set, LFO fades in) 
 *         flags.LEGATO     - trigger only if prior fade is completed
 *        +time             - fade time in 1/8th secs (0-255) 
 *
 *  Rets:  status           - if true, character was handled
 *
 *----------------------------------------------------------------------------*/
       
boolean FadeLFO::charEv( char code )
{
   switch ( code )
   {
      case chrTrigger:                 // trigger the LFO

         if ( time )                   // on trigger-ready
         {
            iniVal();
            iniFader();
            flags &= ~DONE;
         }
         break;

      case chrRelease:

         break;

      #ifdef CONSOLE_OUTPUT
      case chrInfo:                    // display object info to console

         super::charEv( chrInfo );
         console.infoByte( CONSTR("time"), time );
         console.space();
         console.print( ( flags & FADEOUT ) ? '-' : '+' );
         break;
      #endif

      #ifdef INTERN_CONSOLE

      case '~':                        // set legato triggering
      case '\'':                       // set stacato triggering

         super::charEv( code );
         flags |= DONE;                // set for immediate re-triggering
         break;

      case '+':                        // set fade direction to "in"

         flags   &= ~FADEOUT;
         fadeStep = abs( fadeStep );
         break;

      case '-':                        // set fade direction to "out"

         flags   |= FADEOUT;
         fadeStep = -abs( fadeStep );
         break;

      case 't':                        // set fade time

         console.getByte( CONSTR("time"), &this->time );

         flags |= DONE;
         if ( time > 0 )               // if fade enabled
         {
            fadeStep = 8.0 / (time * dynaRate);
            if (flags & FADEOUT)
               fadeStep = -fadeStep;
            iniFader();
            break;
         }                             // else fall thru
      #endif

      case '!':                        // reset

         flags   &= ~(DONE|FADEOUT);   
         time     = 0;                 
         fadeStep = 0.0;               
         iniFader();

         // fall thru to superclass handler

      default:

         return super::charEv( code );
   }
   return true;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  FadeLFO::dynamics
 *
 *  Desc:  Perform a dynamic update by bumping the LFO position, bumping the 
 *         fader value, and setting the DONE bit if the fade has completed.
 *
 *  Memb: +fader            - current fader value
 *         fadeStep         - increment fader this much per dynamic update
 *        +flags.DONE       - LFO is done fading
 *         flags.FADEOUT    - LFO fades out (if not set, LFO fades in) 
 *
 *----------------------------------------------------------------------------*/      

void FadeLFO::dynamics()
{
   super::dynamics();
   fader += fadeStep;
   if (flags & FADEOUT)
   {
      if ( fader < 0.0 ) 
      {
         fader = 0.0;
         flags |= DONE;
      }
   }
   else
   {
      if ( fader > 1.0 ) 
      {
         fader = 1.0;
         flags |= DONE;
      }
   }
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  FadeLFO::iniFader
 *
 *  Desc:  Set initial fader value.
 *
 *  Memb: +fader            - current fader value
 *         flags.FADEOUT    - LFO fades out (if not set, LFO fades in) 
 *         time             - fade time in 1/8th secs (0-255) 
 *
 *----------------------------------------------------------------------------*/
       
void FadeLFO::iniFader()
{
   if ( time == 0 )
      fader = 1.0;
   else
      fader = (flags & FADEOUT) ? 1.0 : 0.0;
}

#ifdef KEYBRD_MENUS

/*----------------------------------------------------------------------------*
 *
 *  Name:  FadeLFO::menu
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
 *           |   |   |   |   |   |   | + |   | ' |   | . |   | 
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |    ___     ___    |    ___     ___     ___    | 
 *           |     |       |     |     |       |       |     |
 *           |     |       |     |     |       |       |     |
 *           |  f  |   d   |  t  |  -  |   ~   |   <   |  !  |
 *           |     |       |     |     |       |       |     |
 *           |     |       |     |     |       |       |     |
 *           -------------------------------------------------
 *
 *----------------------------------------------------------------------------*/      

char FadeLFO::menu( key k )
{
   switch ( k.position() )
   {
      case  4: return 't';
      case  5: return '-';
      case  6: return '+';
      default: return super::menu( k );
   }
}

#endif

/******************************************************************************
 *
 *                               TermLFO 
 *
 *   "Terminating LFO" - an LFO which can be configured to run for a specified 
 *   number of half-cycles once triggered.
 *
 ******************************************************************************/

/*----------------------------------------------------------------------------*
 *
 *  Name:  TermLFO::charEv
 *
 *  Desc:  Process a character event.
 *
 *  Args:  code             - character to process
 *
 *  Memb: +flags.DONE       - LFO is done traversing
 *         flags.LEGATO     - trigger only if prior traversal has completed
 *        +flags.PEAK       - trigger LFO starting at peak value 
 *        +runDC            - downcounter (in half-cycles) to end of traversal
 *        +trav             - traverse this many half-cycles (0 = run forever)
 *
 *  Rets:  status           - if true, character was handled
 *
 *----------------------------------------------------------------------------*/
       
boolean TermLFO::charEv( char code )
{
   switch ( code )
   {
      case chrTrigger:                 // trigger the LFO

         if ( trav )                   // on trigger-ready
         {
            runDC = trav;              // load running downcounter
            iniVal();                  // reset oscillator position
            flags &= ~DONE;            // "not done"
         }
         break;

      case 't':                        // set traversal value

         console.getByte( CONSTR("trig"), &this->trav );

         if ( trav == 0 )              // if now free-running
         {
            flags &= ~DONE;            // insure "not done"
            runDC = 0;                 // insure "not traversing"
         }

         break;

      #ifdef INTERN_CONSOLE
      case '+':                        // trigger begins at "peak" value

         flags   |= PEAK;
         break;

      case '-':                        // trigger begins at "trough" value

         flags   &= ~PEAK;
         break;
      #endif

      #ifdef CONSOLE_OUTPUT
      case chrInfo:                    // display object info to console

         super::charEv( chrInfo );
         console.infoByte( CONSTR("trig"), trav );
         console.space();
         console.print( ( flags & PEAK ) ? '+' : '-' );
         break;
      #endif

      case '!':                        // reset

         flags &= ~(DONE|PEAK);        // "not done, trig from trough"
         trav  = 0;                    // "free-running"
         runDC = 0;                    // "not traversing"
                                       // fall thru to super reset
      default:

         return super::charEv( code );
   }
   return true;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  TermLFO::dynamics
 *
 *  Desc:  Perform a dynamic update by bumping the LFO position, uodating the
 *         traversal downcounter, and setting the DONE bit if the traversal 
 *         has completed.
 *
 *  Memb:  depth            - oscillation depth (0.0 - 1.0)
 *        +flags.DONE       - LFO is done traversing
 *         flags.MUTE       - LFO is muted
 *        +pos              - cur position within oscillation range
 *        +runDC            - downcounter (in half-cycles) to end of traversal
 *         step             - change in osc position per dynamic update
 *         trav             - traverse this many half-cycles (0 = run forever)
 *
 *----------------------------------------------------------------------------*/
       
void TermLFO::dynamics()
{
   if ( flags&MUTE || (trav && flags&DONE) )  // return if LFO is muted or done
   {
      return;
   }

   pos += step;                        // bump oscillator position

   // if bounds exceeded, turn oscillator back and update traversal state

   if ( pos > depth )
   {
      pos  = 2 * depth - pos;
      step = -step;
      if ( runDC && --runDC == 0 )     // if run just completed
      {
         pos = depth;                  // ... peg to max
         flags |= DONE;  
      }
   }
   else if ( pos < 0.0 )
   {
      pos  = - pos;
      step = -step;
      if ( runDC && --runDC == 0 )     // if run just completed
      {
         pos = 0.0;                    // ... peg to min
         flags |= DONE;  
      }
   }

   evaluate();                         // set output value
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  TermLFO::iniPos
 *
 *  Desc:  Set initial oscillator position.
 *
 *  Memb:  depth            - oscillation depth (0.0 - 1.0)
 *         flags.PEAK       - trigger LFO starting at peak value 
 *        +pos              - cur position within oscillation range
 *        +step             - change in osc position per dynamic update
 *
 *----------------------------------------------------------------------------*/
       
void TermLFO::iniPos()
{
   if ( flags&PEAK )
   {
      pos  = depth;                    // start at peak
      step = -abs(step);               // insure decreasing initial step
   }
   else
   {
      pos  = 0.0;                      // start at trough
      step = abs(step);                // insure increasing initial step
   }
}

#ifdef KEYBRD_MENUS

/*----------------------------------------------------------------------------*
 *
 *  Name:  TermLFO::menu
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
 *           |   |   |   |   |   |   | + |   | ' |   | . |   | 
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |    ___     ___    |    ___     ___     ___    | 
 *           |     |       |     |     |       |       |     |
 *           |     |       |     |     |       |       |     |
 *           |  f  |   d   |  t  |  -  |   ~   |   <   |  !  |
 *           |     |       |     |     |       |       |     |
 *           |     |       |     |     |       |       |     |
 *           -------------------------------------------------
 *
 *----------------------------------------------------------------------------*/      

char TermLFO::menu( key k )
{
   switch ( k.position() )
   {
      case  4: return 't';
      case  5: return '-';
      case  6: return '+';
      default: return super::menu( k );
   }
}

#endif

