//
//  Xoid.ino
//
//  An XORing synth. 
//
//  Target:   ArduTouch board
// 
//                      ==== Diagram of the Xoid Voice ====
//
//     ----------------------                 
//     | Oscillator Section | --> envelope --> AutoWah --> Gain --> [output]
//     ----------------------                                           
//              ^
//              | 
//              |
//           Vibrato
//
//
//                   ==== Diagram of the Oscillator Section ====
//
//                            pair0
//                           -------
//              Osc 1 ---->  |     |           -------
//                           | XOR |   ---->   |     |
//              Osc 2 ---->  |     |           |     |
//                           -------           |     |
//                                             | XOR |  --->  [output]
//                           -------           |     |      
//              Osc 3 ---->  |     |           |     |
//                           | XOR |   ---->   |     |
//              Osc 4 ---->  |     |           -------
//                           -------            combo
//                            pair1
//
//  How to use:
//
//  Load preset sounds by double-tapping the right button and then pressing
//  any "white" key.
//
//  Each LED can be in one of 3 states : OFF - ON - BLINKING  
//
//  The LEDs indicate what parameters the pots control:
//
//          red LED    blue LED       top POT            bot POT
//          -------    --------    -------------   --------------------
//             OFF         OFF     combo XOR mix     combo freq ratio
//              ON         OFF     pair0 XOR mix     pair0 freq ratio
//           BLINK         OFF     pair1 XOR mix     pair1 freq ratio
//             OFF          ON       env ATTACK         env DECAY
//              ON          ON       env SUSTAIN       env RELEASE
//           BLINK          ON      vibrato FREQ      vibrato DEPTH
//             OFF       BLINK      autowah FREQ      autowah DEPTH
//              ON       BLINK     autowah ON/OFF     autowah CUTOFF 
//           BLINK       BLINK          gain            portamento
//
//  To bump the red  LED state: press left button
//  To bump the blue LED state: press right button
//
//  When the blue LED is off the red LED also indicates what happens when you
//  tap-press a button (a "tap-press" is a double tap with the 2nd tap being
//  of longer duration) :
//                                       .....   tap-press:  .....
//
//          red LED    blue LED       left BUTTON            right BUTTON  
//          -------    --------    ----------------   -------------------------
//             OFF         OFF     combo XOR on/off    bump combo freq spectrum
//              ON         OFF     pair0 XOR on/off    bump pair0 freq spectrum
//           BLINK         OFF     pair1 XOR on/off    bump pair1 freq spectrum
//
//  Note 1: toggling an XOR function on and off dramatically effects the sound. 
//  Note 2: Bumping the freq spectrum changes the frequency sweep for bottom pot.
//
//  For more info on XOR objects and their U/I see example sketches numbered in
//  the 40s.
//
//  To raise keyboard by an octave, tap right button once.
//  To lower keyboard by an octave, tap left button once.
//
//  ---------------------------------------------------------------------------
// 
//  Copyright (C) 2018, Cornfield Electronics, Inc.
// 
//  This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
//  Unported License.
// 
//  To view a copy of this license, visit
//  http://creativecommons.org/licenses/by-sa/3.0/
// 
//  Created by Bill Alessi & Mitch Altman.
// 
//  ---------------------------------------------------------------------------/*

#include "ArduTouch.h"                       

#ifndef __STNDLONE__

   #error This sketch requires the __STNDLONE__ runtime model (Model.h)

   // otherwise there are no LEDs. 
   // Comment #error statement out if you want to run the Console.

#endif

about_program( Xoid, 0.96 )

/*----------------------------------------------------------------------------*
 *                                 presets
 *----------------------------------------------------------------------------*/      

define_preset( Empty, "" )               
define_preset( Reset, "'" )               

define_preset( Bass,  "'r<m127\\r.749\\0m47\\r<1.25\\`1m44\\r0.5\\```" )
define_preset( Gonk,  "'k6`r<m8\\r.5\\D-.7\\0m8\\r6\\`1m50\\r0.629\\```" )               

define_preset( XFuzz2, "'x26\\"
                       "rr.125\\m81\\"
                       "0r.667\\m67\\D1.4\\`"
                       "1r.441\\m127\\```"
                       "0ea5\\d200\\s170\\r32\\`"
                       "Vf5.5\\d1\\t100\\`"
                       "Eg<g2.5\\``"
                       "Ea<l<d.36\\f.7\\````" 
                       )

define_preset( Zonk,   "'x24\\k6`"
                       "rr.111\\m114\\D.5\\"
                       "0<r3.5\\m36\\`"
                       "1<r.567\\m81\\D3\\```"
                       "0ea2\\d145\\s0\\r100\\`"
                       "Vf.24\\d.5\\t30\\`"
                       "Eg<g2.5\\``"
                       "Ea<l<d.6\\f.2\\````" 
                       )

define_preset( Besot,  "'"
                       "rr6.6\\m38\\"
                       "0r.667\\m76\\`"
                       "1<r12\\m53\\```"
                       "0ea0\\d200\\s179\\r200\\`"
                       "V<-f5\\d.7\\t5\\`"
                       "Eg<g1\\``"
                       "Ea<c120\\l<d1\\f3.858\\````" 
                       )

define_preset( Morbo,  "'"
                       "r<r1\\m68\\D.25\\"
                       "0<r.707\\m76\\D-.35\\`"
                       "1r.5\\m5\\```"
                       "0ea59\\d50\\s0\\r32\\`"
                       "V<f3.2\\d.3\\t40\\`"
                       "Eg<g1.5\\``"
                       "Ea<c40\\l.d.5\\f.5\\````" 
                       )

define_preset( Static, "'"
                       "r<r23\\m79\\"
                       "0<r2.244\\m44\\`"
                       "1<r4\\m91\\```"
                       "0ea79\\d100\\s120\\r190\\`"
                       "V.-f2.5\\d1\\t30\\`"
                       "Eg<g2.25\\``"
                       "Ea<c60\\l<d.305\\f1.73\\````" 
                       )

begin_bank( myPresets )                   // these presets will be loaded

   _preset( Gonk )                        // C
   _preset( Empty )                       // C#
   _preset( Bass )                        // D
   _preset( Empty )                       // D#
   _preset( XFuzz2 )                      // E
   _preset( Zonk )                        // F
   _preset( Empty )                       // F#
   _preset( Besot )                       // G
   _preset( Empty )                       // G#
   _preset( Morbo )                       // A
   _preset( Empty )                       // A#
   _preset( Static )                      // B

end_bank()

// ==================================================================================

// The following classes make Console navigation more decipherable

// ==================================================================================

class RootPair : public XorOsc           // root is same as "combo"  
{
   typedef XorOsc super;                 // superclass is XorOsc

   public:

   RootPair( Osc* o0, Osc* o1 ) : super( o0, o1 ) {};

   #ifdef CONSOLE_OUTPUT
   const char *prompt() { return CONSTR("root"); }
   #endif

} ;

class Pair0 : public XorOsc             
{
   typedef XorOsc super;                 // superclass is XorOsc

   public:

   Pair0( Osc* o0, Osc* o1 ) : super( o0, o1 ) {};

   #ifdef CONSOLE_OUTPUT
   const char *prompt() { return CONSTR("pair0"); }
   #endif

} ;

class Pair1 : public XorOsc             
{
   typedef XorOsc super;                 // superclass is XorOsc

   public:

   Pair1( Osc* o0, Osc* o1 ) : super( o0, o1 ) {};

   #ifdef CONSOLE_OUTPUT
   const char *prompt() { return CONSTR("pair1"); }
   #endif

} ;

// ==================================================================================

class XGain : public Gain
{
   typedef Gain super;              // superclass is Gain

   public:

   bool charEv( char code )
   {
      switch ( code )
      {
         case '!':                  // reset

            super::charEv( code );
            setMaxGain( 2.5 );
            autoClip = false;       // retain old-school "nasty" overdrive
            break;

         default:

            return super::charEv( code );
      }
      return true;
   }

} ;

// ==================================================================================

class XVoice : public StockVoice    // stock voice with added effects
{
   typedef StockVoice super;        // superclass is StockVoice

   public:

   AutoWah wah;                     // built-in low pass filter with LFO
   XGain   gain;                    // built-in gain control

   XVoice()
   {
      addEffect( &this->wah );
      addEffect( &this->gain );
   }

} ;

// ==================================================================================

class XoidSynth : public OneVoxSynth             
{
   typedef  OneVoxSynth super;      // superclass is OneVoxSynth

   public:

   ByteMenu presetMenu;             // keybrd menu for selecting presets

   XVoice  *voice;                  // ptr to synth's lone voice

   XorOsc *pair0;                   // a (potentially XORed) pair of oscillators
   XorOsc *pair1;                   // a (potentially XORed) pair of oscillators
   XorOsc *root;                    // pair0 + pair1 (potentially XORed)

   #define NUMLEDS 2                // number of onboard LEDs

   enum {  // these values enumerate elements of ledState[] 

           LED_OFF   = 0,       
           LED_ON    = 1,      
           LED_BLINK = 2

        } ;

   byte ledState[ NUMLEDS ];        // display state of each onboard LED

   enum {  // these values enumerate frame 

           FRAME00   = 0,       
           FRAME10   = 1,      
           FRAME20   = 2,
           FRAME01   = 4,
           FRAME11   = 5,       
           FRAME21   = 6,      
           FRAME02   = 8,      
           FRAME12   = 9,      
           FRAME22   = 10

        } ;

   byte frame;                      // user interface frame 

   Control *sendControl;            // send pot/button events to this control

   void config() 
   { 
      super::config();
      presets.load( myPresets );    // load bank of presets
   }

   Osc *newOsc( byte nth )                
   {
      switch ( nth )
      {
         case 0:
         {
            pair0 = new Pair0( new Sine(), new Sine() );
            pair1 = new Pair1( new Sine(), new Sine() );
            root  = new RootPair( pair0, pair1 );
            return root;
         }
         /*
         case 1:
         {
            FastWaveOsc *o = new FastWaveOsc();
            o->setTable( wavetable_named( Ether ) );
            return o;
         }
        */
      }
   }

   Voice *newVox( byte nth )                
   {
      voice = new XVoice();
      return voice;
   }

   bool charEv( char code )
   {
      switch ( code )
      {
         #ifdef INTERN_CONSOLE
         case 'r':                           // push root pair
            console.pushMode( voice );
            console.pushMode( voice->osc );
            break;
         #endif

         case '!':                           // reset

            super::charEv( code );

            for ( byte i = 0; i <= NUMLEDS; i++ )
            {
               ledState[i] = LED_OFF;
               dispLED( i );
            }
            setFrame();

            execute( PSTR( "0er32\\" ) );
            execute( PSTR( "0V<d.09\\t5\\f4.5\\" ) );
            execute( PSTR( "0Om24\\r1.500\\" ) );
            execute( PSTR( "0O0m32\\r4.0\\`" ) );
            execute( PSTR( "0O1m32\\r1.76\\`" ) );
            execute( PSTR( "0Ea<ld0\\" ) );
            break;

         #ifdef CONSOLE_OUTPUT

         // The following stuff is meant for developers so you can print 
         // out the current patch to the Console

         case '/':
         {
            objectInfo( root );
            objectInfo( pair0 );
            objectInfo( pair1 );
            objectInfo( &voice->envAmp );
            objectInfo( &voice->vibrato );
            objectInfo( &voice->wah );
            objectInfo( &voice->wah.lfo );
            objectInfo( &voice->gain );
            break;
         }

         case chrInfo:
         {
            super::charEv( code );
            char *frameStr = "{frame:   }";
            frameStr[8] = '0' + ledState[0];
            frameStr[9] = '0' + ledState[1];
            console.print( frameStr );
            break;
         }
         #endif

         default:
            return super::charEv( code );
      }
      return true;
   }

   void bumpLED( byte nth )
   {
      switch ( ledState[nth] )
      {
         case LED_OFF:  ledState[nth] = LED_ON;    break;
         case LED_ON:   ledState[nth] = LED_BLINK; break;
         default:       ledState[nth] = LED_OFF;   break;
      }
      dispLED( nth );
      setFrame();
   }

   void dispLED( byte nth )
   {
      switch ( ledState[nth] )
      {
         case LED_OFF:    offLED( nth );   break;
         case LED_ON:     onLED( nth );    break;
         case LED_BLINK:  blinkLED( nth ); break;
      }
   }

   bool evHandler( obEvent ev )              // handle an onboard event
   {

      if ( handlePots(ev) ) return true;  

      switch ( ev.type() )
      {
         case BUT0_PRESS:                    // 

            bumpLED(0);
            break;

         case BUT1_PRESS:                    //

            bumpLED(1);
            break;

         case BUT0_DTAP:                     // intercept "pop-mode"

            break;

         case BUT1_DTAP:                     // override "one-shot menu" 

            presetMenu.waitKey();
            runPreset( (const char *)presets.dataPtr( presetMenu.value ) );
            break;

         case BUT0_TPRESS:                    
         case BUT1_TPRESS:                   
         {
            switch ( frame )
            {
               case FRAME00:  
               case FRAME10:  
               case FRAME20:  
                  sendControl->evHandler(ev);  
            }
            break;
         } 

         default:

            return super::evHandler( ev );
      }
      return true;
   }

   bool handlePots( obEvent ev )             // handle pot events
   {
      bool pot0 = false;

      if ( ev.type() == POT0 )
         pot0 = true;
      else if ( ev.type() != POT1 )
         return false;                       // event is neither POT0 or POT1

      byte potVal = ev.getPotVal();
            
      switch ( frame )
      {
         case FRAME00:  
         case FRAME10:  
         case FRAME20:  

            sendControl->evHandler(ev);
            break;

         case FRAME01:
         {
            if ( pot0 )
               voice->envAmp.setAttack( potVal );
            else
               voice->envAmp.setDecay( potVal );
            break;
         }

         case FRAME11:
         {
            if ( pot0 )
               voice->envAmp.setSustain( potVal );
            else
               voice->envAmp.setRelease( potVal );
            break;
         }

         case FRAME21:

            voice->vibrato.evHandler(ev);
            break; 

         case FRAME02:

            voice->wah.lfo.evHandler(ev);
            break; 

         case FRAME12:                     // autowah pot0 On/Off; pot1 cutoff
         {
            if ( pot0 )
               voice->wah.setMute( potVal < 127 ? true : false );
            else
               voice->wah.setCutoff( potVal );
            break;
         }

         case FRAME22:

            if ( pot0 )
               voice->gain.evHandler(ev);
            else
               voice->setGlide( potVal );
            break; 

      }
      return true;
   }

   void setFrame()                           // set the U/I frame
   {
      // compute frame from ledState[] entries

      frame = 0;
      byte factor = 1;
      for ( byte i = 0; i <= NUMLEDS; i++ )
      {
         frame += ledState[i] * factor;
         factor <<= 2;
      }

      // set sendControl

      switch ( frame )
      {
         case FRAME00:  sendControl = root;   break;
         case FRAME10:  sendControl = pair0;  break;
         case FRAME20:  sendControl = pair1;  break;
      }
   }

   void objectInfo( Mode *m )
   {
      #ifdef CONSOLE_OUTPUT
         console.romprint( m->prompt() );
         m->charEv('?');
      #endif
   }

   void welcome()
   {
      execute( PSTR( "p0" ) );               // start with 'C' preset loaded
   }

} mySynth;                                     


void setup()
{
   ardutouch_setup( &mySynth );              // initialize ardutouch resources
}                                           

void loop()
{
   ardutouch_loop();                         // perform ongoing ardutouch tasks      
}                                             

