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
#include "Phonic.h"
#include "Osc.h"
#include "Effect.h"
#include "Envelope.h"
#include "LFO.h"
#include "Scroller.h"


class Tremolo : public TrigLFO
{
   public:

   boolean charEv( char );       // process a character event
   void   evaluate();            // set output val (as a function of LFO pos)

   #ifdef CONSOLE_OUTPUT
   const char*  prompt();        // return object's prompt string
   #endif

} ;


class Vibrato : public FadeLFO
{
   public:

   void   evaluate();            // set output val (as a function of LFO pos)
   void   iniPos();              // set initial osc position

   #ifdef CONSOLE_OUTPUT
   const char*  prompt();        // return object's prompt string
   #endif

} ;

class Voice : public MonoPhonic, public Scroller
{
   public:

   enum {  // use these bits in the flags byte: 

           TRIG    = _RESERVE1,   // a trigger is pending 

        } ;

   Osc    *osc;                  // ptr to raw oscillator

   EffectsLoop effects;          // effects loop 

   Voice();

   boolean charEv( char );       // process a character event
   void    dynamics();           // update dynamics
   boolean evHandler( obEvent ); // handle an onboard event
   void    output( char * );     // write output to an audio buffer
   void    setFreq( double );    // set the ideal playback frequency
   void    setGlide( byte );     // set portamento speed
   void    setGlobVol( byte );   // set the global volume level
   void    setVol( byte );       // set the volume level

   #ifdef KEYBRD_MENUS
   char    menu( key );          // map key event to character 
   #endif

   #ifdef CONSOLE_OUTPUT
   const char *prompt();         // return object's prompt string
   #endif

   protected:

   key     pendNote;             // play this note at next dynamic update

   byte    globVol;              // global volume level
   Word    effVol;               // effective volume (msb: integral part,
                                 //                   lsb: fractional part)
   Int     instVol;              // instantaneous volume (msb: integral part,
                                 //                       lsb: fractional part)
   Int     segVol;               // if segue in process, ultimate instVol 

   Envelope envAmp;              // amplitude envelope

   Tremolo tremolo;              // built-in tremolo effect
   Vibrato vibrato;              // built-in vibrato effect

   private:

   byte    glide;                // portamento speed (0 = off)
   char    dirGlide;             // portamento direction +/-/0
   double  instGlide;            // instantaneous portamento coefficient
   double  multGlide;            // instGlide *= multGlide each dynamic update

   void    calcEffVol();         // calculate the effective volume level
   void    calcMultGlide();      // calculate dynamic glide multiplier
   void    doneGlide();          // set real-time glide params to "done"

} ;


#endif   // ifndef VOICE_H_INCLUDED
