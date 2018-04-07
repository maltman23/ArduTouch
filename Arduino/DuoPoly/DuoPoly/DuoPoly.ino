/*
    DuoPoly.ino
 
    A duophonic stereo wavetable synthesizer with dual step sequencers.

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

#include "ArduTouch.h"                    // ArduTouch library declarations

about_program( DuoPoly, 2.47 )            // specify program name & version
set_baud_rate( 115200 )                   // specify serial baud-rate

#ifndef INTERN_CONSOLE
   #error This sketch requires __STNDLONE__ runtime model or higher (Model.h)
#endif

#ifndef KEYBRD_MENUS
   #error This sketch requires KEYBRD_MENUS to be defined (Model.h)
#endif

#ifndef IMPLICIT_SEQUENCER
   #error This sketch requires IMPLICIT_SEQUENCER to be defined (Model.h)
#endif

/*----------------------------------------------------------------------------*
 *                                 presets
 *----------------------------------------------------------------------------*/      

// Hive tempo = 244.0, noteValue(l) = eight

define_preset( Hive, "'w1rEbs6c3<``k3z``lEb-s3c6<``St244\\r3jnhn``[`" )

// Keltos tempo = 68.75, noteValue(l) = sixteenth

define_preset( Keltos, "'w2r*1.5\\g230\\ea62\\d153\\s192\\<`Ebs2<``At<f2.2\\```"
                       "lEbs3<``At<f2.2\\``"
                        "St68.75\\rS3c.m...c.m.....4gcx...s...3nm4sxs.3n.c.m...c.m.....4gcx...s...3nm4s3nm...`"
                        "`[`" )

// Industry tempo = 162.5, noteValue(l, r) = (5*quarter, eigth)

define_preset( Industry, "'lw1Eb-s5c4<``St162.5\\rV120\\2zsvc```"
                         "rEb-s2c1<``St162.5\\r7zc,bjbchvs```l`[" )

// Omission tempo = 102.5 noteValue(l,r) = (eight, eight)

define_preset( Omission, "'rv205\\Tf.64\\d.375\\t1\\L<`Vf5.3\\d.46\\t26\\-<`"
                         "St102.5\\r4.cb5x.........4cb5s.........4cb5z........z4m..........```"
                         "lw1Tf4\\d1\\t3\\~<`St102.5\\r2c..3c.x.2b.nx.```[" )

// Cadiz tempo = 120 noteValue(l,r) = (eight, quarter)

define_preset( Cadiz, "'rw1V<f4.4\\d.13\\`"
                      "St30\\rQ1j......2j...3v..g.v.d.x.2m1j``f54\\g14\\`" 
                      "lw1g248\\"
                      "St120\\r2j4xv3j4jx3j4h2j4xv3j4jx3j4h2j4dg3j4jd3j4h2j4dg3j4gv3m4d```[" )

begin_bank( myPresets )             // these presets will be loaded

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

begin_bank( wavetables )            // these wavetables will be loaded
                         
   _wavetable( Sine )
   _wavetable( dVox )

   #ifndef USE_SERIAL_PORT          // if serial port is excluded 
      _wavetable( Rood )            //    use Rood wavetable
   #endif

end_bank()

/*----------------------------------------------------------------------------*/      

class Chnl : public StockVoice      // stock voice with sequencer
{                                
   typedef StockVoice super;        // superclass is StockVoice

   public:

   Tremolo tremolo;                 // built-in tremolo effect
   Chnl   *other;                   // other channel (for latching)

   byte   led;                      // led # for channel

   Chnl();

   boolean charEv( char );          // handle a character event
   void    dynamics();              // perform a dynamic update
   boolean evHandler( obEvent );    // handle an onboard event
   char    latchIcon();             // return char denoting current freq latching
   void    noteOn( key );           // turn a note on
   void    noteOff( key );          // turn a note off

   #ifdef KEYBRD_MENUS
   char    menu( key );             // map key event to character 
   #endif

   #ifdef CONSOLE_OUTPUT
   const char *prompt();            // return object's prompt string
   #endif

   void setFreqDiff( double );      // latch frequency as difference to other chnl's
   void setFreqLatch( boolean );    // set frequency latching on/off
   void setFreqRatio( double );     // latch frequency as ratio to other chnl's
   void setName( const char* );     // set display name

   private:

   const char *name;                // name for display to console

   boolean latched;                 // if true, channel is latched

   boolean propFreq;                // if true, propagate freq to latched chnl
   boolean propTrig;                // if true, propagate trigger to latched chnl

   boolean arithmetic;              // if true, frequency latching is arithmetic
                                    //    else, frequency latching is geometric

   double  freqOffset;              // frequency difference between channels
                                    // if ! arithmetic the freqOffset is a ratio

   void setArithmetic(boolean);     // set latching to Arithmetic or Geometic (t/f)

} ;  

/*----------------------------------------------------------------------------*/      

class DuoSynth : public VoxSynth
{
   typedef VoxSynth super;          // superclass is VoxSynth

   Chnl    *left, *rght;            // ptrs to left and right channels

   public:

   boolean charEv( char );          // handle a character event
   boolean evHandler( obEvent );    // handle an onboard event
   Osc    *newOsc( byte nth );      // create oscillator for nth voice
   Voice  *newVox( byte nth );      // create nth voice 
   void    output( char*, char* );  // write stereo output
   void    setup();                 // executed at system startup
   void    welcome();               // perform post-reset startup tasks

   #ifdef KEYBRD_MENUS
   char    menu( key );             // map key event to character 
   #endif

   #ifdef CONSOLE_OUTPUT
   const char *prompt()             // return object's prompt string
   {
      return CONSTR("main");
   }
   #endif
} ;

/*----------------------------------------------------------------------------*/      

DuoSynth duoSynth;                  // instantiate synthesizer 

void setup()
{
   ardutouch_setup( &duoSynth );    // initialize ardutouch resources
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

Chnl::Chnl()
{
   useSequencer( new StepSqnc() );
   addAmpMod( &this->tremolo );
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Chnl::charEv
 *
 *  Desc:  Process a character event.
 *
 *  Args:  code             - character to process
 *
 *  Memb: +arithmetic       - freq latching is arithmetic (else, geometric)
 *        +latched          - channel frequency is latched
 *         led              - led # for channel
 *        +propFreq         - propagate freqeuncy to latched channel
 *        +propTrig         - propagate trigger to latched channel
 *         tremolo          - built-in tremolo effect
 *
 *  Rets:  status           - true if character was handled
 *
 *----------------------------------------------------------------------------*/      

boolean Chnl::charEv( char code )
{
   double gotDouble;                // return value from getDouble()

   switch ( code )
   {
      case 'f':                     // input a playback frequency

         super::charEv( 'f' );
         if ( latched ) propFreq = true;
         break;

      case 'T':                     // push tremolo control

         console.pushMode( &this->tremolo );
         break;

      case '+':                     // latch freq arithmetically
      {
         if ( console.getDouble( CONSTR(""), &gotDouble ) )
            setFreqDiff( gotDouble );
         break;
      }
      case '*':                     // latch freq geometrically
      {
         if ( console.getDouble( CONSTR(""), &gotDouble ) )
            setFreqRatio( gotDouble );
         break;
      }
      case focusPUSH:               // mode is being pushed/restored
      case focusRESTORE:

         onLED( led );
         break;

      case '!':                     // reset

         latched    = false;
         arithmetic = false;
         propFreq   = false;
         propTrig   = false;

         ((WaveOsc *)osc)->setTableFromBank( 0 );

         // fall thru to super handler

      default:

         return super::charEv( code );
   }
   return true;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Chnl::dynamics
 *
 *  Desc:  Perform normal voice dynamics. In addition, propagate any pending
 *         frequency change and trigger to latched channel.
 *
 *  Memb:  arithmetic       - freq latching is arithmetic (else, geometric)
 *         latched          - channel frequency is latched
 *         other            - ptr to other channel 
 *        +propFreq         - propagate freqeuncy to latched channel
 *        +propTrig         - propagate trigger to latched channel
 *
 *----------------------------------------------------------------------------*/      

void Chnl::dynamics()
{
   super::dynamics();
   if ( latched )
   {
      if ( propFreq )
      {
         double otherFreq = pendFreq;
         if ( arithmetic )
            otherFreq -= freqOffset;
         else
            otherFreq /= freqOffset;
         other->setFreq( otherFreq );
         propFreq = false;
      }
      if ( propTrig )
      {
         other->trigger();
         propTrig = false;
      }
   }
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Chnl::evHandler
 *
 *  Desc:  Handle an onboard event.
 *
 *  Args:  ev               - onboard event
 *
 *  Memb:  sqnc             - ptr to sequencer
 *
 *  Rets:  status           - true if the event was handled
 *
 *----------------------------------------------------------------------------*/      

boolean Chnl::evHandler( obEvent ev )
{
   switch ( ev.type() )
   {
      case BUT1_TPRESS:                 // record a new sequence

         blinkLED( led );
         sqnc->record();       
         return true;

      default:

         return super::evHandler( ev );
   }
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
 *        +propFreq         - propagate freqeuncy to latched channel
 *        +propTrig         - propagate trigger to latched channel
 *
 *----------------------------------------------------------------------------*/      

void Chnl::noteOn( key note )
{
   super::noteOn( note );
   if ( latched )
   {
      propTrig = true;
      propFreq = true;
   }
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Chnl:noteOff
 *
 *  Desc:  Release playing the current note
 *
 *----------------------------------------------------------------------------*/      

void Chnl::noteOff( key note )
{
   super::noteOff( note );
   if ( latched )
      other->charEv( chrRelease );
}

#ifdef KEYBRD_MENUS

/*----------------------------------------------------------------------------*
 *
 *  Name:  Chnl::menu
 *
 *  Desc:  Given a key, return a character (to be processed via charEv()). 
 *
 *  Args:  k                - key
 *
 *  Rets:  c                - character (0 means "no character")
 *
 *  Note:  If a sketch is compiled with KEYBRD_MENUS defined, then this method 
 *         can be used to map the onboard keys to characters which the system 
 *         will automatically feed to the charEv() method.
 *
 *         This method is only called by the system if the MENU flag in this
 *         object is set (in the ::flags byte inherited from Mode), or if the
 *         keyboard is in a "oneShot menu selection" state.
 *
 *         The key mapping (inclusive of super class) is as follows:
 *
 *           -------------------------------------------------
 *           |   |   |   |   |   |   |   |   |   |   |   |   |
 *           |   |   |   |   |   |   |   |   |   |   |   |   |
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |   | V |   | T |   |   | | |   | ] |   | . |   | 
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |    ___     ___    |    ___     ___     ___    | 
 *           |     |       |     |     |       |       |     |
 *           |     |       |     |     |       |       |     |
 *           |  E  |   w   |  e  |  t  |   [   |   <   |  !  |
 *           |     |       |     |     |       |       |     |
 *           |     |       |     |     |       |       |     |
 *           -------------------------------------------------
 *
 *----------------------------------------------------------------------------*/      

char Chnl::menu( key k )
{
   switch ( k.position() )
   {
      case  0: return 'E';
      case  1: return 'V';
      case  3: return 'T';
      case  4: return 'e';
      case  5: return 't';
      case  6: return '|';
      case  7: return '[';
      case  8: return ']';
      default: return super::menu( k );
   }
}
#endif

char Chnl::latchIcon()
{
#ifdef CONSOLE_OUTPUT
   return ( latched ? ( arithmetic ? '+' : '*' ) : ' ' );
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
 *                                DuoSynth 
 *
 ******************************************************************************/

/*----------------------------------------------------------------------------*
 *
 *  Name:   DuoSynth::setup
 *
 *  Desc:   Initialize the synthesizer:
 *
 *             1) set up channels 
 *             2) load wavetables
 *             3) load any presets 
 *
 *----------------------------------------------------------------------------*/      

void DuoSynth::setup()
{
   setupVoices(2);

   left = (Chnl *)vox[0];
   rght = (Chnl *)vox[1];

   left->other = rght;
   left->led   = LEFT_LED;
   left->setName( CONSTR("left") );     

   rght->other = left;                 
   rght->led   = RIGHT_LED;
   rght->setName( CONSTR("right") );    

   wavebank.load( wavetables );                 // load wavetables
   presets.load( myPresets );                   // load presets
}  

/*----------------------------------------------------------------------------*
 *
 *  Name:  DuoSynth::welcome
 *
 *  Desc:  Perform post-reset startup tasks.
 *
 *  Memb:  rght             - ptr to right channel
 *
 *----------------------------------------------------------------------------*/      

void DuoSynth::welcome()
{
   console.pushMode( rght );
}

boolean DuoSynth::charEv( char code )
{
   switch ( code )
   {
     case 'l':

         console.pushMode( left );
         break;

      case 'r':

         console.pushMode( rght );
         break;

      case 'w':

         if ( wavebank.choose() )
         {
            ((WaveOsc *)rght->osc)->setTableFromBank( wavebank.choice() );
            ((WaveOsc *)left->osc)->setTableFromBank( wavebank.choice() );
         }
         break;

      case '+':
            
         rght->setFreqDiff( rght->osc->getFreq() - left->osc->getFreq() );
         inform();
         break;

      case '*':

         rght->setFreqRatio( rght->osc->getFreq() / left->osc->getFreq() );
         inform();
         break;

      case 'u':                        // unlatch 
            
         rght->setFreqLatch( false );
         inform();
         break;

      case '|':                        // propagate transport commands
      case '[':
      case ']':

         rght->charEv( code );
         left->charEv( code );
         break;

      #ifdef CONSOLE_OUTPUT
      case chrInfo:                    // display object info to console

         super::charEv( chrInfo );
         console.newlntab();
         left->brief();
         console.print( left->latchIcon() );
         console.space();
         rght->brief();
         break;

      #endif

      case focusRESTORE:               // mode is being restored after child pop
   
         offLED( LEFT_LED );
         offLED( RIGHT_LED );
         break;

      case '!':                        // reset
      
      {  const double initHz   = 311.13;     // initial left channel frequency
         const double ratio5th = 1.4983;     // initial left-right freq ratio 
      
         super::charEv( '!' );

         flags |= MENU;                      // maintain status quo 
         left->keybrd.setMute( false );      // maintain status quo 
         rght->keybrd.setMute( false );      // maintain status quo 

         left->setFreq( initHz );
         rght->setFreq( initHz * ratio5th ); 
         break;
      }

      default:

         return super::charEv( code );
   }
   return true;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  DuoSynth::evHandler
 *
 *  Desc:  Handle an onboard event.
 *
 *  Args:  ev               - onboard event
 *
 *  Rets:  status           - true if the event was handled
 *
 *----------------------------------------------------------------------------*/      

boolean DuoSynth::evHandler( obEvent ev )
{
   switch ( ev.type() )
   {
      case BUT0_PRESS:

         console.pushMode( left );
         return true;

      case BUT1_PRESS:

         console.pushMode( rght );
         return true;

      default:

         return super::evHandler( ev );
   }
}

Osc *DuoSynth::newOsc( byte nth )
{
   return new WaveOsc();
}

Voice *DuoSynth::newVox( byte nth )
{
   static Chnl chnLeft, chnRight;

   Chnl *c;
   switch ( nth )
   {
      case 0: c = &chnLeft;  break;
      case 1: c = &chnRight; break;
   }
   c->addEffect( new BSFilter() );
   c->addEffect( new LPFilter() );
   return c;
}

void DuoSynth::output( char *bufL, char* bufR )                     
{ 
   left->output( bufL );
   rght->output( bufR );
}

#ifdef KEYBRD_MENUS

/*----------------------------------------------------------------------------*
 *
 *  Name:  DuoSynth::menu
 *
 *  Desc:  Given a key, return a character (to be processed via charEv()). 
 *
 *  Args:  k                - key
 *
 *  Rets:  c                - character (0 means "no character")
 *
 *  Note:  If a sketch is compiled with KEYBRD_MENUS defined, then this method 
 *         can be used to map the onboard keys to characters which the system 
 *         will automatically feed to the charEv() method.
 *
 *         This method is only called by the system if the MENU flag in this
 *         object is set (in the ::flags byte inherited from Mode), or if the
 *         keyboard is in a "oneShot menu selection" state.
 *
 *         The key mapping (inclusive of super class) is as follows:
 *
 *           -------------------------------------------------
 *           |   |   |   |   |   |   |   |   |   |   |   |   |
 *           |   |   |   |   |   |   |   |   |   |   |   |   |
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |   |   |   | u |   |   | | |   | ] |   | . |   | 
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |    ___     ___    |    ___     ___     ___    | 
 *           |     |       |     |     |       |       |     |
 *           |     |       |     |     |       |       |     |
 *           |  p  |   w   |  +  |  *  |   [   |   <   |  !  |
 *           |     |       |     |     |       |       |     |
 *           |     |       |     |     |       |       |     |
 *           -------------------------------------------------
 *
 *----------------------------------------------------------------------------*/

char DuoSynth::menu( key k )
{
   switch ( k.position() )
   {
      case  2: return 'w';
      case  3: return 'u';
      case  4: return '+';
      case  5: return '*';
      case  6: return '|';
      case  7: return '[';
      case  8: return ']';
      default: return Synth::menu(k);
   }
}
#endif


