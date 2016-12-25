/*
    Synth.h  

    Declaration of the Synth base class for all synthesizers.

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

#include "Phonic.h"
#include "Bank.h"

#define define_preset(x, y)                                                \
                                                                           \
   const char presetName##x[] PROGMEM = { #x };                            \
   const char presetData##x[] PROGMEM = { y };

#define extern_preset(x)                                                   \
                                                                           \
   extern const char presetName##x[];                                      \
   extern const char presetData##x[]

#define _preset(x) _member( presetData##x, presetName##x )

class Synth : public Phonic
{
   public:

   boolean charEv( char );                   // process a character event
   boolean inStereo();                       // true if synth produces stereo 
                                             // output (else, output is mono)
   void    load( const bankmem * );          // load a bank of presets
   virtual void setup();                     // executed during system setup

   #ifdef CONSOLE_OUTPUT
   const char *prompt();                     // return object's prompt string
   #endif

   protected:

   boolean stereo;                           // true is synth is stereo

} ;

class MonoSynth : public Synth
{
   public:

   virtual void output( char* ) 
   {
      // write output to one (mono) audio buffer
   };     

} ;

class StereoSynth : public Synth
{
   public:

   StereoSynth() { stereo = true; }

   virtual void output( char*, char* ) 
   {
      // write output to a left-right pair of audio buffers
   };  

} ; 
   
#endif   // ifndef SYNTH_H_INCLUDED
