/*
    LFO.cpp

    Implementation of the LFO class.

    ---------------------------------------------------------------------------

    Copyright (C) 2016, Cornfield Electronics, Inc.

    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0
    Unported License.

    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/

    Created by Bill Alessi & Mitch Altman.

   ---------------------------------------------------------------------------
*/

#include "Console_.h"
#include "types.h"
#include "Audio.h"
#include "LFO.h"

#define DEF_DEPTH        32         // default depth of oscillation

#define MAX_LOW_FREQ   20.0         // maximum freq  of oscillation
#define MIN_LOW_FREQ    0.01        // minimum freq  of oscillation
#define DEF_LOW_FREQ    3.8         // default freq  of oscillation

#define LFO_QRT_IDX     0x4000 
#define LFO_MID_IDX     0x8000 
#define LFO_END_IDX     0xFFFF

#define LFO_POS_ONE     0x4000
#define LFO_NEG_ONE     ~LFO_POS_ONE

#define LFO_UNIT_FADE   0x8000 
#define LFO_FADE_PPS    8           // fade time denominated in this many parts per sec

#define LFO_WF_MASK     0b00000011  // mask for extracting waveform # from waveform byte

/******************************************************************************
 *
 *                                 LFO 
 *                                                                            
 ******************************************************************************/

/*----------------------------------------------------------------------------*
 *
 *  Name:  LFO::charEv
 *
 *  Desc:  Process a character event.
 *
 *  Args:  code             - character to process
 *
 *  Rets:  status           - if true, character was handled
 *
 *  Memb: +idx              - cur position within oscillation cycle
 *         waveform.SIGNED  - oscillator is signed 
 *
 *----------------------------------------------------------------------------*/
       
bool LFO::charEv( char key )
{
   switch ( key )
   {
      case lfoIniPos:                  // set initial oscillator position

         idx = (waveform&SIGNED) ? LFO_QRT_IDX : 0;
         break;

      #ifdef INTERN_CONSOLE

      case 'd':                        // set depth of oscillation
      {
         byte inpVal;
         if ( console.getByte( CONSTR("depth"), &inpVal ) )
            setDepth( inpVal );
         break;
      }

      case 'f':                        // set frequency of oscillation
      {
         double inpVal;            
         if ( console.getDouble( CONSTR("freq"), &inpVal ) )
            setFreq( inpVal );
         break;
      }

      case 's':                        // configure oscillator as signed

         setSigned( true );
         break;

      case 'u':                        // configure oscillator as unsigned

         setSigned( false );
         break;

      case 'F':                        // set oscillator to falling saw tooth

         setWaveform( LFO_FSAW_WF );
         break;
         
      case 'Q':                        // set oscillator to square wave

         setWaveform( LFO_SQ_WF );
         break;
         
      case 'R':                        // set oscillator to rising saw tooth

         setWaveform( LFO_RSAW_WF );
         break;
         
      case 'T':                        // set oscillator to triangle wave

         setWaveform( LFO_TRI_WF );
         break;
         
      #endif

      #ifdef CONSOLE_OUTPUT
      case chrInfo:                    // display object info to console

         super::charEv( chrInfo );
         console.print( isSigned() ? 's' : 'u');
         {
            char wfLetter;
            switch ( getWaveform() )
            {
               case LFO_TRI_WF:  wfLetter = 'T'; break;
               case LFO_SQ_WF:   wfLetter = 'Q'; break;
               case LFO_RSAW_WF: wfLetter = 'R'; break;
               case LFO_FSAW_WF: wfLetter = 'F'; break;
            }
            console.print( wfLetter );
         }
         console.space();
         console.infoDouble( CONSTR("freq"), freq );
         console.infoByte( CONSTR("depth"), getDepth() );
         break;

      #endif

      case '.':                        // mute

         super::charEv( key );
         revaluate();
         break;

      case '!':                        // reset

         super::charEv( key );
         setFreq( DEF_LOW_FREQ );
         setDepth( DEF_DEPTH );
         setWaveform( LFO_TRI_WF );
         setSigned( false );
         iniVal();
         break;

      default:

         return super::charEv( key );
   }
   return true;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  LFO::dynamics
 *
 *  Desc:  Perform a dynamic update by bumping the LFO position.
 *
 *  Memb: +idx              - current position in oscillator cycle
 *         step             - change in idx per dynamic update
 *
 *----------------------------------------------------------------------------*/
       
void LFO::dynamics()
{
   if ( ! muted() )
   {
      idx += step;                     // advance oscillator 
      evaluate();                      // set output value
   }
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  LFO::evaluate
 *
 *  Desc:  Compute output value based on oscillator position.
 *
 *  Memb:  idx              - current position in oscillator cycle
 *         instDepth        - instantaneous oscillation depth
 *        +value            - current output value
 *
 *----------------------------------------------------------------------------*/
       
void LFO::evaluate()
{
   word amp = (this->*computeAmp)();

   if ( waveform&SIGNED )
   {
      // scale amplitude by depth

      Long modAmp;                     // scaled amplitude 

      modAmp.val = (int )amp;
      modAmp.val *= instDepth;

      // convert scaled amplitude to floating pt output value

      value = modAmp.val * 0.000000476837158203125;
   }
   else  // oscillator is unsigned
   {
      // scale amplitude by depth

      DWord modAmp;                    // scaled amplitude 
     
      modAmp.val = 0;
      modAmp._.lsw._.msb = instDepth;
      modAmp.val *= amp;               // scaled amplitude in msw

      modAmp._.lsw.val = LFO_POS_ONE - modAmp._.msw.val;  

      // convert scaled amplitude to floating pt output value

      value = modAmp._.lsw.val * 0.00006103515625;
  }

}

/*----------------------------------------------------------------------------*
 *
 *  Name:  LFO:evHandler
 *
 *  Desc:  Handle an onboard event.
 *
 *  Args:  ev               - onboard event
 *
 *  Rets:  status           - true if the event was handled
 *
 *----------------------------------------------------------------------------*/      

boolean LFO::evHandler( obEvent ev )
{
   switch ( ev.type() )
   {
      case POT0:       
      {
         byte val    = ev.getPotVal();
         word expVal = val * val;
         setFreq( expVal * .000333 );      // logarithmic 0 - 21.65 Hz
         break;
      }
      case POT1:       

         setDepth( ev.getPotVal128() );
         break;

      default:

         return super::evHandler(ev);
   }
   return true;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  LFO::getDepth
 *
 *  Desc:  Get the oscillation depth.
 *
 *  Rets:  d                - oscillation depth (0-128)
 *
 *  Memb:  depth            - oscillation depth
 *
 *----------------------------------------------------------------------------*/

byte LFO::getDepth()
{
   return depth;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  LFO::getFreq
 *
 *  Desc:  Get the oscillation frequency.
 *
 *  Rets:  f                - oscillation frequency 
 *
 *  Memb:  freq             - oscillation frequency
 *
 *----------------------------------------------------------------------------*/

double LFO::getFreq()
{
   return freq;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  LFO::getWaveform
 *
 *  Desc:  Returns the oscillator waveform number.
 *
 *  Rets:  wf_num           - waveform # 
 *
 *  Memb:  waveform         - oscillator waveform
 *
 *----------------------------------------------------------------------------*/

byte LFO::getWaveform()
{
   return ( waveform & LFO_WF_MASK );
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  LFO::iniVal
 *
 *  Desc:  Set initial oscillator position and revaluate it.
 *
 *----------------------------------------------------------------------------*/
       
void LFO::iniVal()
{
   charEv( lfoIniPos );             // set initial oscillator position
   revaluate();
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  LFO::isSigned
 *
 *  Desc:  Returns true if oscillation range is signed.
 *
 *  Rets:  signed           - if true, oscillation range is signed
 *
 *  Memb:  waveform.SIGNED  - oscillator is signed 
 *
 *----------------------------------------------------------------------------*/

bool LFO::isSigned()
{
   return ( (waveform & SIGNED) == SIGNED );
}

#ifdef KEYBRD_MENUS

/*----------------------------------------------------------------------------*
 *
 *  Name:  LFO::menu
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
 *           |   |   |   |   |   |   |   |   | ' |   | . |   | 
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |    ___     ___    |    ___     ___     ___    | 
 *           |     |       |     |     |       |       |     |
 *           |     |       |     |     |       |       |     |
 *           |  f  |   d   |     |     |   ~   |   <   |  !  |
 *           |     |       |     |     |       |       |     |
 *           |     |       |     |     |       |       |     |
 *           -------------------------------------------------
 *
 *----------------------------------------------------------------------------*/      

char LFO::menu( key k )
{
   switch ( k.position() )
   {
      case  0: return 'f';                   // set oscillator frequency
      case  2: return 'd';                   // set oscillator depth
      default: return super::menu( k );
   }
}

#endif

/*----------------------------------------------------------------------------*
 *
 *  Name:  LFO::revaluate
 *
 *  Desc:  Evaluate the LFO, taking into consideration whether it is muted.
 *
 *----------------------------------------------------------------------------*/
       
void LFO::revaluate()
{
   if ( muted() )
      value = 1.0;
   else
      evaluate();                   
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  LFO::setAmpMethodPtr
 *
 *  Desc:  Set the computeAmp() pointer to the address of the appropriate 
 *         method for computing the unscaled amplitude of the oscillator
 *         at its current position.
 *
 *  Memb: +computeAmp       - ptr to method for computing unscaled amplitude
 *
 *  Note:  There are 2 different methods per waveform type, one for a
 *         signed range, and one for an unsigned range.
 *
 *----------------------------------------------------------------------------*/

   // individual methods per waveform type and (signed or unsigned) range:
       
   int LFO::_signTRI()
   {
      return ( idx < LFO_MID_IDX ) ? idx - LFO_POS_ONE : LFO_POS_ONE - (idx - LFO_MID_IDX);
   }

   int LFO::_unsignTRI()
   {
      return ( idx < LFO_MID_IDX ) ? idx : LFO_END_IDX - idx;
   }

   int LFO::_signSQ()
   {
      return ( idx < LFO_MID_IDX ) ? -LFO_POS_ONE : LFO_POS_ONE;
   }

   int LFO::_unsignSQ()
   {
      return ( idx < LFO_MID_IDX ) ? 0 : LFO_MID_IDX;
   }

   int LFO::_signRSAW()
   {
      return ( ( idx >> 1 ) - LFO_POS_ONE );
   }

   int LFO::_unsignRSAW()
   {
      return ( LFO_MID_IDX - ( idx >> 1 ) );
   }

   int LFO::_signFSAW()
   {
      return ( LFO_POS_ONE - (idx >> 1 ) );
   }

   int LFO::_unsignFSAW()
   {
      return ( idx >> 1 );
   }

void LFO::setAmpMethodPtr()
{
   if ( isSigned() )       // oscillator is signed

      switch ( getWaveform() )
      {
         case LFO_TRI_WF:  computeAmp = &LFO::_signTRI;    break;
         case LFO_SQ_WF:   computeAmp = &LFO::_signSQ;     break;
         case LFO_RSAW_WF: computeAmp = &LFO::_signRSAW;   break;
         case LFO_FSAW_WF: computeAmp = &LFO::_signFSAW;   break;
      }

   else                    // oscillator is unsigned

      switch ( getWaveform() )
      {
         case LFO_TRI_WF:  computeAmp = &LFO::_unsignTRI;  break;
         case LFO_SQ_WF:   computeAmp = &LFO::_unsignSQ;   break;
         case LFO_RSAW_WF: computeAmp = &LFO::_unsignRSAW; break;
         case LFO_FSAW_WF: computeAmp = &LFO::_unsignFSAW; break;
      }
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  LFO::setDepth
 *
 *  Desc:  Set the oscillation depth.
 *
 *  Args:  d                - oscillation depth (0-128)
 *
 *  Memb: +depth            - oscillation depth 
 *        +instDepth        - instantaneous oscillation depth 
 *
 *----------------------------------------------------------------------------*/

void LFO::setDepth( byte d )
{
   if ( d > 128 ) d = 128;
   instDepth = depth = d;
   if ( d == 0 )
      charEv( lfoIniPos );     
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  LFO::setFreq
 *
 *  Desc:  Set the oscillation frequency.
 *
 *  Args:  f                - oscillation frequency 
 *
 *  Memb: +freq             - oscillation frequency
 *
 *----------------------------------------------------------------------------*/

void LFO::setFreq( double f )
{
   #ifdef BOUNDCHECK_LFOS
      if ( f > MAX_LOW_FREQ )
         f = MAX_LOW_FREQ;
      else if ( f < MIN_LOW_FREQ )
         f = MIN_LOW_FREQ;
   #endif

   freq = f;
   step = pow(2,16) * freq / dynaRate;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  LFO::setSigned
 *
 *  Desc:  Set oscillation range as signed or unsigned.
 *
 *  Args:  signed           - if true, oscillation range is signed
 *
 *  Memb: +waveform.SIGNED  - set if oscillation range is signed 
 *
 *----------------------------------------------------------------------------*/

void LFO::setSigned( bool s )
{
   if ( s )
      waveform  |= SIGNED;
   else
      waveform &= ~SIGNED;

   setAmpMethodPtr();
   revaluate();
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  LFO::setWaveform
 *
 *  Desc:  Set the oscillator waveform.
 *
 *  Args:  wf_num           - waveform # 
 *
 *  Memb: +waveform         - oscillator waveform
 *
 *----------------------------------------------------------------------------*/

void LFO::setWaveform( byte wf_num )
{
   if ( wf_num > LFO_MAX_WF )
      return;

   waveform &= ~LFO_WF_MASK;
   waveform += wf_num;

   setAmpMethodPtr();
   revaluate();
}

/******************************************************************************
 *
 *                               FadeLFO 
 *
 *   An LFO which can be configured to fade in or out over a specified period
 *   of time once triggered.
 *
 ******************************************************************************/

/*----------------------------------------------------------------------------*
 *
 *  Name:  FadeLFO::charEv
 *
 *  Desc:  Process a character event.
 *
 *  Args:  code             - character to process
 *
 *  Glob:  dynaRate         - dynamic udpate rate
 *
 *  Memb: +flags.DONE       - LFO is done fading
 *        +flags.FADEOUT    - LFO fades out (if not set, LFO fades in) 
 *         flags.LEGATO     - trigger only if prior fade is completed
 *        +time             - fade time in 1/8th secs (0-255) 
 *
 *  Rets:  status           - if true, character was handled
 *
 *----------------------------------------------------------------------------*/
       
boolean FadeLFO::charEv( char code )
{
   switch ( code )
   {
      case chrTrigger:                 // trigger the LFO

         if ( time )                   // if trigger-sensitive
         {
            iniFader();
            iniVal();
            flags &= ~DONE;
         }
         break;

      case chrRelease:

         break;

      #ifdef CONSOLE_OUTPUT
      case chrInfo:                    // display object info to console

         super::charEv( chrInfo );
         console.infoByte( CONSTR("time"), time );
         console.space();
         console.print( ( flags & FADEOUT ) ? '-' : '+' );
         break;
      #endif

      #ifdef INTERN_CONSOLE

      case '~':                        // set legato triggering
      case '\'':                       // set stacato triggering

         super::charEv( code );
         flags |= DONE;                // set for immediate re-triggering
         break;

      case '+':                        // set fade direction to "in"

         fadeIn();
         break;

      case '-':                        // set fade direction to "out"

         fadeOut();
         break;

      case 't':                        // set fade time
      {
         byte inpVal;
         if ( console.getByte( CONSTR("time"), &inpVal ) )
            setFadeTime( inpVal );
         break;
      }       

      #endif

      case '!':                        // reset

         super::charEv( code );
         fadeIn();
         setFadeTime(0);
         break;

      default:

         return super::charEv( code );
   }
   return true;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  FadeLFO::dynamics
 *
 *  Desc:  Perform a dynamic update by bumping the LFO position, bumping the 
 *         fader value, and setting the DONE bit if the fade has completed.
 *
 *  Memb: +fader            - current fader value
 *         fadeStep         - increment fader this much per dynamic update
 *        +flags.DONE       - LFO is done fading
 *         flags.FADEOUT    - LFO fades out (if not set, LFO fades in) 
 *         flags.MUTE       - LFO is muted 
 *
 *----------------------------------------------------------------------------*/      

void FadeLFO::dynamics()
{
   if ( (flags&MUTE) )     // return if LFO is muted 
      return;
   
   super::dynamics();      // super class performs basic osc dynamics

   // update fader

   if ( (flags&DONE) )     // return if LFO is done fading
      return;

   fader += fadeStep;
   
   if ( fader > LFO_UNIT_FADE )
   {
      fader = (flags & FADEOUT) ? 0 : LFO_UNIT_FADE;
      flags |= DONE;
   }
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  FadeLFO::evaluate
 *
 *  Desc:  Compute output value based on oscillator & fader position.
 *
 *  Memb:  fader            - current fader value
 *         depth            - oscillation depth
 *        +instDepth        - instantaneous oscillation depth
 *
 *----------------------------------------------------------------------------*/
       
void FadeLFO::evaluate()
{
   // compute instantaneous depth = depth * fader

   DWord regDepth;

   regDepth.val = 0;
   regDepth._.lsw.val = depth;
   regDepth._.lsw.val <<= 1;
   regDepth.val *= fader;
   instDepth = regDepth._.msw._.lsb;

   super::evaluate();         // super class will compute output value
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  FadeLFO::fadeIn()
 *
 *  Desc:  Configure LFO to fade in,
 *
 *  Memb: +flags.FADEOUT    - LFO fades out (if not set, LFO fades in) 
 *        +fadeStep         - increment fader this much per dynamic upd
 *
 *----------------------------------------------------------------------------*/
       
void FadeLFO::fadeIn()
{
   flags   &= ~FADEOUT;
   fadeStep = abs(fadeStep);
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  FadeLFO::fadeOut()
 *
 *  Desc:  Configure LFO to fade out.
 *
 *  Memb: +flags.FADEOUT    - LFO fades out (if not set, LFO fades in) 
 *        +fadeStep         - increment fader this much per dynamic upd
 *
 *----------------------------------------------------------------------------*/
       
void FadeLFO::fadeOut()
{
   flags   |= FADEOUT;
   fadeStep = -abs(fadeStep);
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  FadeLFO::iniFader
 *
 *  Desc:  Set initial fader value.
 *
 *  Memb: +fader            - current fader value
 *         flags.FADEOUT    - LFO fades out (if not set, LFO fades in) 
 *         time             - fade time in 1/8th secs 
 *
 *----------------------------------------------------------------------------*/
       
void FadeLFO::iniFader()
{
   if ( time == 0 )
      fader = LFO_UNIT_FADE;
   else
      fader = (flags & FADEOUT) ? LFO_UNIT_FADE : 0;
}


#ifdef KEYBRD_MENUS

/*----------------------------------------------------------------------------*
 *
 *  Name:  FadeLFO::menu
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
 *           |   |   |   |   |   |   | + |   | ' |   | . |   | 
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |    ___     ___    |    ___     ___     ___    | 
 *           |     |       |     |     |       |       |     |
 *           |     |       |     |     |       |       |     |
 *           |  f  |   d   |  t  |  -  |   ~   |   <   |  !  |
 *           |     |       |     |     |       |       |     |
 *           |     |       |     |     |       |       |     |
 *           -------------------------------------------------
 *
 *----------------------------------------------------------------------------*/      

char FadeLFO::menu( key k )
{
   switch ( k.position() )
   {
      case  4: return 't';
      case  5: return '-';
      case  6: return '+';
      default: return super::menu( k );
   }
}

#endif

/*----------------------------------------------------------------------------*
 *
 *  Name:  FadeLFO::setFadeTime
 *
 *  Desc:  Set time of fade.
 *
 *  Memb: +fader            - current fader value
 *        +fadeStep         - increment fader this much per dynamic upd
 *         flags.FADEOUT    - LFO fades out (if not set, LFO fades in) 
 *         time             - fade time in 1/8th secs (0-255) 
 *
 *----------------------------------------------------------------------------*/
       
void FadeLFO::setFadeTime( byte t )
{
   #define LFO_UNIT_FADExPPS (unsigned long)LFO_UNIT_FADE*LFO_FADE_PPS

   time = t;
   if ( time > 0 )               // if fade enabled
   {
      DWord step;

      step.val  = LFO_UNIT_FADExPPS;

      //infoULongBits( CONSTR("Ux8"), step.val );
      step.val /= (time * dynaRate);
      fadeStep  = step._.lsw.val;
      if ( (flags&FADEOUT) )
         fadeStep = -fadeStep;
   }
   else
   { 
      fadeStep = 0;
      flags |= DONE;             // probably not necessary
   }
   iniFader();
}

/******************************************************************************
 *
 *                               TermLFO 
 *
 *   "Terminating LFO" - an LFO which can be configured to run for a specified 
 *   number of half-cycles once triggered.
 *
 ******************************************************************************/

/*----------------------------------------------------------------------------*
 *
 *  Name:  TermLFO::charEv
 *
 *  Desc:  Process a character event.
 *
 *  Args:  code             - character to process
 *
 *  Memb: +flags.DONE       - LFO is done traversing
 *         flags.LEGATO     - trigger only if prior traversal has completed
 *        +flags.PHASE      - determines start/end phase 
 *        +idx              - current position in oscillator cycle
 *        +runDC            - downcounter (in half-cycles) to end of traversal
 *        +trav             - traverse this many half-cycles (0 = run forever)
 *
 *  Rets:  status           - if true, character was handled
 *
 *----------------------------------------------------------------------------*/
       
boolean TermLFO::charEv( char code )
{
   switch ( code )
   {
      case chrTrigger:                 // trigger the LFO

         if ( trav )                   // on trigger-ready
         {
            runDC = trav;              // load running downcounter
            iniVal();                  // reset oscillator position
            flags &= ~DONE;            // "not done"
         }
         break;

      case lfoIniPos:                  // set initial oscillator position

         idx = 0;                      // default is to start at beginning of period
         
         // if any of the following compound conditons are true use default start index 

         if ( sawtooth() )
         {
            if ( isSigned() )  
            {
               if ( flags&PHASE ) break;
            }
            else 
            {
               if ( fullCycle() )  break;
            }
         }
         else // not a sawtooth
         {
            if ( ((flags&PHASE) && fullCycle()) || (!(flags&PHASE) && !fullCycle()) ) 
               break;
         }

         // else start at the mid point of the period

         idx = LFO_MID_IDX;            // start at half-cycle

         break;

      #ifdef INTERN_CONSOLE

      case 't':                        // set traversal count
      {
         byte inpVal;
         if ( console.getByte( CONSTR("trav"), &inpVal ) )
            setTravCount( inpVal );
         break;
      }

      case '+':                        // set phase flag
      
         flags |= PHASE;
         break;

      case '-':                        // unset phase flag

         flags &= ~PHASE;
         break;

      #endif

      #ifdef CONSOLE_OUTPUT
      case chrInfo:                    // display object info to console

         super::charEv( chrInfo );
         console.infoByte( CONSTR("trav"), trav );
         console.space();
         console.print( ( flags & PHASE ) ? '+' : '-' );
         break;
      #endif

      case '!':                        // reset

         super::charEv( code );
         setTravCount(0);              // sets trav = runDC = 0, ~DONE
         break;

      default:

         return super::charEv( code );
   }
   return true;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  TermLFO::dynamics
 *
 *  Desc:  Perform a dynamic update by bumping the LFO position, uodating the
 *         traversal downcounter, and setting the DONE bit if the traversal 
 *         has completed.
 *
 *  Memb: +flags.DONE       - LFO is done traversing
 *         flags.MUTE       - LFO is muted
 *        +idx              - current position in oscillator cycle
 *        +runDC            - downcounter (in half-cycles) to end of traversal
 *         step             - change in idx per dynamic update
 *         trav             - traverse this many half-cycles (0 = run forever)
 *
 *----------------------------------------------------------------------------*/
       
void TermLFO::dynamics()
{
   if ( flags&MUTE || (trav && flags&DONE) )  // return if LFO is muted or done
   {
      return;
   }

   word prior;     // high bit denotes which half-cycle idx was in prior to bump
   word after;     // high bit denotes which half-cycle idx is in after bump

   // bump oscillator while caching which half-cycle it was in prior to and 
   // after the bump

   prior = idx & LFO_HIGHBIT;
   idx  += step;                      
   after = idx & LFO_HIGHBIT;

   // if oscillator has crossed a half-cycle and in doing so has completed an
   // ongoing traversal, then adjust idx to the beginning of the half-cycle 
   // and flag LFO as done.

   if ( prior^after && ( runDC && --runDC == 0 ) )
   {
      idx = after ? LFO_MID_IDX : 0;
      flags |= DONE;  
      
      // adjust sawtooth indices to proper terminal values

      if ( ! isSigned() )
      {
         switch ( getWaveform() )
         {
            case LFO_RSAW_WF:

               idx = flags&PHASE ? LFO_END_IDX : 0 ;
               break;

            case LFO_FSAW_WF:

               idx = flags&PHASE ? 0 : LFO_END_IDX ;
               break;
         }
      }
   }

   evaluate();                         // set output value
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  TermLFO::fullCycle
 *
 *  Desc:  Return a boolean indicating whether the traversal count is a 
 *         multiple of a full waveform cycle.
 *
 *  Memb:  trav             - traversal count in half-cycles (0 = run forever)
 *
 *  Rets:  isFullCycle      - if true, character was handled
 *
 *----------------------------------------------------------------------------*/
       
bool TermLFO::fullCycle()
{
   return ! (trav & 1);
}

#ifdef KEYBRD_MENUS

/*----------------------------------------------------------------------------*
 *
 *  Name:  TermLFO::menu
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
 *           |   |   |   |   |   |   | + |   | ' |   | . |   | 
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |    ___     ___    |    ___     ___     ___    | 
 *           |     |       |     |     |       |       |     |
 *           |     |       |     |     |       |       |     |
 *           |  f  |   d   |  t  |  -  |   ~   |   <   |  !  |
 *           |     |       |     |     |       |       |     |
 *           |     |       |     |     |       |       |     |
 *           -------------------------------------------------
 *
 *----------------------------------------------------------------------------*/      

char TermLFO::menu( key k )
{
   switch ( k.position() )
   {
      case  4: return 't';
      case  5: return '-';
      case  6: return '+';
      default: return super::menu( k );
   }
}

#endif

/*----------------------------------------------------------------------------*
 *
 *  Name:  TermLFO::sawtooth
 *
 *  Desc:  Return a boolean indicating whether the LFO waveform is of a 
 *         sawtooth type.
 *
 *  Rets:  amSawTooth       - if true, character was handled
 *
 *----------------------------------------------------------------------------*/
       
bool TermLFO::sawtooth()
{
   return ( getWaveform() >= LFO_RSAW_WF );
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  TermLFO::setTravCount
 *
 *  Desc:  Set the traversal count
 *
 *  Args:  n                - new traversal count
 *
 *  Memb: +flags.DONE       - LFO is done traversing
 *        +runDC            - downcounter (in half-cycles) to end of traversal
 *        +trav             - traversal count (0 = run forever)
 *
 *  Rets:  status           - if true, character was handled
 *
 *----------------------------------------------------------------------------*/
       
void TermLFO::setTravCount( byte n )
{
   trav = n;
   if ( n == 0 )                 // if now free-running
   {
      flags &= ~DONE;            // insure "not done"
      runDC = 0;                 // insure "not traversing"
   }
}

