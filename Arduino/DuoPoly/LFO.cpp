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

#define MAX_LOW_FREQ    6.0      // maximum freq  of oscillation
#define MIN_LOW_FREQ    0.01     // minimum freq  of oscillation
#define DEF_LOW_FREQ    3.8      // default freq  of oscillation


extern double dynaRate;

                        /*--------------------
                                LFO
                         --------------------*/

void LFO::calcStep()
{
   step = 2.0 * depth * freq / dynaRate;
}

void LFO::charEv( char key )
{
   switch ( key )
   {
      case 'd':                  // set depth of oscillation

         if ( console.getDouble( "depth" ) )
            setDepth( lastDouble );
         break;

      case 'f':                  // set frequency of oscillation

         if ( console.getDouble( "freq" ) )
            setFreq( lastDouble );
         break;

      case '.':                  // mute

         iniPos();               // set to initial position
         evaluate();             // compute output @ iniPos
         Control::charEv( key ); 
         break;

      case '!':                  // reset

         iniOsc( DEF_DEPTH, DEF_LOW_FREQ );
         // fall thru to Control handler

      default:

         Control::charEv( key );
   }
}

void LFO::dynamics()
{
   if ( ! amMute() )
   {
      pos += step;               // bump oscillator position

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
      evaluate();                // set output value
   }
}

void LFO::evaluate()
{
   val = pos;
}

void LFO::info()
{
   Control::info();
   console.infoDouble( "freq", freq );
   console.infoDouble( "depth", depth );
}

void LFO::iniPos()
{
   pos = 0.0;
}

char LFO::menu( key k )
{
   switch ( k.position() )
   {
      case  0: return 'f';
      case  2: return 'd';
      default: return Control::menu( k );
   }
}

void LFO::setDepth( double d )
{
   if ( d > 1.0 )
      d = 1.0;
   else if ( d < 0.0 )
      d = 0.0;
   depth = d;
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

void LFO::iniOsc( double d, double f )
{
   freq = f;
   setDepth( d );
   iniPos();
   evaluate();
}

                        /*--------------------
                                FadeLFO
                         --------------------*/

void FadeLFO::charEv( char code )
{
   switch ( code )
   {
      case cmdTrigger:                 // trigger the LFO 

         if ( time )                   // if fade enabled 
         {
            iniPos();          
            fader = (flags & FADEOUT) ? 1.0 : 0.0; 
         }
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

         console.getByte( "time", &this->time );

         if ( time > 0 )               // if fade enabled
         {
            fadeStep = 8.0 / (time * dynaRate);
            if (flags & FADEOUT)
            {
               fader    = 1.0;
               fadeStep = -fadeStep;
            }
            else
               fader    = 0.0;
            break;
         }                             // else fall thru

      case '!':                        // reset 

         time     = 0;                 // "fading disabled"
         fader    = 1.0;               // 
         fadeStep = 0.0;               //
                                       // fall thru to LFO handler 
      default:

         LFO::charEv( code );
   }
}

void FadeLFO::dynamics()
{
   LFO::dynamics();
   fader += fadeStep;
   if (flags & FADEOUT)
   {
      if ( fader < 0.0 ) fader = 0.0;
   }
   else
   { 
      if ( fader > 1.0 ) fader = 1.0;
   }
}

void FadeLFO::info()
{
   LFO::info();
   console.infoByte( "time", time );
   console.space();
   console.print( ( flags & FADEOUT ) ? '-' : '+' );
}

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

                        /*--------------------
                                TrigLFO
                         --------------------*/

void TrigLFO::charEv( char code )
{
   switch ( code )
   {
      case cmdTrigger:                 // trigger the LFO 

         if ( trav && ( ! runDC || ! (flags&LEGATO) ) ) 
         {
            runDC = trav;              // load running downcounter
            iniPos();                  // reset oscillator position
         }
         break;

      case 'L':

         flags   |= LEGATO;            // set legato triggering
         break;

      case 'S':

         flags   &= ~LEGATO;           // set stacato triggering
         break;

      case 't':                        // set traversal value 

         console.getByte( "trig", &this->trav );

         if ( trav == 0 )              // if now free-running
            runDC = 0;                 // insure "not traversing"

         break;

      case '+':                        // trigger begins at "peak" value

         flags   |= PEAK;
         break;

      case '-':                        // trigger begins at "trough" value

         flags   &= ~PEAK;
         break;

      case '!':                        // reset 

         flags &= ~(LEGATO|PEAK);      // "not stopped, trig from trough" 
         trav  = 0;                    // "free-running"
         runDC = 0;                    // "not traversing"
                                       // fall thru to LFO reset 
      default:

         LFO::charEv( code );
   }
}

void TrigLFO::dynamics()
{
   if ( amMute() | ! runDC )           // return if LFO is muted or stopped
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
         pos = depth;                  // ... peg to max
   }
   else if ( pos < 0.0 )
   {
      pos  = - pos;                    
      step = -step;
      if ( runDC && --runDC == 0 )     // if run just completed
         pos = 0.0;                    // ... peg to min
   }

   evaluate();                         // set output value
}

void TrigLFO::info()
{
   LFO::info();
   console.infoByte( "trig", trav );
   console.space();
   console.print( ( flags & PEAK ) ? '+' : '-' );
   console.print( ( flags & LEGATO ) ? 'L' : 'S' );
}

void TrigLFO::iniPos()
{
   if ( flags & PEAK )
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

char TrigLFO::menu( key k )
{
   switch ( k.position() )
   {
      case  4: return 't';
      case  5: return '-';
      case  6: return '+';
      case  7: return 'L';
      case  8: return 'S';
      default: return LFO::menu( k );
   }
}

