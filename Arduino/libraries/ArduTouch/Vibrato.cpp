/*
    Vibrato.cpp  

    Implementation of Vibrato classes.

    ---------------------------------------------------------------------------
 
    Copyright (C) 2018, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
   ---------------------------------------------------------------------------
*/

#include "types.h"
#include "Console_.h"
#include "Vibrato.h"

/******************************************************************************
 *
 *                               Vibrato 
 *                                                                            
 ******************************************************************************/

/*----------------------------------------------------------------------------*
 *
 *  Name:  Vibrato::charEv
 *
 *  Desc:  Process a character event.
 *
 *  Args:  code             - character to process
 *
 *  Rets:  status           - if true, character was handled
 *
 *----------------------------------------------------------------------------*/
       
boolean Vibrato::charEv( char code )
{
   switch ( code )
   {
      case '!':                        // reset

         super::charEv( code );
         setSigned( true );
         iniVal();
         break;

      default:

         return super::charEv( code );
   }
   return true;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Vibrato::evaluate
 *
 *  Desc:  Compute output value based on oscillator position.
 *
 *  Memb: +value            - current output value
 *
 *----------------------------------------------------------------------------*/

void Vibrato::evaluate()
{
   super::evaluate();

   if ( value >= 0.0 )
      value *= (RATIO_SEMITONE-1.0);
   else
      value *= (1.0-INVERT_SEMITONE);
   value += 1.0;
}


/******************************************************************************
 *
 *                               DelayVibrato 
 *
 ******************************************************************************/

/*----------------------------------------------------------------------------*
 *
 *  Name:  DelayVibrato::charEv
 *
 *  Desc:  Process a character event.
 *
 *  Args:  code             - character to process
 *
 *  Memb: +delay            - delay this many updates before engaging vibrato
 *        +delayDC          - down counter to engaging vibrato
 *
 *  Rets:  status           - true if character was handled
 *
 *  Note:  The "delay" value as displayed and input via the console is 1/4 
 *         the internal value.
 *
 *----------------------------------------------------------------------------*/      

bool DelayVibrato::charEv( char code )         
{                                      
   switch ( code )
   {
      case chrTrigger:           // trigger vibrato

         if ( delay )
            delayDC = delay;
         else
            super::charEv( code );
         break;

      #ifdef INTERN_CONSOLE

      case 'D':                  // set delay
      {
         byte userInput;
         if ( console.getByte( CONSTR("Delay"), &userInput ) )
         delay = userInput;
         delay << 2;             // *4 yields internal value
         break;
      }

      #endif   // INTERN_CONSOLE

      #ifdef CONSOLE_OUTPUT

      case chrInfo:
      {
         super::charEv( code );
         console.newlntab();

         Word uiDelay;
         uiDelay.val = delay;
         uiDelay.val >> 2;      // divide by 4 to get user interface value

         console.infoByte( CONSTR("Delay"), uiDelay._.lsb );

         break;
      }

      #endif   // CONSOLE_OUTPUT
      
      case '!':                  // perform a reset

         super::charEv( code );
         delay   = 0;
         delayDC = 0;
         break;

      default:                         

         return super::charEv( code );
   }
   return true;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  DelayVibrato::dynamics
 *
 *  Desc:  Perform a dynamic update.
 *
 *  Memb: +delayDC          - down counter to engaging vibrato
 *
 *----------------------------------------------------------------------------*/      

void DelayVibrato::dynamics()
{
   if ( delayDC )
   {
      if ( --delayDC ) 
         return;
      super::charEv( chrTrigger );
   }
   super::dynamics();
}

