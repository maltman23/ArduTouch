/*
    WaveBank.h  

    Declarations for the WaveBank module.

    ---------------------------------------------------------------------------
 
    Copyright (C) 2016, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
   ---------------------------------------------------------------------------
*/

#ifndef WAVEBANK_H_INCLUDED
#define WAVEBANK_H_INCLUDED

#include "Bank.h"
#include "WaveTable.h"

#define _wavetable(x) _member( &wavtabDesc##x, wavtabName##x )

class WaveBank : public Bank           // bank of wave tables
{                                      
   public:

   #ifdef CONSOLE_OUTPUT
   const char *prompt() { return CONSTR("waveform"); }
   #endif
} ;

/* ------------------------      public vars      -------------------------- */

extern WaveBank wavebank;              // bank of all instantiable wave tables

extern_wavetable( Sine )
extern_wavetable( dVox )
extern_wavetable( Rood )
extern_wavetable( Ether )
extern_wavetable( SynStr )

/* ----------------------     public functions     ------------------------- */


#endif   // ifndef WAVEBANK_H_INCLUDED