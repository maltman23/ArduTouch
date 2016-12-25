/*
    Osc.cpp  

    Implementation of the Osc class (abstract oscillator).

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
#include "Osc.h"

/*----------------------------------------------------------------------------*
 *
 *  Name:  Osc::calcEffFreq
 *
 *  Desc:  Calculate the effective frequency.
 *
 *  Memb:  detune           - local detuning amount
 *        +effFreq          - effective frequency (includes local detuning)
 *         idealFreq        - ideal frequency (sans local detuning)
 *
 *----------------------------------------------------------------------------*/      

void Osc::calcEffFreq()
{
   #define RATIO_MAXDETUNE     1.014545   // an eight-tone 
   #define INVERT_MAXDETUNE     .985663   // 1 / RATIO_MAXDETUNE
   #define ONETWENTYEIGHTH     .0078125   // 1 / 128

   double factor = 1.0;                   // detuning factor

   if ( detune >= 0 )
      factor += detune * ONETWENTYEIGHTH * (RATIO_MAXDETUNE-1.0);
   else
      factor += detune * ONETWENTYEIGHTH * (1.0-INVERT_MAXDETUNE);

   effFreq = idealFreq * factor;
   onFreq();
}

boolean Osc::charEv( char code )
{
   switch ( code )
   {
      #ifdef INTERN_CONSOLE
      case 'd':                     // set detuning amount

         console.getSByte( CONSTR("detune"), &this->detune );
         setDetune( detune );
         break;

      case 'f':                     // input an ideal frequency
      {
         double newFreq;
         if ( console.getDouble( CONSTR("freq"), &newFreq ) )
            setFreq( newFreq );
         break;
      }
      #endif

      #ifdef CONSOLE_OUTPUT

      case chrInfo:                 // display object info to console

         TControl::charEv( code );

      case chrInLnfo:               // display object info inline to console

         console.infoDouble( CONSTR("freq"), effFreq );
         console.infoInt( CONSTR("detune"), detune );
         break;

      #endif

      case '!':                     // perform a reset

         TControl::charEv('!');
         setDetune(0);
         extFactor = 1.0;
         break;

      default:

         return TControl::charEv( code );
   }
   return true;
}

double Osc::getFreq()
{
   return idealFreq;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Osc::modFreq
 *
 *  Desc:  Modify the instantaneous frequency by applying a factor to 
 *         the effective frequency.
 *
 *  Args:  factor           - factor to apply
 *
 *  Memb:  coeff            - step = freq * coeff
 *         effFreq          - effective frequency (includes internal detuning)
 *        +extFactor        - external detuning factor
 *        +step             - amount to increment idx per tick
 *
 *----------------------------------------------------------------------------*/      

void Osc::modFreq( double factor )
{
   extFactor = factor;
   onFreq();
}

#ifdef CONSOLE_OUTPUT
const char *Osc::prompt()                     
{
   return CONSTR("osc");
}
#endif

/*----------------------------------------------------------------------------*
 *
 *  Name:  Osc::setDetune
 *
 *  Desc:  Set the detuning amount
 *
 *  Args:  val              - amount to detune by (-128 to 127)
 *
 *  Memb: +detune           - detuning amount
 *
 *----------------------------------------------------------------------------*/      

void Osc::setDetune( char val )
{
   detune = val;
   calcEffFreq();  
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Osc::setFreq
 *
 *  Desc:  Set the (ideal) frequency.
 *
 *  Args:  freq             - new frequency
 *
 *  Memb:  effFreq          - effective frequency (includes detuning)
 *        +idealFreq        - ideal frequency (sans detuning)
 *
 *----------------------------------------------------------------------------*/      

void Osc::setFreq( double newFreq )
{
   idealFreq = newFreq;
   calcEffFreq();
}
