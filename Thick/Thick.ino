//
//  Thick.ino
// 
//  A 4-voice unison synth employing pre-figured saw-tooth oscillators.
//
//  How To Use:
//
//    1) Press the keys to play notes
//    2) Tap left button to lower keyboard by 1 octave
//    3) Tap right button to raise keyboard by 1 octave
//    4) Press the left button to use a unison tuning
//    5) Press the right button to use a spread tuning with 5ths and octaves.
//    6) Bottom pot controls portamento speed (clockwise for lower speed,
//       all the way counter-clockwise is portamento off)
//    7) Top pot graduates the portamento speed of each voice so that they
//       run at different rates (clockwise for greater divergence, all the 
//       way counter-clockwise for no difference in speeds)
//
//  Target:   ArduTouch board
// 
//  ---------------------------------------------------------------------------
// 
//  Copyright (C) 2016, Cornfield Electronics, Inc.
// 
//  This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
//  Unported License.
// 
//  To view a copy of this license, visit
//  http://creativecommons.org/licenses/by-sa/3.0/
// 
//  Created by Bill Alessi & Mitch Altman.
// 
//  ---------------------------------------------------------------------------

#include "ArduTouch.h"                       // use the ArduTouch library 

about_program( Thick, 0.56 )                 // specify sketch name & version
set_baud_rate( 115200 )                      // specify serial baud-rate

#ifndef INTERN_CONSOLE                       // required for setup macro to work
   #error This sketch requires __STNDLONE__ runtime model or higher (Model.h)
#endif

/******************************************************************************
 *
 *                               SawTooth 
 *
 ******************************************************************************/

class SawTooth : public Osc                  // a sawtooth oscillator
{
   DWord outVal;                             // current output value, kept as a
                                             // fixed pt, 32-bit number. High byte
                                             // is integral part, lower 3 bytes 
                                             // are the fractional part.

   unsigned long delta;                      // bump outVal this much per tick
   double        coeff;                      // delta = frequency * coeff

   public:

   SawTooth();                               // initialize state vars
   void onFreq();                            // compute freq dependent state vars
   void output( char *buf );                 // write one buffer of output

} ;

/*----------------------------------------------------------------------------*
 *
 *  Name:  SawTooth::SawTooth
 *
 *  Desc:  SawTooth constructor.
 *
 *  Glob:  audioRate        - audio playback rate
 *
 *  Memb: +coeff            - this multiplied by osc frequency gives delta
 *
 *----------------------------------------------------------------------------*/      

SawTooth::SawTooth()
{
   coeff = pow(2, 24) * (256 / audioRate);
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  SawTooth::onFreq
 *
 *  Desc:  Compute frequency-dependent state vars.
 *
 *  Memb:  coeff            - multiply this by osc frequency to get delta
 *         effFreq          - effective frequency (includes internal detuning)
 *         extFactor        - external detuning factor
 *        +delta            - amount to increment outVal per tick
 *
 *  Note:  onFreq() is a virtual method of Osc, and is automatically called
 *         whenever there is a change in frequency for the oscillator. This change
 *         in frequency can come from one of 3 sources: the oscillator is locally
 *         detuned, the "ideal" frequency of the oscillator is set, or an external
 *         (transient) detuning is applied (such as from an ongoing vibrato).
 *
 *         There are 2 implicit inputs to onFreq(), Osc::effFreq and 
 *         Osc::extFactor. Multiplying these together gives the current 
 *         frequency of the oscillator.
 * 
 *----------------------------------------------------------------------------*/      

void SawTooth::onFreq()
{
   delta = effFreq * extFactor * coeff;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  SawTooth::output
 *
 *  Desc:  Write one buffer of output.
 *
 *  Glob:  audioBufSz       - size of system audio buffers
 *
 *  Memb:  outVal           - current output value
 *        +delta            - amount to increment outVal per tick
 *
 *----------------------------------------------------------------------------*/      

void SawTooth::output( char *buf )
{
   byte icnt = audioBufSz;                // write this many ticks of output

   while ( icnt-- )
   {
      outVal.val += delta;                // update outVal       
      byte rounded = outVal._.msw._.msb;  // move high byte to rounded
      if ( outVal._.msw._.lsb > 127 )     // and round 
         ++rounded;
      *buf++ = rounded;                   // write rounded to buffer and bump
   }
}

/******************************************************************************
 *
 *                              ThickSynth 
 *
 ******************************************************************************/

class ThickSynth : public QuadSynth             
{
   enum  OscTuning { UNISON = 0,     // all oscillators at tonic 
                     SPREAD = 1      // spread tuning with 5ths and octaves
                   };

   OscTuning oscTuning;              // current tuning for oscillators
   
   byte  glideRatio;                 // ratio between adjacent voice portamentos
                                     // 0 = 1.0, 255 = close to 0.0
   Word  portRatio;                  // ratio between adjacent voice portamentos
                                     // speeds (Fixed Pt 0100h = 1.0)

   byte  topGlide;                   // top portamento speed (applied to vox[3])

   public:

   void setup() 
   { 
      QuadSynth::setup();

      portRatio.val = 256;           // portRatio = 1.0 (no diff in portamentos)

      // initialize voices

      for ( byte i = 0; i < NumVox; i++ )
      {
         vox[i]->useOsc( new SawTooth() );
         vox[i]->addEffect( new AutoWah() ); 
      }

      reset();

      // the following line executes a macro which sets up each voice's 
      // detuning/autowah/vibrato so the voices sound groovy together

      console.exe( PSTR( "0ea<c180\\lf.5\\d.625\\````"
                         "1Vf1\\d.075\\<`ea<c200\\lf2.75\\d.6\\````"
                         "2Vf.25\\d.1\\<`ea<c150\\lf.75\\d.5\\````"
                         "3ea<c190\\lf.23\\d.75\\````"
                         "d128\\r73\\"
                        ) 
                  ); 

      setOscTuning( SPREAD );

      // the following code is for starting with an impressive glissando :)

      lastNote.set( 7, 5 );           // start from up high

      for ( byte i = 0; i < NumVox; i++ )
      {
         vox[i]->setMute( true );
         vox[i]->envAmp.setSustain( 0 );
         vox[i]->noteOn( lastNote );
         vox[i]->noteOff();
         vox[i]->envAmp.setSustain( 255 );
         vox[i]->setMute( false );
      }

      lastNote.set( 0, 2 );          // initial last note of "C" in octave 2
      masterTuning->defOctave = 2;   // start keyboard in octave 2
      masterTuning->maxOctave = 3;   // constrain keybrd to octaves 0..3
                                     // because it sounds better down there
   }

   void calcGlides()                // calc voice portamento speeds given
   {                                // topGlide and portRatio
      Word portReg;

      // remap topGlide so fastest speed is closest to 0

      portReg.val = 256 - topGlide;          
      byte lastGlide = portReg._.lsb;        

      vox[NumVox-1]->setGlide( lastGlide );

      if ( lastGlide == 0 )
      {
         for ( byte i = NumVox-1; i >= 1; i-- )
            vox[i-1]->setGlide( 0 );
         return;
      }

      for ( byte i = NumVox-1; i >= 1; i-- )
      {
         portReg.val  = portRatio.val;       // scale portamento per voice
         portReg.val *= lastGlide;
         lastGlide     = portReg._.msb;
         vox[i-1]->setGlide( lastGlide ? lastGlide : 1 );
      }
   }

   boolean charEv( char code )               // handle a character event
   {
      switch( code )
      {
         #ifdef INTERN_CONSOLE               // compile cases needed by macros

         case 'd':
         {         
           byte duration;
           if ( console.getByte( CONSTR("duration"), &duration ) )
               setTopGlide( duration );
           break;
         }   

         case 'r':
         case 'R':
         {         
           byte ratio;
           if ( console.getByte( CONSTR("ratio"), &ratio ) )
               setGlideRatio( ratio );
           break;
         }   

         case 's':
         case 'S':

            setOscTuning( SPREAD );
            reTrigger();
            break;

         case 'u':
         case 'U':

            setOscTuning( UNISON );
            reTrigger();
            break;

         #endif

         #ifdef CONSOLE_OUTPUT               // compile cases that display to console 

         case chrInfo:

            QuadSynth::charEv( chrInfo );
            switch( oscTuning )
            {
               case UNISON: 

                  console.infoStr( CONSTR("oscTuning"), CONSTR("UNISON") ); 
                  break;

               case SPREAD: 
               
                  console.infoStr( CONSTR("oscTuning"), CONSTR("SPREAD") ); 
                  break;
            }              
            console.newline();
            console.rtab();
            console.infoByte( CONSTR("duration"), topGlide );
            console.infoByte( CONSTR("ratio"), glideRatio );
            break;

         #endif

         default:
            return QuadSynth::charEv( code );
      }
      return true;
   }

   boolean evHandler( obEvent e )            // event handler
   {
      switch ( e.type() )                    // branch on event type
      {
         case POT0:                          // set portamento ratio
         
            setGlideRatio( e.getPotVal() );
            break;
         
         case POT1:                          // set top portamento speed
         
            setTopGlide( e.getPotVal() );
            break;
          
         case BUT0_PRESS:                    // set unison oscTuning

            setOscTuning( UNISON );
            reTrigger();
            break;
         
         case BUT1_PRESS:                    // set spread oscTuning

            setOscTuning( SPREAD );
            reTrigger();
            break;

         default:                            // pass on unhandled events
                                             
            return QuadSynth::evHandler(e);
      }
      return true;                     
   }

   void setGlideRatio( byte r )
   {
      glideRatio = r;
      portRatio.val = 256 - ( r >> 1 );
      calcGlides();
   }

   void setTopGlide( byte g )                // set the top portamento speed
   {
      topGlide = g;
      calcGlides();                          // recalculate all voice portamentos
   }

   void setOscTuning( OscTuning t )          // set oscillator tuning
   {
      switch( t )
      {
         case UNISON:
         {
            char unison_xpose[]  = {   0,  0,   0,  0 };
            char unison_detune[] = {  -6, 38, -12, 48 };

            for ( byte i = 0; i < NumVox; i++ )
            {
               vox[i]->xpose = unison_xpose[i];
               vox[i]->osc->setDetune( unison_detune[i] );
            }
            break;
         }

         case SPREAD:
         {
            char spread_xpose[]  = { -12, 0, 0, 7 };
            char spread_detune[] = {   1, 3, -2, -5 };

            for ( byte i = 0; i < NumVox; i++ )
            {
               vox[i]->xpose = spread_xpose[i];
               vox[i]->osc->setDetune( spread_detune[i] );
            }
            break;
         }

         default:

            return;
      }
      oscTuning = t;
   }

} synth;                                     


void setup()
{
   ardutouch_setup( &synth );                // initialize ardutouch resources
}

void loop()
{
   ardutouch_loop();                         // perform ongoing ardutouch tasks   
}                                             



