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

about_program( Thick, 0.38 )                 // specify sketch name & version
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

/******************************************************************************
 *
 *                              ThickVoice 
 *
 ******************************************************************************/

class ThickVoice : public Voice              // voice with sawtooth osc, autowah,
{                                            // and note transposition capability

   AutoWah wah;                              // resident autowah effect

   public:

   SawTooth osc;                             // resident oscillator

   char  xpose;                              // amount to transpose notes by

   void setup()
   {
      useOsc( &osc );                        // assign oscillator to voice
      addEffect( &wah );                     // append autowah to effects loop
      reset();                               // and reset the voice
   }

   boolean charEv( char code )               // handle a character event
   {
      switch( code )
      {
         #ifdef INTERN_CONSOLE               // compile cases needed by macros

         case 'x':                           // set transposition amount

            console.getSByte( CONSTR("xpose"), &this->xpose );
            return true;

         #endif

         #ifdef CONSOLE_OUTPUT               // compile cases that display to console 

         case chrInfo:                       // display voice info to console

            Voice::charEv( chrInfo );
            console.newline();
            console.rtab();
            console.infoInt( CONSTR("xpose"), xpose );
            return true;

         #endif

         default:

            return Voice::charEv( code );
      }
   }

   void noteOn( key k )                   // play of a note (with transposition)
   {
      #define MAX_KEYNUM 12*15            // 12 notes per octave, 15 octaves max

      key  xpKey;                         // transposed key struct
      byte pos, oct;                      // transposed key position, octave
      int  keyNum;                        // transposed key number

      // translate untransposed key octave/position into keyNum

      keyNum  = k.position() + k.octave() * 12;

      // transpose keyNum

      keyNum += xpose;

      // range-check transposed keyNum

      while ( keyNum < 0 )
         keyNum += 12;
         
      while ( keyNum > MAX_KEYNUM )
         keyNum -= 12;

      // translate transposed keyNum back into octave/position
      
      oct = keyNum / 12;
      pos = keyNum % 12;
      xpKey.set( pos, oct );

      Voice::noteOn( xpKey );             // play the transposed note

      #undef  MAX_KEYNUM
   }

} ;

/******************************************************************************
 *
 *                              ThickSynth 
 *
 ******************************************************************************/

#define NUMVOCS 4

class ThickSynth : public StereoSynth             
{
   ThickVoice voc[ NUMVOCS ];        // component sawtooth voices 

   char xBuf[ BUFSZ ];               // buffer for combining voice outputs

   key   lastKey;                    // last key played
   byte  deferVoc;                   // if nonzero, call noteOn() for voc[deferVoc-1]
                                     // at next dynamic update

   Word  portRatio;                  // ratio between adjacent voice portamento
                                     // speeds (Fixed Pt 0100h = 1.0)
   byte  topGlide;                   // top portamento speed (applied to voc[3])

   public:

   void setup() 
   { 
      portRatio.val = 256;           // portRatio = 1.0 (no diff in portamentos)

      for ( byte i = 0; i < NUMVOCS; i++ )  // init each voice
         voc[i].setup();

      // the following line executes a macro which sets up each voice's 
      // detuning/autowah/vibrato so the voices sound groovy together

      console.exe( CONSTR( "0d-6\\ea<c180\\lf.5\\d.625\\````"
                           "1d38\\Vf1\\d.075\\<`ea<c200\\lf2.75\\d.6\\````"
                           "2d-12\\Vf.25\\d.1\\<`ea<c150\\lf.75\\d.5\\````"
                           "3d48\\ea<c190\\lf.23\\d.75\\````"
                          ) 
                  ); 

      lastKey.set( 0, 2 );           // initial last note of "C" in octave 2
      masterTuning->defOctave = 2;   // start keyboard in octave 2
      masterTuning->maxOctave = 3;   // constrain keybrd to octaves 0..3
                                     // because it sounds better down there
   }

   void calcGlides()                // calc voice portamento speeds given
   {                                // topGlide and portRatio
      Word portReg;

      byte lastGlide = topGlide;
      voc[NUMVOCS-1].setGlide( lastGlide );

      for ( byte i = NUMVOCS-1; i >= 1; i-- )
      {
         portReg.val  = portRatio.val;       // scale portamento per voice
         portReg.val *= lastGlide;
         lastGlide     = portReg._.msb;
         voc[i-1].setGlide( lastGlide );
      }
   }

   boolean charEv( char code )               // handle a character event
   {
      switch( code )
      {
         #ifdef INTERN_CONSOLE               // compile cases needed by macros

         case '0':                           // 0 thru 3 push respective voices
         case '1':
         case '2':
         case '3':
            console.pushMode( &voc[code-'0'] );
            break;

         #endif

         #ifdef CONSOLE_OUTPUT               // compile cases that display to console 

         case chrInfo:

            StereoSynth::charEv( chrInfo );
            console.infoByte( CONSTR("top"), topGlide );
            break;

         #endif

         default:
            return StereoSynth::charEv( code );
      }
      return true;
   }

   boolean evHandler( obEvent e )            // event handler
   {
      switch ( e.type() )                    // branch on event type
      {
         case KEY_DOWN:                      // a key has been pressed

            deferVoc = NUMVOCS;              // set for defered noteOns
            lastKey = e.getKey();
            return true;                     

         case KEY_UP:                        // key has been released

            for ( byte i = 0; i < NUMVOCS; i++ )
               voc[i].noteOff();                  
            return true;                     

         case POT0:                          // set portamento spread
         
            portRatio.val = 256 - ( e.getPotVal() >> 1 );
            calcGlides();
            return true;
         
         case POT1:                          // set portamento speed
         {
            Word glideVal;
            glideVal.val = 256 - e.getPotVal();
            setTopGlide( glideVal._.lsb );
            return true;
         } 
         case BUT0_PRESS:
         {
            // unison tuning

            char unison_xpose[]  = {   0,  0,   0,  0 };
            char unison_detune[] = {  -6, 38, -12, 48 };

            for ( byte i = 0; i < NUMVOCS; i++ )
            {
               voc[i].xpose = unison_xpose[i];
               voc[i].osc.setDetune( unison_detune[i] );
            }

            reTrigger();
            return true;
         }
         case BUT1_PRESS:
         {
            // spread tuning

            char spread_xpose[]  = { -12, 0, 0, 7 };
            char spread_detune[] = {   1, 3, -2, -5 };

            for ( byte i = 0; i < NUMVOCS; i++ )
            {
               voc[i].xpose = spread_xpose[i];
               voc[i].osc.setDetune( spread_detune[i] );
            }

            reTrigger();
            return true;
         }
         default:                            // pass on unhandled events
                                             
            return StereoSynth::evHandler(e);
      }
   }

   void output( char *bufL, char *bufR )     // output 1 buffer of audio
   {  
      int sum;

      // combine voc[0] and voc[1] output in bufL

      voc[0].output( bufL ); 
      voc[1].output( &this->xBuf[0] ); 

      for ( byte i = 0 ; i < BUFSZ; i++ )
      {
         sum = bufL[ i ] + xBuf[ i ];
         bufL[i] = sum >> 1;
      }

      // combine voc[2] and voc[3] output in bufR

      voc[2].output( bufR ); 
      voc[3].output( &this->xBuf[0] ); 

      for ( byte i = 0 ; i < BUFSZ; i++ )
      {
         sum = bufR[ i ] + xBuf[ i ];
         bufR[i] = sum >> 1;
      }
   }

   void dynamics()                           // perform a dynamic update
   {  
      if ( deferVoc )                        // execute noteOn for next defered voice
      {
         --deferVoc;
         voc[ deferVoc ].noteOn( lastKey );
      }

      for ( byte i = 0; i < NUMVOCS; i++ )   // update dynamics for all voices
         voc[i].dynamics();    
   }

   void reTrigger()                          // retrigger all voices
   {
      for ( byte i = 0; i < NUMVOCS; i++ )   // call noteOff for all voices
         voc[i].noteOff();
      deferVoc = NUMVOCS;                    // set deferVoc so that noteOns() will
   }                                         // be called in subsequent dynamics() 

   void setTopGlide( byte g )                // set the top portamento speed
   {
      topGlide = g;
      calcGlides();                          // recalculate all voice portamentos
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

