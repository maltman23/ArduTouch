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

class PresetBank : public MacroBank
{
   public:

   void  onChoice()
   {
      audio::pause();
      MacroBank::onChoice();
      audio::resume();
   } 

   char *prompt() 
   { 
      return "preset"; 
   }

}  bank;


void Synth::charEv( char code )
{
   switch ( code )
   {
      case 'p':                     // choose a preset

         bank.choose();
         break;

      default:

         StereoPhonic::charEv( code );
   }
}

void Synth::setup( const bankmem *p )
{
   bank.load( p );
}
