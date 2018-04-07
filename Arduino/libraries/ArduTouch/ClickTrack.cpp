/*
    ClickTrack.cpp  

    Implementation of the ClickTrack class.

    ---------------------------------------------------------------------------
 
    Copyright (C) 2018, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
    ---------------------------------------------------------------------------
*/

#include "ClickTrack.h"

/*----------------------------------------------------------------------------*
 *
 *  Name:  ClickTrack::charEv
 *
 *  Desc:  Process a character event.
 *
 *  Args:  code             - character to process
 *
 *  Rets:  status           - true if character was handled
 *
 *----------------------------------------------------------------------------*/      

bool ClickTrack::charEv( char code )      
{
   switch ( code )                     
   {
      case '!':                        // perform a reset

         super::charEv( code );
         setGlobVol( 255 );

         // the following macro
         //    1) enables the local keybrd so that VoxSynth::noteOn()
         //       will not propagate notes to it
         //    2) sets the envelope sustain to 0
         //    3) sets the envelope decay to 4

         execute( PSTR( "k<`es0\\d4\\" ) );  
         break;

      default:                         
         
         return super::charEv( code );
   }
   return true;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  ClickTrack::downbeat
 *
 *  Desc:  Strike a downbeat.
 *
 *----------------------------------------------------------------------------*/

void ClickTrack::downbeat()
{
   noteOn( key( 45 ) );             // play higher-pitched noise
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  ClickTrack::upbeat
 *
 *  Desc:  Strike an upbeat.
 *
 *----------------------------------------------------------------------------*/

void ClickTrack::upbeat()
{
   noteOn( key( 30 ) );             // play lower-pitched noise
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  ClickTrack::dynamics
 *
 *  Desc:  Perform a dynamic update.
 *
 *  Memb:  ampMods          - amplitude modifiers
 *        +flags.FREQ       - if set, an ideal frequency change is pending
 *        +flags.TRIG       - if set, a trigger is pending
 *         osc              - ptr to resident oscillator
 *        +segVol           - if segue in process, ultimate instVol
 *
 *----------------------------------------------------------------------------*/

void ClickTrack::dynamics()
{
   if ( flags & FREQ )              // handle pending frequency change
   {
      osc->setFreq( pendFreq );
      flags &= ~FREQ;               
   }

   if ( flags & TRIG )              // handle pending trigger
   {
      osc->trigger();               
      ampMods.trigger();
      flags &= ~TRIG;               
   }
   else                             // update components
   {
      osc->dynamics();
      ampMods.dynamics();
   }

   /* manage instantaneous volume */

   /*
   if ( muted() )
   {
      segVol.val = 0;
   }
   else
   */
   {
      segVol.val = 256;
      segVol.val *= ampMods.value();
   }
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  ClickTrack::output
 *
 *  Desc:  Write output to an audio buffer.
 *
 *  Args:  buf              - ptr to audio buffer  
 *
 *  Glob:  audioBufSz       - size of system audio buffers
 *
 *  Memb:  osc              - ptr to resident oscillator
 *         segVol           - if segue in process, ultimate instVol
 *
 *----------------------------------------------------------------------------*/

void ClickTrack::output( char *buf )        
{
   byte numrecs = audioBufSz;     

   osc->output( buf );

   Int amp;
   while ( numrecs-- ) 
   {
      amp.val  = segVol.val;
      amp.val *= *buf;
      *buf++   = amp._.msb;
   }
}



