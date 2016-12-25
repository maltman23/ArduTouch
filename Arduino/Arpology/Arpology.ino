//
//  Arpology.ino
// 
//  A 4-voice arpeggiating synth.
//
//  How To Use:
//
//    1) Press the keys to play notes
//    2) Tap left button to lower keyboard by 1 octave
//    3) Tap right button to raise keyboard by 1 octave
//    4) Press the left button to use a major mode
//    5) Press the right button to use a minor mode
//    6) Bottom pot controls speed (clockwise for  )
//    7) Top pot graduates ... (clockwise for )
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

about_program( Arpology, 0.62 )              // specify sketch name & version
set_baud_rate( 115200 )                      // specify serial baud-rate

#ifndef INTERN_CONSOLE                       // required for setup macro to work
   #error This sketch requires __STNDLONE__ runtime model or higher (Model.h)
#endif

/******************************************************************************
 *
 *                                  XVoice 
 *
 ******************************************************************************/

class XVoice : public Voice                 // voice with note transposition
{
   public:

   char   xpose;                             // amount to transpose notes by

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
 *                              QuadVoice 
 *
 ******************************************************************************/

class QuadVoice : public XVoice          // XVoice with sawtooth osc + autowah
{
   public:

   byte  vocNum;                         // voice # (for prompt purposes)

   boolean evHandler( obEvent e )        // event handler
   {
      switch ( e.type() )                // branch on event type
      {
         case KEY_DOWN:                  // a key has been pressed
         case KEY_UP:                    // key has been released

            return false;                // pass to lower mode on stack

         default:                        // pass on unhandled events
                                             
            return XVoice::evHandler(e);
      }
   }

   #ifdef CONSOLE_OUTPUT
   const char *prompt()                  // return object's prompt string
   {
      switch( vocNum )
      {
         case 0:  return CONSTR("0");
         case 1:  return CONSTR("1");
         case 2:  return CONSTR("2");
         case 3:  return CONSTR("3");
         default: return XVoice::prompt();
      }
   }
   #endif

} ;

/******************************************************************************
 *
 *                             Thirds / Voicing 
 *
 ******************************************************************************/

typedef char Thirds[2];
typedef char Voicing[4];

/******************************************************************************
 *
 *                                QuadSynth 
 *
 ******************************************************************************/

#define NUMVOCS 4

class QuadSynth : public StereoSynth             
{
   protected:

   QuadVoice *voc[ NUMVOCS ];        // component voices 

   char  xBuf[ BUFSZ ];              // buffer for combining voice outputs

   key   lastNote;                   // last note played
   byte  deferVoc;                   // if nonzero, call noteOn() for voc[deferVoc-1]
                                     // at next dynamic update
   public:

   void setup() 
   { 
      for ( byte i = 0; i < NUMVOCS; i++ ) // allocate each voice
      {
         voc[i] = new QuadVoice();
         voc[i]->vocNum = i;
      }
   }

   boolean charEv( char code )       // handle a character event
   {
      switch( code )
      {
         #ifdef INTERN_CONSOLE       // compile cases needed by macros

         case '0':                   // 0 thru 3 push respective voices
         case '1':
         case '2':
         case '3':

            console.pushMode( voc[code-'0'] );
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

            noteOn( e.getKey() );
            break;

         case KEY_UP:                        // key has been released

            for ( byte i = 0; i < NUMVOCS; i++ )
               voc[i]->noteOff();                  
            break;

         default:                            // pass on unhandled events
                                             
            return StereoSynth::evHandler(e);
      }
      return true;                     
   }

   virtual void noteOn( key newNote )        // play a note
   {
      deferVoc = NUMVOCS;                    // set for defered noteOns
      lastNote = newNote;
   }

   void output( char *bufL, char *bufR )     // output 1 buffer of audio
   {  
      int sum;

      // combine voc[0] and voc[1] output in bufL

      voc[0]->output( bufL ); 
      voc[1]->output( &this->xBuf[0] ); 

      for ( byte i = 0 ; i < BUFSZ; i++ )
      {
         sum = bufL[ i ] + xBuf[ i ];
         bufL[i] = sum >> 1;
      }

      // combine voc[2] and voc[3] output in bufR

      voc[2]->output( bufR ); 
      voc[3]->output( &this->xBuf[0] ); 

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
         voc[ deferVoc ]->noteOn( lastNote );
      }

      for ( byte i = 0; i < NUMVOCS; i++ )   // update dynamics for all voices
         voc[i]->dynamics();    
   }

   void reTrigger()                          // retrigger all voices
   {
      for ( byte i = 0; i < NUMVOCS; i++ )   // call noteOff for all voices
         voc[i]->noteOff();
      deferVoc = NUMVOCS;                    // set deferVoc so that noteOns() will
   }                                         // be called in subsequent dynamics() 

   void setAttack( byte attack )
   {
      for ( byte i = 0; i < NUMVOCS; i++ )
      {
         voc[i]->envAmp.setAttack( attack );
      }
   }

   void setDecay( byte decay )
   {
      for ( byte i = 0; i < NUMVOCS; i++ )
      {
         voc[i]->envAmp.setDecay( decay );
      }
   }

   void setPostAttack( byte postAttack )
   {
      setDecay( postAttack );
      setSustain( postAttack );
   }

   void setRelease( byte release )
   {
      for ( byte i = 0; i < NUMVOCS; i++ )
      {
         voc[i]->envAmp.setRelease( release );
      }
   }

   void setSustain( byte sustain )
   {
      for ( byte i = 0; i < NUMVOCS; i++ )
      {
         voc[i]->envAmp.setSustain( sustain );
      }
   }

   void setVoicing( Voicing v )
   {
      for ( byte i = 0; i < NUMVOCS; i++ )   
         voc[i]->xpose = v[i];
   }

} ;                                     

/******************************************************************************
 *
 *                                 Tonality 
 *
 ******************************************************************************/

class Tonality
{
   protected:

   Thirds *thirds;
   Voicing buffer;

   public:

   const char* name;                   // ptr to name of Tonality in ROM

   virtual Voicing &getVoicing( byte ith )
   {  
      Thirds  *ptrThird  = thirds;
               ptrThird += ith;
      Thirds  &ithThird  = *ptrThird;

      buffer[0] = 0;
      buffer[1] = 12 + ithThird[1];
      buffer[2] = 24 + ithThird[0];
      buffer[3] = 36;

      return this->buffer;
   }

} ;


Thirds majorThirds[12] = { { 4, 7 },
                           { 4, 7 },
                           { 3, 7 },
                           { 3, 6 },
                           { 3, 8 },
                           { 4, 7 },
                           { 3, 8 },
                           { 4, 7 },
                           { 3, 8 },
                           { 3, 7 },
                           { 3, 6 },
                           { 3, 8 }
                           } ;


class MajorTonality : public Tonality
{
   public:

   MajorTonality()
   {
      name   = CONSTR( "Major" );
      thirds = &majorThirds[0];
   }

} ;


Thirds minorThirds[12] = { { 3, 7 },
                           { 3, 8 },
                           { 3, 7 },
                           { 3, 7 },
                           { 3, 8 },
                           { 3, 7 },
                           { 3, 8 },
                           { 4, 7 },
                           { 4, 7 },
                           { 3, 7 },
                           { 3, 7 },
                           { 3, 8 }
                           } ;

class MinorTonality : public Tonality
{
   public:

   MinorTonality()
   {
      name   = CONSTR( "Minor" );
      thirds = &minorThirds[0];
   }

} ;

/******************************************************************************
 *
 *                                   Strobe 
 *
 ******************************************************************************/

class Strobe : public LFO
{
   public:

   boolean request;
   boolean handled;

   boolean charEv( char code )      // handle a character event
   {
      switch( code )
      {

         case '!':                  // reset

            LFO::charEv('!');
            request = false;
            handled = false;
            setDepth(1.0);
            break;

         default:

            return LFO::charEv( code );
      }
      return true;
   }

   void dynamics()                  // update object dynamics
   {
      if ( amMute() )
         return;

      LFO::dynamics();

      if ( pos > .5 )
      {
         if ( ! handled )
            request = true;
      }
      else
      {
         if ( handled )
         {
            request = false;
            handled = false;
         }
      }
   }

   boolean pending()
   {
      return ( request && ! handled );
   }


} ;

/******************************************************************************
 *       
 *                         Arpeggiation Patterns 
 *
 *  Arpeggiation patterns are byte strings. The 1st element specifies the 
 *  number of entries in the pattern. The remaining elements are voice
 *  ordinals (0 through 3) denoting one of the four parts in a voicing.
 *  Each ordinal must be specified ar least once in the pattern, otherwise
 *  the voice correponding to the unspecified ordinal will keep sustaining
 *  from the prior pattern that was played.
 *
 ******************************************************************************/

#define NUM_PATTERNS 9

byte pattern1[] = { 4,  0, 1, 2, 3 };
byte pattern2[] = { 4,  0, 2, 1, 3 };
byte pattern3[] = { 4,  3, 2, 1, 0 };
byte pattern4[] = { 8,  0, 1, 2, 0, 2, 1, 2, 3 };
byte pattern5[] = { 8,  0, 2, 3, 1, 2, 1, 2, 3 };
byte pattern6[] = { 6,  0, 1, 2, 1, 2, 3 };
byte pattern7[] = { 6,  0, 1, 0, 1, 2, 3 };
byte pattern8[] = { 6,  0, 2, 1, 3, 1, 2 };
byte pattern9[] = { 9,  0, 2, 3, 2, 1, 2, 3, 1, 2 };

byte *patterns[] = 
{
   NULL,                            // 1st element always NULL
   &pattern1[0],
   &pattern2[0],
   &pattern3[0],
   &pattern4[0],
   &pattern5[0],
   &pattern6[0],
   &pattern7[0],
   &pattern8[0],
   &pattern9[0]
} ;


/******************************************************************************
 *       
 *                                ArpSynth 
 *
 ******************************************************************************/

class ArpSynth : public QuadSynth             
{
   Tonality *tonality;                       // current tonality for synth
   
   Tonality *majorTonality;
   Tonality *minorTonality;

   Strobe   arpegCue;                        // cue for next arpegiating note

   byte     *pattern;                        // ptr to arpeggiation pattern
   byte     arpegIdx;                        // current index into pattern

   byte     currentPatNum;                   // pattern # being played
   byte     pendingPatNum;                    // pattern # to be loaded next

   typedef byte Frame ; enum                 // user-interface frame
   { 
      ARPEG_FRAME,      // arpeggiator frame
      ENV_FRAME         // envelope frame
   } ;

   Frame curFrame;

   public:

   void setup() 
   { 
      QuadSynth::setup();

      // initialize tonalities 

      majorTonality = new MajorTonality();
      minorTonality = new MinorTonality();

      tonality = majorTonality;

      // initialize voices

      for ( byte i = 0; i < NUMVOCS; i++ )
      {
         WaveOsc *o = new WaveOsc();
         o->setTable( wave_descriptor( Ether ) ); 
         voc[i]->useOsc( o );
      }

      for ( byte i = 0; i < NUMVOCS; i++ )   // init each voice
      {
         voc[i]->addEffect( new AutoWah() ); 
         voc[i]->reset();                    
      }

      console.exe( CONSTR( "0d1\\`1d3\\`2d-2\\`3d-5\\`" ) );

      pendingPatNum = 1;
      arpegCue.reset();
      arpegCue.setMute( false );

      setAttack( 39 );
      setPostAttack( 33 );
      setRelease( 20 );

      selectFrame( ENV_FRAME );

      lastNote.set( 0, 2 );          // initial last note of "C" in octave 2
      masterTuning->defOctave = 2;   // start keyboard in octave 2
      masterTuning->maxOctave = 3;   // constrain keybrd to octaves 0..3
                                     // because it sounds better down there
   }

   boolean charEv( char code )       // handle a character event
   {
      switch( code )
      {
         #ifdef INTERN_CONSOLE       // compile cases needed by macros

         case '+':

            setTonality( majorTonality );
            break;

         case '-':

            setTonality( minorTonality );
            break;

         case 'p':
         
            console.getByte( CONSTR("pattern"), &pendingPatNum );
            break;   
      
         #endif

         #ifdef CONSOLE_OUTPUT       // compile cases that display to console 

         case chrInfo:

            QuadSynth::charEv( chrInfo );
            console.infoByte( CONSTR("pattern"), currentPatNum ); 
            console.infoStr( CONSTR("tonality"), tonality->name ); 
            break;

         #endif

         default:
            return QuadSynth::charEv( code );
      }
      return true;
   }

   void dynamics()                           // perform a dynamic update
   {
      for ( byte i = 0; i < NUMVOCS; i++ )   // update dynamics for all voices
         voc[i]->dynamics();    

      arpegCue.dynamics();
      if ( arpegCue.pending() && ! deferVoc )
      {
         if ( pendingPatNum > 0 && arpegIdx == 1 ) 
         {
            if ( pendingPatNum <= NUM_PATTERNS )
            {
               currentPatNum = pendingPatNum;
               pattern       = patterns[ currentPatNum ];
               arpegIdx      = 1;
            }
            pendingPatNum = 0;
         }

         voc[ pattern[arpegIdx++] ]->noteOn( lastNote );
         if ( arpegIdx > pattern[0] )
            arpegIdx = 1;
         arpegCue.handled = true;
      }

   }

   boolean evHandler( obEvent e )            // event handler
   {
      switch ( e.type() )                    // branch on event type
      {
         case KEY_UP:                        // key has been released

            return true;                     // all keys play through to sustain

         case POT0:                          
         
            switch ( curFrame )
            {
               case   ENV_FRAME:  setAttack( e.getPotVal() >> 1 ); break;
               case ARPEG_FRAME:
               {
                  // select an arpeggiation sequence

                  const double scalar = (double )NUM_PATTERNS / 255.0;

                  byte potVal = e.getPotVal();
                  if ( potVal == 255 ) 
                     potVal = 254;

                  double mapVal = 1.00 + scalar * potVal;
                  pendingPatNum = (byte) mapVal;

                  break;
               }
            } 
            break;

         case POT1:                          
         
            switch ( curFrame )
            {
               case   ENV_FRAME:  setPostAttack( e.getPotVal() >> 1 ); break;
               case ARPEG_FRAME:  setRate( e.getPotVal() );            break;
            } 
            break;
         
         case BUT0_PRESS:                    // select arpeggiator frame

            selectFrame( ARPEG_FRAME );
            break;

         case BUT1_PRESS:                    // select enveloping frame

            selectFrame( ENV_FRAME );
            break;

         // because we are in PLAYTHRU mode, BUT1_DTAP events are converted 
         // by system to META_ONESHOT events, so we intercept them here.

         case BUT1_DTAP:                     // toggle tonality
         case META_ONESHOT:                  
               
            if ( tonality == majorTonality )
               setTonality( minorTonality );
            else
               setTonality( majorTonality );
            break;
            
         default:                            // pass on unhandled events
                                             
            return QuadSynth::evHandler(e);
      }
      return true;                     
   }

   void noteOn( key newNote )                // play a note
   {
      lastNote = newNote;
      setVoicing( tonality->getVoicing( newNote.position() ) );
   }

   void selectFrame( Frame frame )
   {
      switch( frame )
      {
         case ARPEG_FRAME:

            offLED( BLUE_LED );
            onLED( RED_LED );
            break;

         case ENV_FRAME:

            offLED( RED_LED );
            onLED( BLUE_LED );
            break;

         default:

            return;
      }
      curFrame = frame;
   }

   void setRate( byte val )                  // set arpeggiation rate
   {
      const byte   xLowMax   =  63;
      const byte   xSweetMax = 191;
      const byte   xHighMax  = 255;

      const double yLowMin   =  0.25;
      const double ySweetMin =  3.8;
      const double ySweetMax =  8.0;
      const double yHighMax  = 15.0;

      byte   xMin, xMax;
      double yMin, yMax;

      // set rate via 3 logarithmic zones

      if ( val <= xLowMax )
      {
         xMin = 0;
         xMax = xLowMax;
         yMin = yLowMin;
         yMax = ySweetMin;
      }
      else if ( val <= xSweetMax )
      {
         xMin = xLowMax+1;
         xMax = xSweetMax;
         yMin = ySweetMin;
         yMax = ySweetMax;
      }
      else
      {
         xMin = xSweetMax+1;
         xMax = 255;
         yMin = ySweetMax;
         yMax = yHighMax;
      }

      byte x       = val - xMin;
      byte xRange  = xMax - xMin;

      double xSquare = x * x;
      double rSquare = xRange * xRange;
 
      double newFreq = yMin + ( yMax - yMin ) * ( xSquare / rSquare );

      arpegCue.setFreq( newFreq );

      /* following lines useful for monitoring from console:
      
         console.newlntab();
         console.infoByte( CONSTR("val"), val );
         console.infoDouble( CONSTR("freq"), newFreq );

      */

   }

   void setTonality( Tonality *tonality )    // set tonality for voicings
   {
      this->tonality = tonality;
      setVoicing( tonality->getVoicing( lastNote.position() ) );
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



