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

