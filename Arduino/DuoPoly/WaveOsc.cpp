/*
    WaveOsc.cpp  

    Implementation of the WaveOsc and auxillary classes.

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
#include "ConKeyBrd.h"
#include "WaveOsc.h"
#include "WaveBank.h"

                        /*--------------------
                                 WaveOsc
                         --------------------*/

void WaveOsc::butEv( but b )
{
   byte      butNum  = b.num();
   butEvType butType = b.type();

   switch ( butType )
   {
      case butTAP:               // bump keybrd up(but 1) - down(but 0) 1 octave 

         keybrd.bumpOctave( butNum );
         break;

      default:                 

         MonoPhonic::butEv( b );
   }
}

void WaveOsc::charEv( char code )
{
   switch ( code )
   {
      case 'f':                  // input a playback frequency

         if ( console.getDouble( "freq" ) )
            setFreq( lastDouble );
         break;

      case 'T':                  // push tremolo control

         console.pushMode( &this->tremolo );
         break;

      case 'v':                  // input a volume level

         if ( console.getDouble( "vol" ) )
            setVol( lastDouble );
         break;

      case 'V':                  // push vibrato control

         console.pushMode( &this->vibrato );
         break;

      case 'w':                  // select a waveform

         if ( wavebank.choose() )
            setWave( wavebank.choice() );
         break;

      case '!':                  // reset

         tremolo.reset();
         vibrato.reset();
         setVol( 1.0 );
         // fall thru to MonoPhonic handler

      default:

         MonoPhonic::charEv( code );
   }
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  WaveOsc::dynamics
 *
 *  Desc:  Perform a dynamic update of the tremolo and vibrato controls,
 *         and propagate changes to instantaneous volume and frequency.
 *
 *  Args:  -- none --
 *
 *  Memb:  coeff            - step = freq * coeff
 *         freq             - ideal playback frequency
 *        +instVol          - instantaneous volume
 *        +step             - amount to increment idx per tick
 *         vol              - volume level (0.0 - 1.0)
 *
 *----------------------------------------------------------------------------*/      

void WaveOsc::dynamics()
{
   /* manage instantaneous volume 
   
      evScalar is an 8-bit fraction (0 - 0.5) that accounts for both volume
      level and tremolo. It is pre-divided by 2 for use in WaveOsc::evaluate() 
      -- see further comments there. 
   */

   tremolo.dynamics();
   instVol.val = vol * tremolo.val * 128.0;

   /* manage instantaneous frequency */

   vibrato.dynamics();
   step = freq * vibrato.val * coeff;
}

double WaveOsc::getFreq()
{
   return freq;
}

void WaveOsc::info()
{
   MonoPhonic::info();
   printNameFreq();
   console.infoDouble( " vol", vol );
}


void WaveOsc::printNameFreq()
{
   console.romprint( name );
   console.space();
   console.print( toStr( getFreq()) );
}

void WaveOsc::setFreq( double x )
{
   freq = x;
   step = x * vibrato.val * coeff;
}

void WaveOsc::setVol( double x )
{
   if ( x >= 1.0 )
      x = 1.0;
   else if ( x <= 0.0 )
      x = 0.0;
   vol = x;
   instVol.val = x * tremolo.val * 128.0;
}

void WaveOsc::setWave( byte i ) 
{
   desWavTab &d = wavebank[i];

   name    = wavebank.name(i);
   table   = d.table;
   period  = d.period;
   length  = d.length;
   idx.val = 0;
   coeff   = pow(2, 16) * (period / audioRate);
   step    = freq * coeff;
}

                        /*--------------------
                                Tremolo
                         --------------------*/

void Tremolo::evaluate()
{
   val = 1.0 - pos;
}

char* Tremolo::prompt()
{
   return CONSTR("tremolo");
}

void Tremolo::charEv( char code )
{
   if ( code == '!' )         // reset
      iniOsc( .4, 2.5 );  

   TrigLFO::charEv( code );
}

                        /*--------------------
                                Vibrato
                         --------------------*/

#define RATIO_SEMITONE   1.059463 // frequency ratio between adjacent pitches
#define INVERT_SEMITONE   .943874 // 1 / RATIO_SEMITONE

void Vibrato::evaluate()
{
   double spos = fader * ((2.0 * pos) - depth);
   if ( spos >= 0 )
      val = 1.0 + (spos * (RATIO_SEMITONE-1.0) );
   else
      val = 1.0 + (spos * (1.0 - INVERT_SEMITONE));
}

void Vibrato::iniPos()
{
   pos = depth * 0.5;
}

char* Vibrato::prompt()
{
   return CONSTR("vibrato");
}
