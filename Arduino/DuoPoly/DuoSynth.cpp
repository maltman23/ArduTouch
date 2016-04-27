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
#include "DuoSynth.h"
#include "Filter.h"

ChnlSqnc left, rght;
BSFilter bsfL, bsfR;
XMFilter xmfL, xmfR;

DuoSynth::DuoSynth()
{

   left.setName( CONSTR("left") );     // set up left channel
   left.other = &rght;                 
   left.effects.append( &bsfL );
   left.effects.append( &xmfL );

   rght.setName( CONSTR("right") );    // set up right channel
   rght.other = &left;                 
   rght.effects.append( &bsfR );
   rght.effects.append( &xmfR );

   secDC = ticksPerSec;          
}

void DuoSynth::butEv( but b )
{
   byte      butNum  = b.num();
   butEvType butType = b.type();

   switch ( butType )
   {
      case butPRESS:  
      case butTAP:

         console.pushMode( butNum ? &rght : &left );
         break;

      default:                 

         Synth::butEv( b );
   }
}

void DuoSynth::charEv( char code )
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
            rght.setWave( wavebank.choice() );
            left.setWave( wavebank.choice() );
         }
         break;

      case '-':
            
         rght.setFreqDiff( rght.getFreq() - left.getFreq() );
         newline_info_prompt();
         break;

      case '/':

         rght.setFreqRatio( rght.getFreq() / left.getFreq() );
         newline_info_prompt();
         break;

      case 'u':                        // unlatch 
            
         rght.setFreqLatch( false );
         newline_info_prompt();
         break;

      case '|':                        // propagate transport commands
      case '[':
      case ']':

         rght.charEv( code );
         left.charEv( code );
         break;

      case '!':                        // reset
      
      {  const double initHz   = 311.13;      // initial left channel frequency
         const double ratio5th = 1.4983;      // initial left-right freq ratio 

         left.reset();
         left.setWave( 0 );
         left.setFreq( initHz );

         rght.reset();
         rght.setWave( 0 );
         rght.setFreq( initHz * ratio5th ); }

      default:

         Synth::charEv( code );
   }
}

void DuoSynth::dynamics()                     
{                                      
   left.dynamics();
   rght.dynamics();
}

void DuoSynth::generate( char *bufL, char* bufR )                     
{           
   if ( amMute() ) 
   {
      byte icnt = audio::bufSz;     
      while ( icnt-- ) 
      {
         *bufL++ = 0;
         *bufR++ = 0;
      }
   }
   else
   {
      left.generate( bufL );
      rght.generate( bufR );
   }
}

void DuoSynth::info()
{
   Synth::info();

   left.printNameFreq();
   console.space();
   console.print( left.latchIcon() );
   console.space();
   rght.printNameFreq();
}

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

char *DuoSynth::prompt()
{
   return CONSTR("main");
}



