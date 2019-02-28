/*
    ClickTrack.h  

    Declaration of the ClickTrack class.

    ClickTrack is a Voice which has been set up to emit a 'click' sound,
    like that of a metronome. ClickTrack can emit two kinds of clicks,
    one for a downbeat, and one for an upbeat.

    ---------------------------------------------------------------------------
 
    Copyright (C) 2018, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
   ---------------------------------------------------------------------------
*/

#ifndef CLICKTRACK_H_INCLUDED
#define CLICKTRACK_H_INCLUDED

#include "Voice.h"
#include "NoiseOsc.h"

class ClickTrack : public ADSRVoice
{
   typedef ADSRVoice super;            // superclass is Voice
   
   public:

   ClickTrack()
   {
      useOsc( new WhiteNoise() );
   }

   bool  charEv( char );               // handle a character event
   void  dynamics();                   // update dynamics
   void  downbeat();                   // strike a downbeat
   void  output( char * );             // write output to an audio buffer
   void  upbeat();                     // strike an upbeat

} ;

#endif   // ifndef CLICKTRACK_H_INCLUDED