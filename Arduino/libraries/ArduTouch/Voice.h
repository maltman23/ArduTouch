/*
    Voice.h  

    Declaration of the Voice, and auxillary classes.

    ---------------------------------------------------------------------------
 
    Copyright (C) 2016, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
   ---------------------------------------------------------------------------
*/

#ifndef VOICE_H_INCLUDED
#define VOICE_H_INCLUDED

#include "types.h"
#include "Console_.h"
#include "Instrument.h"
#include "Osc.h"
#include "Control.h"
#include "Envelope.h"
#include "Scroller.h"
#include "Vibrato.h"

/******************************************************************************
 *
 *                                AmpMods
 *
 ******************************************************************************/

class AmpMods : public Factors      // chain of amplitude modifiers
{
   PROMPT_STR( Amp ) 
} ;

/******************************************************************************
 *
 *                               PitchMods
 *
 ******************************************************************************/

class PitchMods : public Factors    // chain of pitch modifiers
{
   PROMPT_STR( Pitch ) 
} ;


/******************************************************************************
 *
 *                                Voice 
 *                                                                            
 ******************************************************************************/

class Voice : public Instrument, public Scroller
{
   typedef Instrument super;        // superclass is Instrument

   public:

   enum {  // use these bits in the flags byte: 

           FREQ   = _RESERVE2,      // a freq change is pending
           TRIG   = _RESERVE3       // a trigger is pending

        } ;

   byte   num;                      // voice # 

   Osc    *osc;                     // ptr to resident oscillator

   AmpMods     ampMods;             // amplitude modifiers
   PitchMods   pitchMods;           // pitch modifiers
   Effects     effects;             // effects chain 

   Voice()
   {
      setScrollable(3);             // scrollable pots: vol, detune, glide
      osc = NULL;
      num = 0;
   }

   void    addAmpMod( Factor* );    // add an amplitude modifier  
   void    addEffect( Effect* );    // add an effect 
   void    addPitchMod( Factor* );  // add a pitch modifier  
   boolean charEv( char );          // process a character event
   void    dynamics();              // update dynamics
   boolean evHandler( obEvent );    // handle an onboard event
   void    noteOn( key );           // turn a note on
   void    noteOff( key );          // turn a note off
   void    output( char * );        // write output to an audio buffer
   void    release();               // release components (on KeyUp)
   void    setFreq( double );       // set the ideal playback frequency
   void    setGlide( byte );        // set portamento speed
   void    setGlobVol( byte );      // set the global volume level
   void    setVol( byte );          // set the volume level
   void    trigger();               // trigger components
   void    useOsc( Osc* );          // specify oscillator to use

   #ifdef KEYBRD_MENUS
   char    menu( key );             // given a key, return a character 
   #endif

   #ifdef CONSOLE_OUTPUT
   const char *prompt();            // return object's prompt string
   #endif

   protected:

   byte    globVol;                 // global volume level
   Word    effVol;                  // effective volume (msb: integral part,
                                    //                   lsb: fractional part)
   Int     instVol;                 // instantaneous volume (msb: integral part,
                                    //                       lsb: fractional part)
   Int     segVol;                  // if segue in process, ultimate instVol 

   double  pendFreq;                // set this frequency at next dynamic update

   private:

   byte    glide;                   // portamento speed (0 = off)
   char    dirGlide;                // portamento direction +/-/0
   double  instGlide;               // instantaneous portamento coefficient
   double  multGlide;               // instGlide *= multGlide each dynamic update

   void    calcEffVol();            // calculate the effective volume level
   void    calcMultGlide();         // calculate dynamic glide multiplier
   void    doneGlide();             // set real-time glide params to "done"

} ;

/******************************************************************************
 *
 *                               ADSRVoice 
 *                                                                            
 ******************************************************************************/

class ADSRVoice : public Voice      // a Voice with built-in ADSR envelope
{
   typedef Voice super;             // superclass is Voice

   public:

   ADSR    envAmp;                  // built-in ADSR amplitude envelope

   ADSRVoice()
   {
      addAmpMod( &this->envAmp );
   }

   boolean charEv( char );          // process a character event

} ;

/******************************************************************************
 *
 *                               StockVoice 
 *                                                                            
 ******************************************************************************/

class StockVoice : public ADSRVoice // a Voice with built-in ADSR and vibrato
{
   typedef ADSRVoice super;         // superclass is ADSRVoice

   public:

   Vibrato vibrato;                 // built-in vibrato effect

   StockVoice()
   {
      addPitchMod( &this->vibrato );
   }

   boolean charEv( char );          // process a character event

} ;

#endif   // ifndef VOICE_H_INCLUDED
