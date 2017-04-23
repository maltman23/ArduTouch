/*
    DuoPoly.ino
 
    A duophonic macro-capable stereo wavetable synthesizer with dual sequencers.

    Target:   ArduTouch board
 
    ---------------------------------------------------------------------------
 
    Copyright (C) 2016, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
    ---------------------------------------------------------------------------
 */

#include "ArduTouch.h"                       // ArduTouch library declarations

about_program( DuoPoly, 2.27 )               // specify program name & version
set_baud_rate( 115200 )                      // specify serial baud-rate

#ifndef INTERN_CONSOLE
   #error This sketch requires __STNDLONE__ runtime model or higher (Model.h)
#endif

#ifndef KEYBRD_MENUS
   #error This sketch requires KEYBRD_MENUS to be defined (Model.h)
#endif

/*----------------------------------------------------------------------------*
 *                                 presets
 *----------------------------------------------------------------------------*/      


define_preset( Hive, "#!w1rebs6c3<``k3z``leb-s3c6<``t488\\S3jnhn`[`#" )

define_preset( Keltos, "#!w2r*1.5\\g230\\Ea62\\d153\\s192\\<`ebs2<```"
                       "lt275\\ebs3<``"
                        "S3c.m...c.m.....4gcx...s...3nm4sxs.3n.c.m...c.m.....4gcx...s...3nm4s3nm...`[`#" )

define_preset( Industry, "#!lw1t32.5\\eb-s5c4<``S2zsvc``reb-s2c1<``t325\\S7zc,bjbchvs``l`[#" )

define_preset( Omission, "#!rt205\\v205\\Tf.64\\d.375\\t1\\L<`Vf5.3\\d.46\\t26\\-<`"
                         "S4.cb5x.........4cb5s.........4cb5z........z4m..........``"
                         "lw1t205\\Tf4\\d1\\t3\\~<`S2c..3c.x.2b.nx.``[#" )

define_preset( Cadiz, "#!rw1t30\\V<f4.4\\d.13\\`"
                      "S1j......2j...3v..g.v.d.x.2m1j`f54\\g14\\`" 
                      "lw1t240\\g248\\"
                      "S2j4xv3j4jx3j4h2j4xv3j4jx3j4h2j4dg3j4jd3j4h2j4dg3j4gv3m4d``[#" )

begin_bank( myPresets )                // these presets will be loaded

   _preset( Hive )
   _preset( Keltos )
   _preset( Industry )
   _preset( Omission )
   _preset( Cadiz )

end_bank()

/*----------------------------------------------------------------------------*
 *                           define WaveBank
 *----------------------------------------------------------------------------*      
 *
 *  Note:   The following pre-defined wavetables are available:
 *
 *             Sine / dVox / Rood / Ether / SynStr
 *
 *----------------------------------------------------------------------------*/      

begin_bank( wavetables )               // these wavetables will be loaded
                         
   _wavetable( Sine )
   _wavetable( dVox )
   _wavetable( Rood )


   #ifndef USE_SERIAL_PORT             // if serial port is excluded there   
      _wavetable( SynStr )             // is room to load an extra wavetable :)
   #endif

end_bank()

/*----------------------------------------------------------------------------*/      


class Chnl : public Voice        // a Voice, considered as one of a pair  
{                                // of (possibly latched) audio channels
   public:

   Chnl   *other;                // other channel (for latching)

   byte   led;                   // led # for channel

   boolean amFreqLatching();     // returns true if currently freq latching 
   boolean charEv( char );       // handle a character event
   boolean evHandler( obEvent ); // handle an onboard event
   char    latchIcon();          // return char denoting current freq latching
   void    noteOn( key );        // initiate the playing of a note
   void    noteOff();            // release playing the current note

   #ifdef CONSOLE_OUTPUT
   const char *prompt();         // return object's prompt string
   #endif

   void setFreqDiff( double );   // latch frequency as difference to other chnl's
   void setFreqLatch( boolean ); // set frequency latching on/off
   void setFreqRatio( double );  // latch frequency as ratio to other chnl's
   void setName( const char* );  // set display name

   protected:

   void propFreq();              // propagate freq change to other chnl

   private:

   const char *name;             // name for display to console
   boolean latched;              // if true, channel frequency is latched
   boolean arithmetic;           // if true, frequency latching is arithmetic
                                 //    else, frequency latching is geometric
   double  freqOffset;           // frequency difference between channels
                                 // if ! arithmetic the freqOffset is a ratio

   void setArithmetic(boolean);  // set latching to Arithmetic or Geometic (t/f)

} ;  

/*----------------------------------------------------------------------------*/      

class ChnlSqnc : public Chnl, public StepSqnc  // Chnl with dedicated sequencer
{
   public: 

   ChnlSqnc();

   boolean charEv( char );       // handle a character event
   boolean evHandler( obEvent ); // handle an onboard event
   void    output( char * );     // write output to an audio buffer

   #ifdef KEYBRD_MENUS
   char    menu( key );          // map key event to character 
   #endif

} ;

/*----------------------------------------------------------------------------*/      

class DuoSynth : public StereoSynth
{
   public:

   boolean charEv( char );          // handle a character event
   boolean evHandler( obEvent );    // handle an onboard event
   void    dynamics();              // perform a dynamic update
   void    output(char*, char*);    // write one stereo buffer of audio
   void    setVol( byte );          // set the volume level
   void    setup();                 // execute at system setup

   #ifdef KEYBRD_MENUS
   char    menu( key );             // map key event to character 
   #endif

   #ifdef CONSOLE_OUTPUT
   const char *prompt();            // return object's prompt string
   #endif
} ;

/*----------------------------------------------------------------------------*/      

DuoSynth synth;                     // instantiate synthesizer 

void setup()
{
   ardutouch_setup( &synth );       // initialize ardutouch resources
}

void loop()
{
   ardutouch_loop();                // perform ongoing ardutouch tasks  
}                                             

/******************************************************************************
 *
 *                                   Chnl 
 *
 ******************************************************************************/

boolean Chnl::amFreqLatching()
{
   return ( latched && ! arithmetic );
}

boolean Chnl::charEv( char code )
{
   double gotDouble;                // return value from getDouble()

   switch ( code )
   {
      case 'f':                     // input a playback frequency

         Voice::charEv( 'f' );
         if ( latched ) propFreq();
         break;

      case 'k':                     // push console "piano keyboard" mode

         console.pushMode( &keybrd );
         break;

      case '+':                     // latch freq arithmetically

         if ( console.getDouble( CONSTR("+"), &gotDouble ) )
            setFreqDiff( gotDouble );
         break;

      case '-':

         if ( console.getDouble( CONSTR("-"), &gotDouble ) )
            setFreqDiff( -gotDouble );
         break;

      case '*':                     // latch freq geometrically

         if ( console.getDouble( CONSTR("*"), &gotDouble ) )
            setFreqRatio( gotDouble );
         break;

      case '/':

         if ( console.getDouble( CONSTR("/"), &gotDouble ) )
            setFreqRatio( 1.0 / gotDouble );
         break;

      case focusPUSH:               // mode is being pushed/restored
      case focusRESTORE:

         onLED( led );
         break;

      case '!':                     // reset

         latched    = false;
         arithmetic = false;
         // fall thru to Voice handler

      default:

         return Voice::charEv( code );
   }
   return true;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Chnl::noteOn
 *
 *  Desc:  Initiate the playing of a note
 *
 *  Args:  note             - key of note to be played  
 *
 *  Memb:  latched          - if true, channel frequency is latched
 *
 *----------------------------------------------------------------------------*/      

void Chnl::noteOn( key note )
{
   Voice::noteOn( note );
   if ( latched )
   {
      propFreq();
      other->trigger();
   }
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Chnl:noteOff
 *
 *  Desc:  Release playing the current note
 *
 *----------------------------------------------------------------------------*/      

void Chnl::noteOff()
{
   Voice::noteOff();
   if ( latched )
      other->charEv( chrRelease );
}

boolean Chnl::evHandler( obEvent ev )
{
   switch ( ev.type() )
   {
      case BUT1_DTAP:
               
         console.oneShotMenu();
         return true;

      default:

         return Voice::evHandler( ev );
   }
}

char Chnl::latchIcon()
{
#ifdef CONSOLE_OUTPUT
   return ( latched ? ( arithmetic ? '-' : '/' ) : ' ' );
#else
   return ' ';
#endif
}

#ifdef CONSOLE_OUTPUT
const char* Chnl::prompt()
{
   return name;
}
#endif

void Chnl::propFreq()
{
   double x = osc->getFreq();
   if ( arithmetic )
      other->setFreq( x - freqOffset );
   else
      other->setFreq( x / freqOffset );
}

void Chnl::setFreqDiff( double x )
{
   freqOffset = x;
   setFreq( other->osc->getFreq() + freqOffset );
   other->freqOffset = -x;
   setArithmetic( true );
}

void Chnl::setFreqRatio( double x )
{
   freqOffset = x;
   setFreq( other->osc->getFreq() * freqOffset );
   other->freqOffset = 1.0 / x;
   setArithmetic( false );
}

void Chnl::setName( const char* name )
{
#ifdef CONSOLE_OUTPUT
   this->name = name;
#endif
}

void Chnl::setArithmetic( boolean state )
{
   arithmetic = state;
   other->arithmetic = state;
   setFreqLatch( true );
}

void Chnl::setFreqLatch( boolean state )
{
   latched = state;
   other->latched = state;
}

/******************************************************************************
 *
 *                                ChnlSqnc 
 *
 ******************************************************************************/

ChnlSqnc::ChnlSqnc()
{
   subject = this;
}

boolean ChnlSqnc::charEv( char code )
{
   #ifdef CONSOLE_OUTPUT
   if ( code == chrInfo )
   {
      Chnl::charEv( chrInfo );
      console.newline();
      StepSqnc::info();
      return true;
   }
   #endif

   if ( StepSqnc::_charEv( code ) )  
      return true;
   else
      return Chnl::charEv( code );
}

boolean ChnlSqnc::evHandler( obEvent ev )
{
   switch ( ev.type() )
   {
      case BUT1_TPRESS:                 // push step programmer

         blinkLED( led );
         StepSqnc::_charEv('S');       
         return true;

      default:

         return Chnl::evHandler( ev );
   }
}

void ChnlSqnc::output( char *buf )
{
   StepSqnc::cont();
   Chnl::output( buf );
}

#ifdef KEYBRD_MENUS
char ChnlSqnc::menu( key k )
{
   switch ( k.position() )
   {
      case  5: return 't';
      case  6: return '|';
      case  7: return '[';
      case  8: return ']';
      default: return Chnl::menu( k );
   }
}
#endif

/******************************************************************************
 *
 *                                DuoSynth 
 *
 ******************************************************************************/

ChnlSqnc left, rght;
WaveOsc  oscL, oscR;
LPFilter lpfL, lpfR;
BSFilter bsfL, bsfR;

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

      case focusRESTORE:               // mode is being restored after child pop
   
         offLED( LEFT_LED );
         offLED( RIGHT_LED );
         break;

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
   if ( ! amMute() )
   {
      left.setGlobVol(x);
      rght.setGlobVol(x);
   }
}

/*----------------------------------------------------------------------------*
 *
 *  Name:   DuoSynth::setup
 *
 *  Desc:   Initialize the synthesizer:
 *
 *             1) set up channels 
 *             2) load wavetables
 *             3) load any presets 
 *             4) push mode for right channel 
 *
 *----------------------------------------------------------------------------*/      

void DuoSynth::setup()
{
   flags &= ~PLAYTHRU;

   left.other = &rght;                 
   left.led   = LEFT_LED;
   left.addEffect( &bsfL );
   left.addEffect( &lpfL );
   left.useOsc( &oscL );
   left.setName( CONSTR("left") );     

   rght.other = &left;                 
   rght.led   = RIGHT_LED;
   rght.addEffect( &bsfR );
   rght.addEffect( &lpfR );
   rght.useOsc( &oscR );
   rght.setName( CONSTR("right") );    

   wavebank.load( wavetables );                 // load wavetables
   presets.load( myPresets );                   // load presets
   reset();
   console.pushMode( &rght );
}  

