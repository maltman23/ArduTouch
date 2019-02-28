/*
    Voice.cpp  

    Implementation of the Voice and StockVoice classes.

    ---------------------------------------------------------------------------
 
    Copyright (C) 2016, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
   ---------------------------------------------------------------------------
*/

#include "System.h"
#include "Tuning.h"
#include "Voice.h"

/******************************************************************************
 *
 *                                Voice 
 *                                                                            
 ******************************************************************************/

/*----------------------------------------------------------------------------*
 *
 *  Name:  Voice::addAmpMod
 *
 *  Desc:  Add an amplitude modifier.
 *
 *  Args:  ampMod           - address of amplitude modifier to add
 *
 *  Memb:  ampMods          - applitude modifiers
 *
 *----------------------------------------------------------------------------*/      

void Voice::addAmpMod( Factor *ampMod )
{
   ampMods.add( ampMod );
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Voice::addEffect
 *
 *  Desc:  Add an effect.
 *
 *  Args:  effect           - address of effect to add
 *
 *  Memb:  effects          - effects loop
 *
 *----------------------------------------------------------------------------*/      

void Voice::addEffect( Effect *e )
{
   effects.add( e );
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Voice::addPitchMod
 *
 *  Desc:  Add a pitch modifier.
 *
 *  Args:  pitchMod         - address of pitch modifier to add
 *
 *  Memb:  pitchMods        - pitch modifiers
 *
 *----------------------------------------------------------------------------*/      

void Voice::addPitchMod( Factor *pitchMod )
{
   pitchMods.add( pitchMod );
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Voice::calcEffVol
 *
 *  Desc:  Calculate the effective volume.
 *
 *  Memb: +effVol           - effective volume level (9 bit)
 *         globVol          - global volume lebel    (8 bit)
 *         vol              - (local) volume level   (8 bit)
 *
 *  Note:  There are multiple parameters relating to the volume level.
 *         These can be of two types: 8 bit and 9 bit. Both types represent
 *         a number between 0.0 and 1.0. The 9 bit volume parameters (effVol, 
 *         instVol, and SegVol) are straightforward: a volume level of 1.0
 *         is represented by 256. The 8 bit parameters (vol and globVol) 
 *         attempt to (mostly) preserve 9 bits of precision in 8 through the 
 *         following rouse: a volume level of 1.0 is represented by 255. For 
 *         all other 8-bit values the volume level = 8-bit-value / 256.
 *
 *----------------------------------------------------------------------------*/      

void Voice::calcEffVol()
{
   if ( vol == 255 && globVol == 255 )
   {
      effVol.val = 256;
      return;
   }

   word v = ( vol     == 255 ? 256 : vol );
   word g = ( globVol == 255 ? 256 : globVol );

   Word e; e.val = v * g;

   effVol._.lsb = e._.msb;
   effVol._.msb = 0;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Voice::calcMultGlide
 *
 *  Desc:  Calculate the glide multiplier.
 *
 *  Memb:  dirGlide         - ongoing portamento direction:
 *                              > 0  pitch is increasing 
 *                              < 0  pitch is decreasing 
 *                              = 0  pitch is steady (portamento done)
 *         glide            - portamento speed 1:255 (0 = off)
 *         multGlide        - dynamic multiplier for the instantaneous glide 
 *
 *----------------------------------------------------------------------------*/      

void Voice::calcMultGlide()
{
   #define GLIDE_ROOT_MANTISSA .0001    // arbitrary value which yields a useful
                                        // range of portamento speeds (with the 
                                        // dynamic update rate at ~150 Hz)

   #define EXPONENTIAL_GLIDE     210    // glide speed increases exponentially
                                        // beginning at this value
   double multiple = glide;
   if ( glide > EXPONENTIAL_GLIDE )
   {
      word ramp = glide - EXPONENTIAL_GLIDE;
      multiple += ( ramp * ramp );
   }

   multGlide = 1.0 + (multiple * GLIDE_ROOT_MANTISSA);

   if ( dirGlide < 0 )
      multGlide = 1.0 / multGlide;

   #undef  GLIDE_ROOT_MANTISSA
   #undef  EXPONENTIAL_GLIDE
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Voice::charEv
 *
 *  Desc:  Process a character event.
 *
 *  Args:  code             - character to process
 *
 *  Memb:  ampMods          - amplitude modifiers
 *         effects          - effects chain
 *        +glide            - portamento speed 1:255 (0 = off)
 *         pitchMods        - pitch modifiers
 *        +vol              - volume level 
 *
 *  Rets:  status           - true if character was handled
 *
 *----------------------------------------------------------------------------*/      

boolean Voice::charEv( char code )    
{
   switch ( code )
   {
      #ifdef INTERN_CONSOLE         // compile cases needed by macros

      case 'A':                     // push amplitude modifiers

         console.pushMode( &this->ampMods );
         break;

      case 'E':                     // push effects chain

         console.pushMode( &this->effects );
         break;

      case 'g':                     // input a portamento speed

         console.getByte( CONSTR("glide"), &this->glide );
         setGlide( glide );
         break;

      case 'O':                     // push oscillator

         console.pushMode( osc );
         break;

      case 'P':                     // push pitch modifiers

         console.pushMode( &this->pitchMods );
         break;

      #endif

      #ifdef CONSOLE_OUTPUT

      case chrInfo:                 // display object info to console

         super::charEv( chrInfo );
         console.newlntab();
         osc->charEv( chrInLnfo );
         console.infoByte( CONSTR("glide"), glide );
         console.newlntab();
         pitchMods.brief();            
         console.newlntab();
         effects.brief();
         console.newlntab();
         ampMods.brief();            
         break;

      #endif

      case '!':                     // perform a reset

         setGlide( 0 );             // no portamento

         osc->reset();              // reset oscillator
         ampMods.reset();           // reset amplitude modifiers
         pitchMods.reset();         // reset pitch modifiers
         effects.reset();           // reset effects

         flags &= ~(FREQ|TRIG);     // clear any pending freq change and trigger

      #ifdef IMPLICIT_SEQUENCER

      case sqncPLAYON:              // squelch any ongoing output
      case sqncPLAYOFF:        
      
      #endif     

         setFreq(0.0);

         // fall thru to super 
            
      default:

          if ( super::charEv( code ) )
            return true;
          else
            return osc->charEv( code );    // pass unhandled events to osc
   }
   return true;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Voice::doneGlide
 *
 *  Desc:  Set the any ongoing portamento to completed.
 *
 *  Args:  -- none --
 *
 *  Memb:  +dirGlide        - ongoing portamento direction:
 *                              > 0  pitch is increasing 
 *                              < 0  pitch is decreasing 
 *                              = 0  pitch is steady (portamento done)
 *         +instGlide       - instantaneous portamento coeffficient
 *
 *----------------------------------------------------------------------------*/      

void Voice::doneGlide()
{
   instGlide = 1.0;              // "no portamento modification of freq"
   dirGlide  = 0;                // "portamento completed"
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Voice::dynamics
 *
 *  Desc:  Handle any pending trigger and note. Perform a dynamic update of
 *         built-in controls, advance any ongoing portamento, and propagate 
 *         changes to the instantaneous volume and frequency of the voice.
 *
 *  Memb:  ampMods          - amplitude modifiers
 *        +dirGlide         - ongoing portamento direction:
 *                              > 0  pitch is increasing 
 *                              < 0  pitch is decreasing 
 *                              = 0  pitch is steady (portamento done)
 *         effects          - effects chain
 *         effVol           - effective volume 
 *        +flags.FREQ       - if set, an ideal frequency change is pending
 *        +flags.TRIG       - if set, a trigger is pending
 *         glide            - portamento speed 1:255 (0 = off)
 *        +instGlide        - instantaneous portamento coefficient
 *         multGlide        - dynamic multiplier for the instantaneous glide 
 *         osc              - ptr to resident oscillator
 *         pendFreq         - pending ideal frequency  
 *         pitchMods        - pitch modifiers
 *        +segVol           - if segue in process, ultimate instVol
 *         vol              - volume level 
 *
 *----------------------------------------------------------------------------*/      

void Voice::dynamics()
{
   super::dynamics();

   if ( flags & FREQ )              // handle pending frequency change
   {
      if ( glide )                  // recompute any ongoing portamento
      {
         double instFreq = osc->getFreq() * instGlide;
         if ( pendFreq != instFreq )
         {
            dirGlide  = ( pendFreq > instFreq ) ? 1 : -1;
            instGlide = instFreq / pendFreq;
            if ( instGlide )
               calcMultGlide();
            else
               doneGlide();
         }
         else
            doneGlide();
      }
      osc->setFreq( pendFreq );
      flags &= ~FREQ;               
   }

   if ( flags & TRIG )              // handle pending trigger
   {
      osc->trigger();               
      ampMods.trigger();
      pitchMods.trigger();
      effects.trigger();
      flags &= ~TRIG;               
   }
   else                             // update components
   {
      osc->dynamics();
      ampMods.dynamics();
      pitchMods.dynamics();
      effects.dynamics();
   }

   /* manage instantaneous volume */

   if ( muted() )
   {
      segVol.val = 0;
   }
   else
   {
      segVol.val = effVol.val;
      segVol.val *= ampMods.value();
   }

   /* manage instantaneous frequency */

   if ( dirGlide )                  // if portamento is ongoing ...
   {
      instGlide *= multGlide;       //    update instantaneous glide
      if ( dirGlide > 0 )           //    if pitch is increasing
      {
         if ( instGlide > 1.0 )     //      if portamento has overshot 
            doneGlide();
      }
      else                          //    pitch is decreasing
      {
         if ( instGlide < 1.0 )     //      if portamento has overshot
            doneGlide();
      }
   }
   osc->modFreq( instGlide * pitchMods.value() );
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Voice::evHandler
 *
 *  Desc:  Handle an onboard event.
 *
 *  Args:  ev               - onboard event
 *
 *  Rets:  status           - true if the event was handled
 *
 *----------------------------------------------------------------------------*/      

boolean Voice::evHandler( obEvent ev )
{
   if ( byte param = getScrollParm( ev ) )
   {
      byte val = ev.getPotVal();
      switch ( param )
      {
         case 1:  setVol( val );             break;      
         case 2:  osc->setDetune( val-128 ); break;      
         case 3:  setGlide( val );           break;
      }
   }
   else
   {
      switch ( ev.type() )
      {
         case BUT0_PRESS:

            scrollUp();
            break;

         case BUT1_PRESS:

            scrollDn();
            break;

         default:

            return super::evHandler( ev );
      }
   }

   return true;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Voice::noteOn
 *
 *  Desc:  Turn a note on.
 *
 *  Args:  note             - note to turn on  
 *
 *  Memb:  xpose            - transpose notes by this many intervals
 *
 *----------------------------------------------------------------------------*/      

void Voice::noteOn( key note )
{
   note.transpose( xpose ); 
   setFreq( masterTuning->pitch( note ) );
   trigger();
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Voice::noteOff
 *
 *  Desc:  Turn a note off.
 *
 *  Args:  note             - note to turn off  
 *
 *----------------------------------------------------------------------------*/      

void Voice::noteOff( key note )
{
   release();
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Voice::output
 *
 *  Desc:  Write output to an audio buffer.
 *
 *  Args:  buf              - ptr to audio buffer  
 *
 *  Glob:  audioBufSz       - size of system audio buffers
 *
 *  Memb:  effects          - effects chain
 *        +instVol          - instantaneous volume
 *         osc              - ptr to resident oscillator
 *         segVol           - if segue in process, ultimate instVol
 *
 *  Note:  if a segue is in progress, this method will gradually adjust the 
 *         instantaneous volume until it equals the segue volume.
 *
 *----------------------------------------------------------------------------*/      

void Voice::output( char *buf )        
{
   byte numrecs = audioBufSz;     

   osc->output( buf );
   effects.process( buf );

   Int amp;
   Int diffVol;

   diffVol.val = segVol.val - instVol.val;

   #define MAX_SEGUE 2

   if ( diffVol.val > 0 )
   {
      while ( segVol.val > instVol.val )
      {
         instVol.val += MAX_SEGUE;

         if ( instVol.val > segVol.val )
            instVol.val = segVol.val;

         amp.val  = instVol.val;
         amp.val *= *buf;
         *buf++   = amp._.msb;

         if ( --numrecs == 0)
            break;
      }
   }
   else if ( diffVol.val < 0 )
   {
      while ( segVol.val < instVol.val )
      {
         instVol.val -= MAX_SEGUE;

         if ( instVol.val < segVol.val )
            instVol.val = segVol.val;

         amp.val  = instVol.val;
         amp.val *= *buf;
         *buf++   = amp._.msb;

         if ( --numrecs == 0 )
            break;
      }
   }

   while ( numrecs-- ) 
   {
      amp.val  = instVol.val;
      amp.val *= *buf;
      *buf++   = amp._.msb;
   }
}

#ifdef KEYBRD_MENUS

/*----------------------------------------------------------------------------*
 *
 *  Name:  Voice::menu
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
 *           |   | A |   | P |   |   |   |   | k |   | . |   | 
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |    ___     ___    |    ___     ___     ___    | 
 *           |     |       |     |     |       |       |     |
 *           |     |       |     |     |       |       |     |
 *           |  E  |       |  e  |     |   x   |   <   |  !  |
 *           |     |       |     |     |       |       |     |
 *           |     |       |     |     |       |       |     |
 *           -------------------------------------------------
 *
 *----------------------------------------------------------------------------*/      

char Voice::menu( key k )
{
   switch ( k.position() )
   {
      case  0: return 'E';
      case  1: return 'A';
      case  3: return 'P';
      case  4: return 'e';
      default: 
      {
         // pass key to the menu() of the superclass. 
         // if this returns a non-null character, return it.
         // otherwise, return whatever the oscillator's menu() returns.

         if ( char superChar = super::menu(k) )  
            return superChar;
         else
            return osc->menu(k);
      }
   }
}
#endif

#ifdef CONSOLE_OUTPUT

const char *Voice::prompt()                   
{
   static const char ids[] PROGMEM = {"v0\0v1\0v2\0v3\0v4\0v5\0v6\0v7\0"};
   return &ids[num*3];
}

#endif

/*----------------------------------------------------------------------------*
 *
 *  Name:  Voice::release
 *
 *  Desc:  Release voice components.
 *
 *----------------------------------------------------------------------------*/      

void Voice::release()
{
   osc->release();               
   ampMods.release();
   effects.release();
   pitchMods.release();
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Voice::setFreq
 *
 *  Desc:  Set the (ideal) frequency.
 *
 *  Args:  freq             - new frequency
 *
 *  Memb: +flags.FREQ       - if set, a frequency change is pending
 *        +pendFreq         - pending frequency  
 *
 *----------------------------------------------------------------------------*/      

void Voice::setFreq( double newFreq )
{
   pendFreq = newFreq;
   flags |= FREQ;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Voice::setGlide
 *
 *  Desc:  Set the portamento speed.
 *
 *  Args:  speed            - portamento speed
 *
 *  Memb: +glide            - portamento speed 1:255 (0 = off)
 *
 *----------------------------------------------------------------------------*/      

void Voice::setGlide( byte g )
{
   glide = g;
   if ( g )
      calcMultGlide();
   else
      doneGlide();
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Voice::setGlobVol
 *
 *  Desc:  Set the global volume level.
 *
 *  Args:  vol              - global volume level
 *
 *  Memb: +globVol          - global volume level (0:255)
 *
 *----------------------------------------------------------------------------*/      

void Voice::setGlobVol( byte x )
{
   globVol = x;
   calcEffVol();
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Voice::setVol
 *
 *  Desc:  Set the (local) volume level.
 *
 *  Args:  vol              - volume level
 *
 *----------------------------------------------------------------------------*/      

void Voice::setVol( byte x )
{
   super::setVol(x);
   calcEffVol();
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Voice::trigger
 *
 *  Desc:  Trigger voice components.
 *
 *  Memb: +flags.TRIG       - if set, a trigger is pending
 *
 *  Note:  This routine sets a flag. The actual triggering is defered until
 *         the next dynamic update.
 *
 *----------------------------------------------------------------------------*/      

void Voice::trigger()
{
   flags |= TRIG;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Voice::useOsc
 *
 *  Desc:  Specify an oscillator to use.
 *
 *  Args:  o               - address of oscillator to use
 *
 *  Memb: +osc             - ptr to voice's oscillator
 *
 *----------------------------------------------------------------------------*/      

void Voice::useOsc( Osc *o )
{
   osc = o;
}

/******************************************************************************
 *
 *                                ADSRVoice 
 *                                                                            
 ******************************************************************************/

/*----------------------------------------------------------------------------*
 *
 *  Name:  ADSRVoice::charEv
 *
 *  Desc:  Process a character event.
 *
 *  Args:  code             - character to process
 *
 *  Memb:  envAmp           - built-in ADSR amplitude envelope
 *
 *  Rets:  status           - true if character was handled
 *
 *----------------------------------------------------------------------------*/      

boolean ADSRVoice::charEv( char code )    
{
   switch ( code )
   {
      #ifdef INTERN_CONSOLE         // compile cases needed by macros

      case 'e':                     // push amplitude envelope control

         console.pushMode( &this->envAmp );
         break;

      #endif

      default:

         super::charEv( code );
   }
   return true;
}

/******************************************************************************
 *
 *                               StockVoice 
 *                                                                            
 ******************************************************************************/

/*----------------------------------------------------------------------------*
 *
 *  Name:  StockVoice::charEv
 *
 *  Desc:  Process a character event.
 *
 *  Args:  code             - character to process
 *
 *  Memb:  vibrato          - built-in vibrato effect
 *
 *  Rets:  status           - true if character was handled
 *
 *----------------------------------------------------------------------------*/      

boolean StockVoice::charEv( char code )    
{
   switch ( code )
   {
      #ifdef INTERN_CONSOLE         // compile cases needed by macros

      case 'V':                     // push vibrato control

         console.pushMode( &this->vibrato );
         break;

      #endif

      default:

         super::charEv( code );
   }
   return true;
}



