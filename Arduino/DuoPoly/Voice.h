/*
    Voice.h  

    Declaration of the Voice class.

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

#include "WaveOsc.h"
#include "Effect.h"

class Voice : public WaveOsc
{
   public:

   EffectsLoop effects;  

   char   vNum;                        // character code for "voice #'

   void   charEv( char code )          // process a character event
   {
      switch ( code )
      {
         case cmdTrigger:
      
            tremolo.trigger();
            vibrato.trigger();
            break;

         case 'e':

            console.pushMode( &this->effects );
            break;

         case '!':

            effects.charEv( code );    // fall thru to WaveOsc handler
            
         default:

             WaveOsc::charEv( code );
      }
   }

   void   generate( char *buf )        // generate one buffer of audio
   {
      if ( amMute() )
      {
         byte icnt = audio::bufSz;     
         while ( icnt-- ) *buf++ = 0;
         return;
      }

      WaveOsc::generate( buf );
      effects.process( buf );
   }

   char*  prompt()                     // return object's prompt string
   {
      static char* pStr = "voice  ";
      pStr[6] = vNum;
      return pStr;
   }

} ;


#endif   // ifndef VOICE_H_INCLUDED
