/*
    ZapShot.ino

    A stereo synth using 2 sweeping quantized square wave oscillators.
 
    Target:   ArduTouch board
 
    How to use:

    Start playing the keyboard, you will hear a sound somewhat like the
    video game "Pong". Press the right button and the blue LED will start
    flashing. Now ZapShot is in "auto-trigger" mode, and the sound will 
    automatically keep playing until the right button is pressed again. 
    This frees up your hands to twist the pots :)

    There are six presets. To select a preset double-tap the right button 
    and then press a "white" key.

    The following presets are available:

         Key C - "Pong"
         Key D - "Bong"
         Key E - "AirRaid"
         Key F - "Stuka"
         Key G - "Alarms"
         Key A - "Quizzle"

    ---------------------------------------------------------------------------
 
    Copyright (C) 2018, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
    ---------------------------------------------------------------------------
 */

#include "ArduTouch.h"                       // use the ArduTouch library 

about_program( ZapShot, 1.30 )               // specify sketch name & version

  
/*----------------------------------------------------------------------------*
 *                              ZapVox macros
 *----------------------------------------------------------------------------*/      

// The macros that follow are intended for ZapVox ("Zap Voice") objects.
//
// In this synth ZapVox macros are indirectly executed via the 'M' character 
// in a preset string. For example, if a preset string contains "M0\\", 
// then macro 0 in the macros bank (see below -- maps to Pong) will be 
// executed for each ZapVox.

#define define_macro(x, y)                                                 \
                                                                           \
   const char macroName##x[] PROGMEM = { #x };                             \
   const char macroData##x[] PROGMEM = { y };

#define _macro(x) _member( macroData##x, macroName##x )

define_macro( Pong,    "S<" )

define_macro( Bong,    "OEr232\\l226\\`Br144\\l139\\" )

define_macro( AirRaid, "Ebs6\\c1\\<``"
                       "Oat\\b183\\e154\\s12\\"
                       "Br184\\l158\\`Er152\\l134\\`Sl2\\r16\\`"
                       )               

define_macro( Stuka,  "Ebs6\\c2\\<``"
                       "Oat\\b10\\e100\\s8\\"
                       "Br10\\l24\\`Er80\\l138\\`Sl3\\r8\\`"
                       )               


define_macro( Alarms,  "Ebs6\\c4\\-<``"
                       "Oat\\b90\\e60\\s16\\p60\\Pl3\\r102\\`"
                       )

// the macros bank orders the above macros, so that they can be referenced
// by number and executed in preset strings via the "Mx\\" string, where x 
// is an ordinal 0, 1, ...

begin_bank( macros )

   _macro( Pong )                      
   _macro( AirRaid )                      
   _macro( Stuka )                       
   _macro( Alarms )                       
   _macro( Bong )                      

end_bank()

/*----------------------------------------------------------------------------*
 *                                 presets
 *----------------------------------------------------------------------------*/      

define_preset( Empty,   "" )

define_preset( Pong,    "!M0\\"                // execute macro 0 (Pong) per voice
                        "0OLt\\``"             // set latching on
                        "1ON5\\X250\\``"       // set osc 1 min/max edglen to 5/250
                        "P<P36\\f.25\\d.5\\p`" // set panning pin/freq/depth
                        )

define_preset( Bong,    "!M4\\"                  // execute Bong macro
                        "0OLt\\at\\"             // set latching, autotrig on
                        "b142\\e230\\``"         // set osc 0 begin/end
                        "1ON5\\X250\\"           // set osc 1 min/max edglen to 5/250
                        "b142\\e230\\``"         // set osc 1 begin/end
                        "P<P129\\f5.118\\d1\\p`" // set panning pin/freq/depth
                        "t"                      // trigger
                        )

define_preset( AirRaid, "!M1\\d.2\\t" )  

define_preset( Stuka,   "!M2\\1v128\\`d10.35\\P<f.1\\d.75\\`t" )

define_preset( Alarms,  "!M3\\1OX250\\``t" )

define_preset( Quizzle, "!0Oaf\\Lt\\p2\\s8\\b50\\e35\\Bl45\\r60\\`El25\\r40\\```"
                        "0v230\\Ebs2\\c3\\-<```"
                        "1Oat\\p20\\r2\\s16\\b4\\e30\\Bl2\\r8\\`El25\\r35\\```"
                        "1Ebs6\\c4\\-<```"
                        "P<d.25\\`"
                        )

begin_bank( myPresets )                   // these presets will be loaded

   _preset( Pong )                        // C
   _preset( Empty )                       // C#
   _preset( Bong )                        // D
   _preset( Empty )                       // D#
   _preset( AirRaid )                     // E
   _preset( Stuka )                       // F
   _preset( Empty )                       // F#
   _preset( Alarms )                      // G
   _preset( Empty )                       // G#
   _preset( Quizzle )                     // A
   _preset( Empty )                       // A#
   _preset( Empty )                       // B

end_bank()


class PeakParam : public ParamSend
{
   typedef  ParamSend super;        // superclass is ParamSend

   public:

   boolean charEv( char code )    
   {
      switch ( code )
      {
         case '!':                  // perform a reset

            super::charEv( code );
            setMinMax( 0, 127 );
            setRange( 0, 127 );
            break;

         default:

            return super::charEv( code );  
      }
   }

   PROMPT_STR( Peak )
} ;

class EdgeParam : public ParamSend
{
   typedef  ParamSend super;       // superclass is ParamSend

   public:

   boolean charEv( char code )    
   {
      switch ( code )
      {
         case '!':                        // perform a reset

            super::charEv( code );
            setMinMax( 1, 255 );
            setRange( 255, 1 );
            break;

         default:

            return super::charEv( code );  
      }
   }

} ;

class BeginParam : public EdgeParam { public: PROMPT_STR( Begin ) };
class EndParam   : public EdgeParam { public: PROMPT_STR( End ) };

class StretchParam : public ParamSend
{
   typedef ParamSend super;         // superclass is ParamSend

   public:

   boolean charEv( char code )    
   {
      switch ( code )
      {
         case '!':                  // perform a reset

            super::charEv( code );
            setMinMax( 1, 16 );
            setRange( 1, 16 );
            break;

         default:

            return super::charEv( code );  
      }
   }

   PROMPT_STR( Stretch )
} ;


/******************************************************************************
 *
 *                                 ZapOsc 
 *                                                                            
 ******************************************************************************/

class ZapOsc : public SweepOsc
{
   typedef  SweepOsc super;         // superclass is SweepOsc

   byte  unsBeg;                    // unscaled beginning edge length
   byte  unsEnd;                    // unscaled ending edge length

   byte   minLen;                   // minimum edge length during sweep
   byte   maxLen;                   // maximum edge length during sweep
   double minmaxScaler;             // scales from 1:255 to min:max

   public:

   byte vPotMap[ NumPots ];         // virtual pot map for each physical port

   BeginParam   vpBegin;            // virtual pot for beginning edge length
   EndParam     vpEnd;              // virtual pot for ending edge length
   PeakParam    vpPeak;             // virtual pot for peak amplitude
   StretchParam vpStretch;          // virtual pot for stretch factor

   bool charEv( char code );        // process a character event
   byte map( byte );                // map an edge length into minmax range
   void scaleSweep(double, double); // prepare for a scaled sweep
   void setBegin( byte );           // set beginning edge length of sweep
   void setEnd( byte );             // set ending edge length of sweep
   void setMinMax( byte, byte );    // set min & max edge lengths during sweep
   void unscaleSweep();             // prepare for an unscaled sweep

   PROMPT_STR( zapOsc ); 
} ;


/*----------------------------------------------------------------------------*
 *
 *  Name:  ZapOsc::charEv
 *
 *  Desc:  Process a character event.
 *
 *  Args:  code             - character to process
 *
 *  Glob:  NumPots          - number of onboard pots
 *
 *  Memb:  vpBegin          - virtual pot for beginning edge length
 *         vpEnd            - virtual pot for ending edge length
 *         vpStretch        - virtual pot for stretch factor
 *
 *  Rets:  status           - true if character was handled
 *
 *----------------------------------------------------------------------------*/      

bool ZapOsc::charEv( char code )    
{
   switch ( code )
   {
     #ifdef INTERN_CONSOLE
                                          
      case 'B':                           // push virtual pot for begin param

         console.pushMode( &vpBegin );
         break;

      case 'E':                           // push virtual pot for end param

         console.pushMode( &vpEnd );
         break;

      case 'N':                           // set minimum edge len during sweep

         if ( console.getByte( CONSTR("miNLen"), &minLen ) )
            setMinMax( minLen, maxLen );
         break;

      case 'P':                           // push virtual pot for peak amplitude

         console.pushMode( &vpPeak );
         break;

      case 'S':                           // push virtual pot for stretch param

         console.pushMode( &vpStretch );
         break;

      case 'X':                           // set maximum edge len during sweep

         if ( console.getByte( CONSTR("maXLen"), &maxLen ) )
            setMinMax( minLen, maxLen );
         break;

     #endif // INTERN_CONSOLE

     #ifdef CONSOLE_OUTPUT

      case chrInfo:                       // display object info to console

         super::charEv( chrInfo );

         console.newlntab();
         vpPeak.brief();
         vpBegin.brief();
         vpEnd.brief();
         vpStretch.brief();

         console.newlntab();
         console.infoByte( CONSTR("miNlen"), minLen );
         console.infoByte( CONSTR("maXlen"), maxLen );

         break;

     #endif // CONSOLE_OUTPUT

      case '!':                           // perform a reset

         setMinMax( 1, 255 );
         super::charEv( code );  

         vpBegin.reset();
         vpEnd.reset();
         vpPeak.reset();
         vpStretch.reset();

         vpBegin.connect( this, ptr1ByteMethod( &ZapOsc::setBegin ) );
         vpEnd.connect( this, ptr1ByteMethod( &ZapOsc::setEnd ) );
         vpPeak.connect( this, ptr1ByteMethod( &ZapOsc::setPeak ) );
         vpStretch.connect( this, ptr1ByteMethod( &ZapOsc::setStretch ) );

         break;

      default:

         return super::charEv( code );    // pass unhandled events to super class
   }
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  ZapOsc::map
 *
 *  Desc:  Set the ending edge length for the sweep.
 *
 *  Args:  length           - value for ending edge length
 *
 *  Memb: +unsEnd           - unscaled ending edge length
 *
 *----------------------------------------------------------------------------*/

byte ZapOsc::map( byte len )
{
   double mapLen = minLen + (len-1)*minmaxScaler + 0.5;
   return (byte )mapLen;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  ZapOsc::scaleSweep
 *
 *  Desc:  Prepare for a scaled sweep.
 *
 *  Args:  botScale         - scaling coefficient for bottom edge length
 *         topScale         - scaling coefficient for top edge length
 *
 *  Memb:  unsBeg           - unscaled beginning edge length
 *         unsEnd           - unscaled ending edge length
 *
 *----------------------------------------------------------------------------*/

void ZapOsc::scaleSweep( double botScale, double topScale )             
{
   double scaBeg, scaEnd;
   if ( unsEnd > unsBeg )
   {
      scaEnd = unsEnd - ((unsEnd - unsBeg) * botScale);
      scaBeg = unsBeg + ((scaEnd - unsBeg) * topScale);
   }
   else
   {
      scaBeg = unsBeg - ((unsBeg - unsEnd) * botScale);
      scaBeg = unsBeg + ((scaBeg - unsEnd) * topScale);
   }
   super::setBegin( map(scaBeg) );
   super::setEnd( map(scaEnd) );
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  ZapOsc::setBegin
 *
 *  Desc:  Set the beginning edge length for the sweep.
 *
 *  Args:  length           - value for beginning edge length
 *
 *  Memb: +unsBeg           - unscaled beginning edge length
 *
 *----------------------------------------------------------------------------*/

void ZapOsc::setBegin( byte val )
{
   unsBeg = val ? val : 1;          // must be at least 1
   super::setBegin( map(unsBeg) );
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  ZapOsc::setEnd
 *
 *  Desc:  Set the ending edge length for the sweep.
 *
 *  Args:  length           - value for ending edge length
 *
 *  Memb: +unsEnd           - unscaled ending edge length
 *
 *----------------------------------------------------------------------------*/

void ZapOsc::setEnd( byte val )
{
   unsEnd = val ? val : 1;             // must be at least 1
   super::setEnd( map(unsEnd) );
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  ZapOsc::setMinMax
 *
 *  Desc:  Set minimum and maximum edge lengths for sweep.
 *
 *  Memb:  min              - minimum edge length during sweep 
 *         max              - maximum edge length during sweep 
 *
 *  Memb: +maxLen           - maximum edge length during sweep 
 *        +minLen           - minimum edge length during sweep 

 *
 *----------------------------------------------------------------------------*/

void ZapOsc::setMinMax( byte min, byte max )
{
   if ( min <= max )
   {
      minLen = min;
      maxLen = max;
   }
   else
   {
      minLen = max;
      maxLen = min;
   }
   if ( minLen == 0 ) minLen = 1;
   if ( maxLen <  2 ) maxLen = 2;

   int range    = maxLen+1-minLen;
   minmaxScaler = range / 255.0;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  ZapOsc::unscaleSweep
 *
 *  Desc:  Prepare for an unscaled sweep.
 *
 *  Memb:  unsBeg           - unscaled beginning edge length
 *         unsEnd           - unscaled ending edge length
 *
 *----------------------------------------------------------------------------*/

void ZapOsc::unscaleSweep()             
{
   super::setBegin( map(unsBeg) );
   super::setEnd( map(unsEnd) );
}

/******************************************************************************
 *
 *                             MasterZapOsc 
 *                                                                            
 ******************************************************************************/

class MasterZapOsc : public ZapOsc
{
   typedef  ZapOsc super;           // superclass is ZapOsc

   public:

   SweepOsc *slave;                 // ptr to slave oscillator
   bool      latched;               // if true, slave osc sweeps are latched

   byte      statusLED;             // display run-time status to this LED #

   MasterZapOsc()
   {
      slave     = NULL;             // no slave oscillator specified
      statusLED = NULL_LED;         // by default there is no assigned LED
   }

   bool charEv( char code );        // process a character event
   void refreshLED();               // do a visual refresh of dedicated LED 
   void startSweep();               // initiate a new sweep from begin to end

   PROMPT_STR( mastZap )
} ;

/*----------------------------------------------------------------------------*
 *
 *  Name:  MasterZapOsc::charEv
 *
 *  Desc:  Process a character event.
 *
 *  Args:  code             - character to process
 *
 *  Memb:  latched          - if true, slave osc sweeps are latched
 *
 *  Rets:  status           - true if character was handled
 *
 *----------------------------------------------------------------------------*/      

bool MasterZapOsc::charEv( char code )    
{
   switch ( code )
   {
     #ifdef INTERN_CONSOLE
                                          
     #endif // INTERN_CONSOLE

     case 'a':                            // set autotrig on/off

         super::charEv( code );
         refreshLED();
         break;

     case 'L':                            // set latched status 

         console.getBool( CONSTR("Latched"), &latched );
         break;

     #ifdef CONSOLE_OUTPUT

      case chrInfo:                       // display object info to console

         super::charEv( chrInfo );

         console.newlntab();
         console.infoBool( CONSTR("Latched"), latched );

         break;

     #endif // CONSOLE_OUTPUT

      case '!':                           // perform a reset

         super::charEv( code );
         latched = false;
         break;

      default:

         return super::charEv( code );    // pass unhandled events to super class
   }
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  MasterZapOsc::startSweep
 *
 *  Desc:  Initiate a sweep of the oscillator.
 *
 *----------------------------------------------------------------------------*/

void MasterZapOsc::startSweep()
{
   super::startSweep();

   if ( latched && slave ) 
   {
      slave->trigger();
      // console.print( '.' );
      // console.romprint( slave->prompt() );
   }
}

void MasterZapOsc::refreshLED()
{
   if ( autotrig )
      blinkLED( statusLED );
   else
      offLED( statusLED );
}

/******************************************************************************
 *
 *                               Scaling 
 *                                                                            
 ******************************************************************************/

class Scaling : public Control
{
   typedef  Control super;          // superclass is Control

   protected:

   double perKeyScaler;             // keybrd scaler per key (pre-divided by 255)

   byte bot;                        // keybrd scaling for bottom edge (parts/255)
   byte top;                        // keybrd scaling for top edge (parts/255)

   public:

   bool charEv( char code )         // process a character event
   {
      switch ( code )
      {
        #ifdef INTERN_CONSOLE
      
         case 'b':                  // set bot scaling parameter 
      
            if ( console.getByte( CONSTR("bot"), &bot ) )
               setBot( bot );
            break;

         case 't':                  // set top scaling parameter 
      
            if ( console.getByte( CONSTR("top"), &top ) )
               setTop( top );
            break;

       #endif // INTERN_CONSOLE

       #ifdef CONSOLE_OUTPUT

         case chrInfo:              // display object info to console

         super::charEv( code );
         console.infoByte( CONSTR("bot"), bot );
         console.infoByte( CONSTR("top"), top );
         break;

     #endif // CONSOLE_OUTPUT

      case '!':                           // perform a reset

         super::charEv( code );  
         setMaxScaling( .5 );
         setBot( 192 );                   // default 75.0% scaling
         setTop( 32 );                    // default 12.5% scaling
         break;

      default:

         return super::charEv( code );    // pass unhandled events to super class
     }
     return true;
   }

   double getBotScaler( byte keyNum )
   {
      double scaler = perKeyScaler * bot * keyNum;
      // console.infoDouble( CONSTR("botScaler"), scaler );
      return scaler;
   }

   double getTopScaler( byte keyNum )
   {
      double scaler = perKeyScaler * top * ( NumKeys - 1 - keyNum );
      // console.infoDouble( CONSTR("topScaler"), scaler );
      return scaler;
   }

   void setBot( byte val )
   {
      bot = val;
   }

   void setMaxScaling( double ratio )
   {
      const double MAX_SCALING = .75;

      if ( ratio > MAX_SCALING ) 
         ratio = MAX_SCALING;

      perKeyScaler = ratio / (255 * (NumKeys-1));  // pre-divide by 255 
   }

   void setTop( byte val )
   {
      top = val;
   }

   PROMPT_STR( Scaling )
} ;

/******************************************************************************
 *
 *                                 ZapVox 
 *                                                                            
 ******************************************************************************/

class ZapVox : public Voice
{
   typedef  Voice super;            // superclass is Voice

   Scaling  scaling;                // keybrd scaling control
   BSFilter bsfilter;               // bit-shift filter control 

   byte  pendKey;                   // physical key # causing pending trigger

   public:

   ZapOsc   *zapOsc;                // ptr to resident zap oscillator 

   ZapVox() 
   {
      addEffect( &this->bsfilter );
   }

   bool  charEv( char code );       // process a character event
   void  dynamics();                // update dynamics
   void  noteOn( key );             // turn a note on

} ;

/*----------------------------------------------------------------------------*
 *
 *  Name:  ZapOsc::charEv
 *
 *  Desc:  Process a character event.
 *
 *  Args:  code             - character to process
 *
 *  Glob:  NumPots          - number of onboard pots
 *
 *  Memb:  vpBegin          - virtual pot for beginning edge length
 *         vpEnd            - virtual pot for ending edge length
 *         vpStretch        - virtual pot for stretch factor
 *
 *  Rets:  status           - true if character was handled
 *
 *----------------------------------------------------------------------------*/      

bool ZapVox::charEv( char code )    
{
   switch ( code )
   {
     #ifdef INTERN_CONSOLE
      
      case 'S':                           // push scaling control

         console.pushMode( &scaling );
         break;

     #endif // INTERN_CONSOLE

     #ifdef CONSOLE_OUTPUT

      case chrInfo:                       // display object info to console

         super::charEv( chrInfo );

         console.newlntab();
         scaling.brief();

         break;

     #endif // CONSOLE_OUTPUT

      case '!':                           // perform a reset

         super::charEv( code );  
         scaling.reset();
         break;

      default:

         return super::charEv( code );    // pass unhandled events to super class
   }
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  ZapVox::dynamics
 *
 *  Desc:  Perform a dynamic update.
 *
 *  Memb:  flags.TRIG       - if set, a trigger is pending
 *         pendKey          - physical key # causing pending trigger
 *         scaling          - keybrd scaling control
 *
 *----------------------------------------------------------------------------*/      

void ZapVox::dynamics()
{
   if ( flags & TRIG )              // handle pending trigger
   {
      if ( scaling.muted() )
         zapOsc->unscaleSweep();
      else
      {
         /*
         console.infoByte( CONSTR("key"), pendKey );
         console.infoDouble( CONSTR("botScaler"), scaling.getBotScaler( pendKey ) );
         console.infoDouble( CONSTR("topScaler"), scaling.getTopScaler( pendKey ) );
         */
         zapOsc->scaleSweep( scaling.getBotScaler( pendKey ), 
                             scaling.getTopScaler( pendKey ) );
      }
   }
   super::dynamics();
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  ZapVox::noteOn
 *
 *  Desc:  Turn a note on.
 *
 *  Args:  note             - note to turn on  
 *
 *  Memb: +pendKey          - physical key # causing pending trigger
 *
 *----------------------------------------------------------------------------*/      

void ZapVox::noteOn( key note )
{
   pendKey = note.physkey();
   trigger();
}

class XPots : public VirtualPotDemux
{
   typedef Mode super;              // superclass is Mode

   public:

   PROMPT_STR( XPots ) 
};

class YPots : public VirtualPotDemux
{
   typedef Mode super;              // superclass is Mode

   public:

   PROMPT_STR( YPots ) 
};

/******************************************************************************
 *
 *                              ZapShotSynth 
 *                                                                            
 ******************************************************************************/


class FreqPan  : public PotSend { public: PROMPT_STR( FreqPan ) } ;
class DepthPan : public PotSend { public: PROMPT_STR( DepthPan ) } ;

class ZapShotSynth : public TwoVoxPanSynth
{
   typedef TwoVoxPanSynth super;    // superclass is TwoVoxPanSynth

   ZapVox       *zapVox[2];         // type-cast copies of vox[0..1]
   MasterZapOsc *masterOsc;         // type-cast copy of vox[0]->osc

   ByteMenu presetMenu;             // keybrd menu for selecting presets

   XPots xPots;                     // demux for x-group of virtual pots
   YPots yPots;                     // demux for y-group of virtual pots

   FreqPan  *freqPan;               // virtual pot for frequency of panning
   DepthPan *depthPan;              // virtual pot for depth of panning

   // the following bits are used for individual lines in a virtual pot output mask 

   enum     { _BEGIN_VPOT   = 0b00000001,  // vpot for beginning edge length
              _END_VPOT     = 0b00000010,  // vpot for ending edge length
              _PEAK_VPOT    = 0b00000100,  // vpot for peak amplitude
              _STRETCH_VPOT = 0b00001000,  // vpot for stretch factor
            };

   Bank macroBank;

   double delay;                       // inter-channel delay (in secs)
   word   delayUpds;                   // delay duration in # of updates
   word   delayDC;                     // downcounter to delay
   key    delayNote;                   // delayed note

   public:

   void config()
   {
      super::config();                 // allocates voices and oscillators

      depthPan = new DepthPan();
      freqPan  = new FreqPan();

      // assign slave oscillator and statusLED to master oscillator  

      masterOsc            = (MasterZapOsc *)zapVox[0]->zapOsc;
      masterOsc->slave     = zapVox[1]->zapOsc;
      masterOsc->statusLED = BLUE_LED; 

      presets.load( myPresets );      // load bank of presets
      macroBank.load( macros );       // load bank of macros
   }

   Osc *newOsc( byte ith )
   {
      zapVox[ith]->zapOsc = ( ith == 0 ) ? new MasterZapOsc() : new ZapOsc();
      return zapVox[ith]->zapOsc;
   }

   Voice *newVox( byte ith )
   {
      zapVox[ith] = new ZapVox();
      return zapVox[ith];
   }

   boolean charEv( char code )    
   {
      switch ( code )
      {
         case 'd':                  // set inter-channel delay

            if ( console.getDouble( CONSTR("delay"), &delay ) )
               setDelay( delay );
            break;

         case 't':

            noteOn( key(0) );
            break;

         case 'D':                  // push depthPan virtual pot

            console.pushMode( depthPan );
            break;

         case 'F':                  // push freqPan virtual pot

            console.pushMode( freqPan );
            break;

         case 'M':                  // choose a macro and run it for each voice
         {
            byte i;
            if ( console.getByte( CONSTR("macro#"), &i ) && macroBank.select(i) )
               forEach( (const char*)macroBank.dataPtr() );
            break;
         }
         case 'X':                  // map bank X of virtual pots

            console.pushMode( &xPots );
            break;

         case 'Y':                  // map bank Y of virtual pots

            console.pushMode( &yPots );
            break;

         #ifdef CONSOLE_OUTPUT

         case chrInfo:

            super::charEv( code );
            console.newlntab();
            console.infoDouble( CONSTR("delay"), delay );
            console.newlntab();
            freqPan->brief();
            depthPan->brief();
            console.newlntab();
            xPots.brief();
            console.newlntab();
            yPots.brief();
            break;

         #endif

         case '!':                        // perform a reset

            super::charEv( code );

            freqPan->reset();
            freqPan->connect( panControl, 0 );

            depthPan->reset();
            depthPan->connect( panControl, 1 );

            xPots.reset();
            xPots.connect( 0, &(zapVox[0]->zapOsc->vpBegin) );
            xPots.connect( 1, &(zapVox[1]->zapOsc->vpBegin) );
            xPots.connect( 2, &(zapVox[0]->zapOsc->vpPeak) );
            xPots.connect( 3, &(zapVox[1]->zapOsc->vpPeak) );
            xPots.execute( PSTR( "m11\\" ) );

            yPots.reset();
            yPots.connect( 0, &(zapVox[0]->zapOsc->vpEnd) );
            yPots.connect( 1, &(zapVox[1]->zapOsc->vpEnd) );
            yPots.connect( 2, &(zapVox[0]->zapOsc->vpStretch) );
            yPots.connect( 3, &(zapVox[1]->zapOsc->vpStretch) );
            yPots.execute( PSTR( "m11\\" ) );

            setDelay(0.0);
            delayDC = 0;

            break;

         default:

            return super::charEv( code );  
      }
   }

   void dynamics()                            // perform a dynamic update
   {
      if ( delayDC )
      {
         --delayDC;
         if ( delayDC == 0 )
            vox[1]->noteOn( delayNote );
      }
      super::dynamics();
   }

   bool evHandler( obEvent ev )              // handle an onboard event
   {
      switch ( ev.type() )
      {
         case POT0:  
         
            xPots.setPos( ev.getPotVal() );
            break;

         case POT1:  
      
            yPots.setPos( ev.getPotVal() );
            break;

         case BUT1_PRESS:                    // toggle auto-triggering
         {
            byte lastToToggle = masterOsc->latched ? 0 : 1 ;
            if ( masterOsc->autoTrigEnabled() )
            {
               for ( byte i = 0; i <= lastToToggle; i++ )
                  vox[i]->execute( PSTR("Oaf\\") );
            }
            else
            {
               for ( byte i = 0; i <= lastToToggle; i++ )
                  vox[i]->execute( PSTR("Oat\\") );
            }
            break;
         }

         case BUT1_DTAP:                     // override "one-shot menu" 

            presetMenu.waitKey();
            execute( (const char *)presets.dataPtr( presetMenu.value ) );
            break;

         default:
            return super::evHandler(ev);
      }
      return true;
   }


   void forEach( const char *macStr )
   {
      for ( byte i = 0; i < numVox; i++ )
         vox[i]->execute( macStr );
   }

   void noteOn( key note )                    // turn a note on
   {
      note.transpose( xpose ); 

      vox[0]->noteOn( note );

      if ( delayUpds )
      {
         delayDC   = delayUpds;
         delayNote = note;
      }
      else
      {
         if ( ! masterOsc->latched )
            vox[1]->noteOn( note );
      }
   }

   void setDelay( double secs )
   {
      const double maxDelay = 25.0;           // max delay

      if ( secs < 0.0 )       secs = 0.0;
      if ( secs > maxDelay )  secs = maxDelay;

      delay               = secs;
      double delayUpdates = secs * dynaRate;
      delayUpds           = (word )delayUpdates;

      //console.infoInt( CONSTR("updates"), delayUpds );
   }

   void welcome()
   {
      presets.select(0);                      // load Pong preset
   }

} mySynth ;

void setup()
{
   ardutouch_setup( &mySynth );               // initialize system resources
}

void loop()
{
   ardutouch_loop();                          // perform ongoing system tasks  
}                                             

