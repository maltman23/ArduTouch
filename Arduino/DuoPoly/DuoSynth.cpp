/*
    DuoSynth.cpp  

    Implementation of the DuoSynth class.

    ---------------------------------------------------------------------------
 
    Copyright (C) 2016, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
   ---------------------------------------------------------------------------
*/

#include "Audio.h"
#include "Console_.h"
#include "Chnl.h"
#include "WaveBank.h"
#include "WaveOsc.h"
#include "DuoSynth.h"
#include "StdEffects.h"
#include "System.h"

ChnlSqnc left, rght;
WaveOsc  oscL, oscR;
BSFilter bsfL, bsfR;
XMFilter xmfL, xmfR;

DuoSynth::DuoSynth()
{
   flags &= ~PLAYTHRU;

   left.osc   = &oscL;
   left.other = &rght;                 
   left.led   = LEFT_LED;
   left.effects.append( &bsfL );
   left.effects.append( &xmfL );
   left.setName( CONSTR("left") );     

   rght.osc   = &oscR;
   rght.other = &left;                 
   rght.led   = RIGHT_LED;
   rght.effects.append( &bsfR );
   rght.effects.append( &xmfR );
   rght.setName( CONSTR("right") );    

}

boolean DuoSynth::charEv( char code )
{
   switch ( code )
   {
     case 'l':

         console.pushMode( &left );
         break;

      case 'r':

         console.pushMode( &rght );
         break;

      case 'w':

         if ( wavebank.choose() )
         {
            ((WaveOsc *)rght.osc)->setTableFromBank( wavebank.choice() );
            ((WaveOsc *)left.osc)->setTableFromBank( wavebank.choice() );
         }
         break;

      case '-':
            
         rght.setFreqDiff( rght.osc->getFreq() - left.osc->getFreq() );
         inform();
         break;

      case '/':

         rght.setFreqRatio( rght.osc->getFreq() / left.osc->getFreq() );
         inform();
         break;

      case 'u':                        // unlatch 
            
         rght.setFreqLatch( false );
         inform();
         break;

      case '|':                        // propagate transport commands
      case '[':
      case ']':

         rght.charEv( code );
         left.charEv( code );
         break;

      case '.':                        // mute
      case '<':                        // unmute

      {
         byte chnlVol = ( code == '.' ? 0 : vol );
         left.setGlobVol( chnlVol );
         rght.setGlobVol( chnlVol );
         Synth::charEv( code );
         break;
      }

      #ifdef CONSOLE_OUTPUT
      case chrInfo:                    // display object info to console

         Synth::charEv( chrInfo );
         left.brief();
         console.print( left.latchIcon() );
         console.space();
         rght.brief();
         break;
      #endif

      case '!':                        // reset
      
      {  const double initHz   = 311.13;      // initial left channel frequency
         const double ratio5th = 1.4983;      // initial left-right freq ratio 

         left.reset();
         ((WaveOsc *)left.osc)->setTableFromBank( 0 );
         left.setFreq( initHz );

         rght.reset();
         ((WaveOsc *)rght.osc)->setTableFromBank( 0 );
         rght.setFreq( initHz * ratio5th ); 
      }

      default:

         return Synth::charEv( code );
   }
   return true;
}

void DuoSynth::dynamics()                     
{                                      
   left.dynamics();
   rght.dynamics();
}

boolean DuoSynth::evHandler( obEvent ev )
{
   switch ( ev.type() )
   {
      case BUT0_PRESS:

         console.pushMode( &left );
         return true;

      case BUT1_PRESS:

         console.pushMode( &rght );
         return true;

      default:

         return Synth::evHandler( ev );
   }
}

void DuoSynth::onFocus( focus f )
{
   if ( f == focusRESTORE )
   {
      system::offLED( LEFT_LED );
      system::offLED( RIGHT_LED );
   }
}

void DuoSynth::output( char *bufL, char* bufR )                     
{           
   left.output( bufL );
   rght.output( bufR );
}

#ifdef KEYBRD_MENUS
char DuoSynth::menu( key k )
{
   switch ( k.position() )
   {
      case  0: return 'p';
      case  2: return 'w';
      case  3: return 'u';
      case  4: return '-';
      case  5: return '/';
      case  6: return '|';
      case  7: return '[';
      case  8: return ']';
      default: return Synth::menu(k);
   }
}
#endif

#ifdef CONSOLE_OUTPUT
const char *DuoSynth::prompt()
{
   return CONSTR("main");
}
#endif

/*----------------------------------------------------------------------------*
 *
 *  Name:  DuoSynth::setVol
 *
 *  Desc:  Set the volume level.
 *
 *  Args:  vol              - volume level
 *
 *  Memb: +vol              - volume level (0:255)
 *
 *----------------------------------------------------------------------------*/      

void DuoSynth::setVol( byte x )
{
   Synth::setVol(x);
   left.setGlobVol(x);
   rght.setGlobVol(x);
}

void DuoSynth::startup()
{
   Synth::startup();
   console.pushMode( &rght );
}

