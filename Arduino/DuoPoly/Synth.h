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
#include "Console_.h"

class Synth : public StereoPhonic
{
   public:

   void    charEv( char );                   // process a character event
   virtual void  setup( const bankmem * );   // initialize synth (load presets)

} ;

#endif   // ifndef SYNTH_H_INCLUDED
