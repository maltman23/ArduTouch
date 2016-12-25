/*
    Voice.cpp  

    Implementation of the Voice, Tremolo and Vibrato classes.

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

Voice::Voice()
{
   setScrollable(3);
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Voice::addEffect
 *
 *  Desc:  Add an effect to the effects loop.
 *
 *  Args:  effect           - address of effect to add
 *
 *  Memb:  effects          - effects loop
 *
 *  Note:  The effect is appended to the end of the effects loop.
 *
 *----------------------------------------------------------------------------*/      

void Voice::addEffect( Effect *e )
{
   effects.append( e );
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
 *  Memb: +glide            - portamento speed 1:255 (0 = off)
 *        +vol              - volume level 
 *
 *----------------------------------------------------------------------------*/      

boolean Voice::charEv( char code )    
{
   switch ( code )
   {
      #ifdef INTERN_CONSOLE

      case 'e':                     // push effects loop

         console.pushMode( &this->effects );
         break;

      case 'E':                     // push envelope control

         console.pushMode( &this->envAmp );
         break;

      case 'g':                     // input a portamento speed

         console.getByte( CONSTR("glide"), &this->glide );
         setGlide( glide );
         break;

      case 'O':                     // push oscillator

         console.pushMode( osc );
         break;

      case 'T':                     // push tremolo control

         console.pushMode( &this->tremolo );
         break;

      case 'V':                     // push vibrato control

         console.pushMode( &this->vibrato );
         break;

      #endif

      #ifdef CONSOLE_OUTPUT
      case chrInfo:                 // display object info to console

         MonoPhonic::charEv( chrInfo );
         osc->charEv( chrInLnfo );
         console.infoByte( CONSTR("glide"), glide );
         console.newlntab();
         envAmp.brief();            
         tremolo.brief();           
         vibrato.brief();           
         console.newlntab();
         effects.brief();
         break;
      #endif

      case '!':                     // perform a reset

         osc->reset();              // reset oscillator

         setGlide( 0 );             // no portamento
         envAmp.reset();            // reset envelope
         envAmp.setMute( false );   // enable envelope by default
         tremolo.reset();           // reset tremolo
         vibrato.reset();           // reset vibrato 
         effects.charEv( code );    // reset effects

         setGlobVol( mute_at_reset ? 0 : 255 );  

         // fall thru to MonoPhonic reset
            
      default:

          if ( MonoPhonic::charEv( code ) )
            return true;
          else
            return osc->charEv( code );    // pass unhandled events to osc
   }
   return true;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Voice::clearEffects
 *
 *  Desc:  Remove all effects from the effects loop.
 *
 *  Memb:  effects          - effects loop
 *
 *----------------------------------------------------------------------------*/      

void Voice::clearEffects()
{
   effects.clear();
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
 *  Memb:  dirGlide         - ongoing portamento direction:
 *                              > 0  pitch is increasing 
 *                              < 0  pitch is decreasing 
 *                              = 0  pitch is steady (portamento done)
 *         effVol           - effective volume 
 *        +instGlide        - instantaneous portamento coefficient
 *        +instVol          - instantaneous volume
 *         multGlide        - dynamic multiplier for the instantaneous glide 
 *        +segVol           - if segue in process, ultimate instVol
 *         vol              - volume level 
 *
 *----------------------------------------------------------------------------*/      

void Voice::dynamics()
{
   // update component dynamics

   osc->dynamics();
   envAmp.dynamics();
   tremolo.dynamics();
   vibrato.dynamics();
   effects.dynamics();

   /* manage instantaneous volume */

   if ( amMute() )
   {
      segVol.val = 0;
   }
   else
   {
      segVol.val = effVol.val;
      segVol.val *= envAmp.level * tremolo.val;
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

   osc->modFreq( instGlide * vibrato.val );
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
     return true;
   }

   switch ( ev.type() )
   {
      case KEY_DOWN:

         noteOn( ev.getKey() );
         return true;

      case KEY_UP:

         noteOff();
         return true;

      case BUT0_PRESS:

         scrollUp();
         return true;

      case BUT1_PRESS:

         scrollDn();
         return true;

      default:

         return MonoPhonic::evHandler( ev );
   }
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Voice::noteOn
 *
 *  Desc:  Initiate the playing of a note
 *
 *  Args:  note             - key of note to be played  
 *
 *----------------------------------------------------------------------------*/      

void Voice::noteOn( key note )
{
   setFreq( masterTuning->pitch( note ) );
   trigger();
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Voice::noteOff
 *
 *  Desc:  Release playing the current note
 *
 *----------------------------------------------------------------------------*/      

void Voice::noteOff()
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
 *  Memb: +instVol          - instantaneous volume
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
char Voice::menu( key k )
{
   switch ( k.position() )
   {
      case  0: return 'E';
      case  1: return 'V';
      case  3: return 'T';
      case  4: return 'e';
      default: 
      {
         if ( char parent = MonoPhonic::menu( k ) )
            return parent;
         else
            return osc->menu(k);
      }
   }
}
#endif

#ifdef CONSOLE_OUTPUT
const char *Voice::prompt()                     
{
   return CONSTR("voice");
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
   envAmp.release();
   effects.release();
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Voice::setFreq
 *
 *  Desc:  Set the (ideal) frequency.
 *
 *  Args:  freq             - new frequency
 *
 *  Memb: +dirGlide         - ongoing portamento direction:
 *                              > 0  pitch is increasing 
 *                              < 0  pitch is decreasing 
 *                              = 0  pitch is steady (portamento done)
 *         glide            - portamento speed 1:255 (0 = off)
 *        +instGlide        - instantaneous portamento coefficient
 *
 *----------------------------------------------------------------------------*/      

void Voice::setFreq( double newFreq )
{
   if ( glide )         // recompute any ongoing portamento
   {
      double instFreq = osc->getFreq() * instGlide;

      if ( newFreq != instFreq )
      {
         dirGlide  = ( newFreq > instFreq ) ? 1 : -1;
         instGlide = instFreq / newFreq;
         if ( instGlide )
            calcMultGlide();
         else
            doneGlide();
      }
      else
      {
         doneGlide();
      }
   }

   osc->setFreq( newFreq );
   osc->modFreq( instGlide * vibrato.val );
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
 *  Memb: +vol              - volume level (0:255)
 *
 *----------------------------------------------------------------------------*/      

void Voice::setVol( byte x )
{
   MonoPhonic::setVol(x);
   calcEffVol();
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Voice::trigger
 *
 *  Desc:  Trigger voice components.
 *
 *----------------------------------------------------------------------------*/      

void Voice::trigger()
{
   osc->trigger();               
   envAmp.trigger();
   tremolo.trigger();
   vibrato.trigger();
   effects.trigger();
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
 *                               Tremolo 
 *                                                                            
 ******************************************************************************/

/*----------------------------------------------------------------------------*
 *
 *  Name:  Tremolo::charEv
 *
 *  Desc:  Process a character event.
 *
 *  Args:  code             - character to process
 *
 *  Memb:  +val              - current output value
 *
 *  Rets:  status           - if true, character was handled
 *
 *----------------------------------------------------------------------------*/

boolean Tremolo::charEv( char code )
{
   switch ( code )
   {
      case '.':                        // mute

         TermLFO::charEv('.');
         val = 1.0;
         break;

      case '!':                        // reset

        iniOsc( .4, 2.5 );  
        // fall thru

      default:

         return TermLFO::charEv( code );
   }
   return true;
}

#ifdef CONSOLE_OUTPUT
const char* Tremolo::prompt()
{
   return CONSTR("tremolo");
}
#endif


/******************************************************************************
 *
 *                               Vibrato 
 *                                                                            
 ******************************************************************************/

#define RATIO_SEMITONE   1.059463   // frequency ratio between adjacent pitches
#define INVERT_SEMITONE   .943874   // 1 / RATIO_SEMITONE

/*----------------------------------------------------------------------------*
 *
 *  Name:  Vibrato::evaluate
 *
 *  Desc:  Compute output value based on oscillator position.
 *
 *  Memb:  depth            - oscillation depth (0.0 - 1.0)
 *        +fader            - current fader value
 *         pos              - current position within oscillation range
 *        +val              - current output value
 *
 *----------------------------------------------------------------------------*/

void Vibrato::evaluate()
{
   double spos = fader * ((2.0 * pos) - depth);
   if ( spos >= 0 )
      val = 1.0 + (spos * (RATIO_SEMITONE-1.0) );
   else
      val = 1.0 + (spos * (1.0 - INVERT_SEMITONE));
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Vibrato::iniPos
 *
 *  Desc:  Set initial oscillator position.
 *
 *  Memb:  depth            - oscillation depth (0.0 - 1.0)
 *        +pos              - cur position within oscillation range
 *
 *----------------------------------------------------------------------------*/
       
void Vibrato::iniPos()
{
   pos = depth * 0.5;
}

#ifdef CONSOLE_OUTPUT
const char* Vibrato::prompt()
{
   return CONSTR("vibrato");
}
#endif
