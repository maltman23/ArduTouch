/*
    Synth.cpp  

    Implementation of the Synth and auxillary classes.

    ---------------------------------------------------------------------------
 
    Copyright (C) 2016, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
   ---------------------------------------------------------------------------
*/

#include "Audio.h"
#include "Synth.h"

PresetBank presets;                 // bank of synth presets

boolean Synth::charEv( char code )
{
   switch ( code )
   {
      #ifdef INTERN_CONSOLE

      case 'p':                     // choose a preset

         presets.choose();
         break;

      case '#':                     // toggle mute, and mute_at_reset status 

         // Use '#' as the 1st and last character in a preset to insure its
         // noise-free loading. '#' toggles the mute status of the synth, and
         // suppresses the implicit unmute at reset.

         mute_at_reset = ! mute_at_reset;
         setMute( mute_at_reset );
         break;

      #endif

      case '!':                     // reset

         if ( ! mute_at_reset )         
            Phonic::charEv('!');
         break;

      case '.':

         audio::wait( 30 );         // wait for contextual volume to "settle"
         Phonic::charEv('.');
         break;

      case '<':

         mute_at_reset = false;     // clear any mute-at-reset status
         // fall thru 

      default:

         return Phonic::charEv( code );
   }
   return true;
}

boolean Synth::inStereo()
{
   return stereo;
}

#ifdef KEYBRD_MENUS
char Synth::menu( key k )
{
   switch ( k.position() )
   {
      case  0: return 'p';
      default: return Phonic::menu(k);
   }
}
#endif

#ifdef CONSOLE_OUTPUT
const char *Synth::prompt()                     
{
   return CONSTR("synth");
}
#endif

void Synth::setup()
{
   reset();
}