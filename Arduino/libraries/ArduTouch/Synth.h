/*
    Synth.h  

    Declaration of various Synth base classes.

    ---------------------------------------------------------------------------
 
    Copyright (C) 2016, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
   ---------------------------------------------------------------------------
*/

#ifndef SYNTH_H_INCLUDED
#define SYNTH_H_INCLUDED

#include "Console_.h"
#include "Instrument.h"
#include "Voice.h"
#include "Bank.h"
#include "Pan.h"

#define define_preset(x, y)                                                \
                                                                           \
   const char presetName##x[] PROGMEM = { #x };                            \
   const char presetData##x[] PROGMEM = { y };

#define extern_preset(x)                                                   \
                                                                           \
   extern const char presetName##x[];                                      \
   extern const char presetData##x[]

#define _preset(x) _member( presetData##x, presetName##x )

class PresetBank : public MacroBank
{
   public:

   #ifdef CONSOLE_OUTPUT
      const char *prompt() { return CONSTR("preset"); }
   #endif

} ;

extern PresetBank presets;             // bank of synth presets

/******************************************************************************
 *
 *                                Synth 
 *                                                                            
 ******************************************************************************/

class Synth : public StereoInstrument  // basic synthesizer class
{
   typedef StereoInstrument super;     // superclass is StereoInstrument

   protected:

   boolean preset_loading;             // a preset is in the process of loading
   boolean quiet_reset;                // preset has performed a quiet reset 

   public:

   virtual void config() {};           // executed at system startup
   virtual void welcome() {};          // perform post-reset startup tasks

   boolean charEv( char );             // process a character event
   void    runPreset( const char * );  // run a preset 

   #ifdef KEYBRD_MENUS
   char    menu( key );                // given a key, return a character 
   #endif

   PROMPT_STR( synth ) 

} ;


/******************************************************************************
 *
 *                                 VoxSynth 
 *
 ******************************************************************************/

class VoxSynth : public Synth          // synthesizer with voices   
{
   typedef Synth super;                // superclass is Synth

   protected:

   static const byte MaxVox = 8;       // max allowable number of voices

   byte  numVox;                       // number of voices
   Voice *vox[ MaxVox ];               // array of voices

   public:

   VoxSynth()
   {
      numVox = 0;
   }

   virtual Osc   *newOsc( byte nth );  // create oscillator for nth voice
   virtual Voice *newVox( byte nth );  // create nth voice 

   bool charEv( char );                // process a character event 
   void configVoices( byte numVox );   // sets up vox[] via newVox()/newOsc()
   void dynamics();                    // perform a dynamic update
   void noteOn( key );                 // turn a note on
   void noteOff( key );                // turn a note off
   void setVoicing( char* v );         // set transposition interval per voice
   void setVol( byte );                // set volume level of synth

   #ifdef KEYBRD_MENUS
   char    menu( key );                // given a key, return a character 
   #endif

} ;                                     

/******************************************************************************
 *
 *                                OneVoxSynth 
 *
 ******************************************************************************/

class OneVoxSynth : public VoxSynth    // stock one voice (monophonic) synth
{
   typedef VoxSynth super;             // superclass is VoxSynth

   public:

   void config();                      // executed at system startup
   void output( char* );               // write mono output to an audio buffer

   #ifdef KEYBRD_MENUS
   char    menu( key );                // given a key, return a character 
   #endif

} ;

/******************************************************************************
 *
 *                                TwoVoxSynth 
 *
 ******************************************************************************/

class TwoVoxSynth : public VoxSynth    // stock two voice (stereo) synth
{
   typedef VoxSynth super;             // superclass is VoxSynth

   public:

   void config();                      // executed at system startup
   void output( char*, char* );        // write stereo output to audio buffers

} ;

/******************************************************************************
 *
 *                              TwoVoxPanSynth 
 *
 ******************************************************************************/

class TwoVoxPanSynth : public TwoVoxSynth // two voice stereo synth with panning
{
   typedef TwoVoxSynth super;          // superclass is VoxSynth

   protected:

   // panPos determines the static pan position for vox[0] and vox[1].
   //
   //    panPos == 0   means vox[0] is panned completely to the left 
   //    panPos == 128 means vox[0] is centered in the stereo field
   //    panPos == 255 means vox[0] is panned completely to the right
   //
   // The pan position for vox[1] is the complement of that for vox[0].

   byte        panPos;                 // static pan position for voices  
   PanControl *panControl;             // dynamic controller of panPos

   public:

   void config();                      // executed at system startup

   boolean charEv( char code );        // handle a character event
   void dynamics();                    // perform a dynamic update
   void output( char*, char* );        // write stereo output to audio buffers

} ;

#endif   // ifndef SYNTH_H_INCLUDED
