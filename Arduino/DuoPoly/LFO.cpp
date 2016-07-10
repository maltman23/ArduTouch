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
#include "LFO.h"

#define DEF_DEPTH       0.25     // default depth of oscillation

#define MAX_LOW_FREQ   20.0      // maximum freq  of oscillation
#define MIN_LOW_FREQ    0.01     // minimum freq  of oscillation
#define DEF_LOW_FREQ    3.8      // default freq  of oscillation


extern double dynaRate;

                        /*--------------------
                                LFO
                         --------------------*/

void LFO::calcStep()
{
   /* calculate the step, preserving whatever the current direction */

   boolean decreasing = ( step < 0.0 );
   step = 2.0 * depth * freq / dynaRate;
   if ( decreasing )
      step = -step;
}

boolean LFO::charEv( char key )
{
   switch ( key )
   {
      #ifdef INTERN_CONSOLE
      case 'd':                        // set depth of oscillation

         if ( console.getDouble( CONSTR("depth") ) )
            setDepth( lastDouble );
         break;

      case 'f':                        // set frequency of oscillation

         if ( console.getDouble( CONSTR("freq") ) )
            setFreq( lastDouble );
         break;
      #endif

      #ifdef CONSOLE_OUTPUT
      case chrInfo:                    // display object info to console

         TControl::charEv( chrInfo );
         console.infoDouble( CONSTR("freq"), freq );
         console.infoDouble( CONSTR("depth"), depth );
         break;
      #endif

      case '.':                        // mute

         iniVal();                     // set to initial position
         TControl::charEv( key );
         break;

      case '!':                        // reset

         iniOsc( DEF_DEPTH, DEF_LOW_FREQ );
         // fall thru to Control handler

      default:

         return TControl::charEv( key );
   }
   return true;
}

void LFO::dynamics()
{
   if ( ! amMute() )
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

void LFO::evaluate()
{
   val = pos;
}

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

         return TControl::evHandler(ev);
   }
}

void LFO::iniOsc( double d, double f )
{
   freq = f;
   setDepth( d );
   iniPos();
   evaluate();
}

void LFO::iniPos()
{
   pos = 0.0;
}

void LFO::iniVal()
{
   iniPos();
   evaluate();
}

#ifdef KEYBRD_MENUS
char LFO::menu( key k )
{
   switch ( k.position() )
   {
      case  0: return 'f';
      case  2: return 'd';
      default: return Control::menu( k );
   }
}
#endif

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
}

void LFO::setFreq( double f )
{
   if ( f > MAX_LOW_FREQ )
      f = MAX_LOW_FREQ;
   else if ( f < MIN_LOW_FREQ )
      f = MIN_LOW_FREQ;
   freq = f;
   calcStep();
}

                        /*--------------------
                                FadeLFO
                         --------------------*/

boolean FadeLFO::charEv( char code )
{
   switch ( code )
   {
      case chrTrigger:                 // trigger the LFO

         iniVal();
         iniFader();
         flags &= ~DONE;
         break;

      #ifdef CONSOLE_OUTPUT
      case chrInfo:                    // display object info to console

         LFO::charEv( chrInfo );
         console.infoByte( CONSTR("time"), time );
         console.space();
         console.print( ( flags & FADEOUT ) ? '-' : '+' );
         break;
      #endif

      #ifdef INTERN_CONSOLE
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

         if ( time > 0 )               // if fade enabled
         {
            flags &= ~DONE;
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

         // fall thru to LFO handler

      default:

         return LFO::charEv( code );
   }
   return true;
}

void FadeLFO::dynamics()
{
   LFO::dynamics();
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

void FadeLFO::iniFader()
{
   if ( time == 0 )
      fader = 1.0;
   else
      fader = (flags & FADEOUT) ? 1.0 : 0.0;
}

#ifdef KEYBRD_MENUS
char FadeLFO::menu( key k )
{
   switch ( k.position() )
   {
      case  4: return 't';
      case  5: return '-';
      case  6: return '+';
      default: return LFO::menu( k );
   }
}
#endif

boolean FadeLFO::ready()
{
   if ( ! time )
      return false;
   else
      return ( ! flags&LEGATO || flags&DONE );
}

                        /*--------------------
                                TrigLFO
                         --------------------*/

boolean TrigLFO::charEv( char code )
{
   switch ( code )
   {
      case chrTrigger:                 // trigger the LFO

         runDC = trav;                 // load running downcounter
         flags &= ~DONE;               // "not done"
         iniVal();                     // reset oscillator position
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

         LFO::charEv( chrInfo );
         console.infoByte( CONSTR("trig"), trav );
         console.space();
         console.print( ( flags & PEAK ) ? '+' : '-' );
         break;
      #endif

      case '!':                        // reset

         flags &= ~(DONE|PEAK);        // "not done, trig from trough"
         trav  = 0;                    // "free-running"
         runDC = 0;                    // "not traversing"
                                       // fall thru to LFO reset
      default:

         return LFO::charEv( code );
   }
   return true;
}

void TrigLFO::dynamics()
{
   if ( flags & (MUTE|DONE) )          // return if LFO is muted or done
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

void TrigLFO::iniPos()
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
char TrigLFO::menu( key k )
{
   switch ( k.position() )
   {
      case  4: return 't';
      case  5: return '-';
      case  6: return '+';
      default: return LFO::menu( k );
   }
}
#endif

boolean TrigLFO::ready()
{
   return ( trav && ( ! runDC || ! (flags&LEGATO) ) );
}
