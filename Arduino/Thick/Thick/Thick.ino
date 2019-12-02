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

about_program( Thick, 0.72 )                 // specify sketch name & version

#ifndef INTERN_CONSOLE                       // required for config macro to work
   #error This sketch requires __STNDLONE__ runtime model or higher (Model.h)
#endif

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

   boolean  cleanSlate;              // true if user has yet to play a note

   public:

   void config() 
   { 
      QuadSynth::config();

      keybrd.setTopOct( 3 );         // constrain keyboard to octaves 0-3
      keybrd.setDefOct( 2 );         // start keyboard in octave 2

      // add autowah effect to each voice

      for ( byte i = 0; i < numVox; i++ )
         vox[i]->addEffect( new AutoWah() ); 
   }

   void calcGlides()                // calc voice portamento speeds given
   {                                // topGlide and portRatio
      Word portReg;

      // remap topGlide so fastest speed is closest to 0

      portReg.val = 256 - topGlide;          
      byte lastGlide = portReg._.lsb;        

      vox[numVox-1]->setGlide( lastGlide );

      if ( lastGlide == 0 )
      {
         for ( byte i = numVox-1; i >= 1; i-- )
            vox[i-1]->setGlide( 0 );
         return;
      }

      for ( byte i = numVox-1; i >= 1; i-- )
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

         case '!':                           // perform a reset

            QuadSynth::charEv( code );       // execute reset of super class

            setMute( true );                 // mute synth until initial note 
            setTopGlide( 0 );                // no portamento for top voice
            setGlideRatio( 0 );              // no difference in portamento speeds
            setRelease( 80 );                // set envelope release
            setOscTuning( SPREAD );          // use a tuning of octaves and 5ths

            // the following line executes a macro which sets up each voice's 
            // detuning/autowah/vibrato so the voices sound groovy together
      
            console.exe( PSTR( "0Ea<c180\\lf.5\\d80\\````"
                               "1Vf1\\d10\\<`Ea<c200\\lf2.75\\d77\\````"
                               "2Vf.25\\d13\\<`Ea<c150\\lf.75\\d64\\````"
                               "3Ea<c190\\lf.23\\d96\\````"
                              ) 
                        ); 

            // the following code is for starting with a nice multi-octave 
            // glissando: the voices are started on a high note, with the synth 
            // muted, then in noteOn() when the 1st note is played, glide 
            // parameters will be set and the synth unmuted, yielding a sweeping 
            // glissando to the 1st note played by the user.

            cleanSlate = true;               // user has yet to play a note
            lastNote.set( 7, 5 );            // start from up high

            for ( byte i = 0; i < numVox; i++ )
               vox[i]->noteOn( lastNote );

            break;

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

   Osc *newOsc( byte nth )
   {
      return new SawTooth();
   }
   
   void noteOn( key k )
   {
      if ( cleanSlate )                      // if this is 1st note played
      {
         setMute( false );                   // unmute synth
         setTopGlide( 128 );                 // and set glide parameters
         setGlideRatio( 73 );                // for an initial sweeping
         cleanSlate = false;                 // glissando
      }
      QuadSynth::noteOn( k );
   }

   void setGlideRatio( byte r )
   {
      glideRatio = r;
      portRatio.val = 256 - ( r >> 1 );
      calcGlides();
   }

   void setRelease( byte release )
   {
      for ( byte i = 0; i < numVox; i++ )
         ((ADSRVoice *)vox[i])->envAmp.setRelease( release );
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

            for ( byte i = 0; i < numVox; i++ )
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

            for ( byte i = 0; i < numVox; i++ )
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

} thick;                                     


void setup()
{
   ardutouch_setup( &thick );                // initialize ardutouch resources
}

void loop()
{
   ardutouch_loop();                         // perform ongoing ardutouch tasks   
}                                             



