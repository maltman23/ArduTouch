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

   PROMPT_STR( wavebank ) 

} ;

/* ------------------------      public vars      -------------------------- */

extern WaveBank wavebank;              // bank of all instantiable wave tables

// sinusoidal waveforms (with relative strengths of harmonics):

extern_wavetable( Sine )               // 1 - 100%

extern_wavetable( Sn2 )                // 1 - 100%  2 - 40%  
extern_wavetable( Sn3 )                // 1 - 100%  3 - 40%  
extern_wavetable( Sn4 )                // 1 - 100%  4 - 40%  
extern_wavetable( Sn5 )                // 1 - 100%  5 - 40%  
extern_wavetable( Sn6 )                // 1 - 100%  6 - 40%  
extern_wavetable( Sn8 )                // 1 - 100%  8 - 40%  

extern_wavetable( Sn2b )               // 1 - 100%  2 - 20%  
extern_wavetable( Sn3b )               // 1 - 100%  3 - 20%  
extern_wavetable( Sn4b )               // 1 - 100%  4 - 20%  
extern_wavetable( Sn5b )               // 1 - 100%  5 - 20%  
extern_wavetable( Sn6b )               // 1 - 100%  6 - 20%  
extern_wavetable( Sn8b )               // 1 - 100%  8 - 20%  

extern_wavetable( Sn23 )               // 1 - 100%  2 - 30%  3 - 15%
extern_wavetable( Sn24 )               // 1 - 100%  2 - 25%  4 - 12%
extern_wavetable( Sn24b )              // 1 - 100%  2 - 50%  4 - 25%
extern_wavetable( Sn25 )               // 1 - 100%  2 - 25%  5 - 16%
extern_wavetable( Sn26 )               // 1 - 100%  2 - 20%  6 - 40%
extern_wavetable( Sn34 )               // 1 - 100%  3 - 25%  4 - 16%
extern_wavetable( Sn35 )               // 1 - 100%  3 - 25%  5 - 12%
extern_wavetable( Sn35b )              // 1 - 100%  3 - 50%  5 - 25%
extern_wavetable( Sn36 )               // 1 - 100%  3 - 18%  6 - 24%
extern_wavetable( Sn38 )               // 1 - 100%  3 - 12%  8 - 24%
extern_wavetable( Sn39 )               // 1 - 100%  3 - 12%  9 - 24%
extern_wavetable( Sn45 )               // 1 - 100%  4 - 25%  5 - 16%
extern_wavetable( Sn46 )               // 1 - 100%  4 - 25%  6 - 12%
extern_wavetable( Sn56 )               // 1 - 100%  5 - 25%  6 - 16%

extern_wavetable( Sn234 )              // 1 - 100%  2 - 25%  3 - 40%  4 - 08%
extern_wavetable( Sn235 )              // 1 - 100%  2 - 40%  3 - 20%  5 - 10%
extern_wavetable( Sn245 )              // 1 - 100%  2 - 25%  4 - 40%  5 - 08%
extern_wavetable( Sn258 )              // 1 - 100%  2 - 20%  5 - 30%  8 - 20%
extern_wavetable( Sn267 )              // 1 - 100%  2 - 25%  6 - 40%  7 - 08%
extern_wavetable( Sn268 )              // 1 -  50%  2 - 80%  6 - 20%  8 - 12%
extern_wavetable( Sn345 )              // 1 - 100%  3 - 25%  4 - 40%  5 - 08%
extern_wavetable( Sn346 )              // 1 - 100%  3 - 60%  4 - 40%  6 - 20%
extern_wavetable( Sn456 )              // 1 - 100%  4 - 25%  5 - 40%  6 - 08%
extern_wavetable( Sn468 )              // 1 - 100%  4 - 25%  6 - 40%  8 - 12%
extern_wavetable( Sn357 )              // 1 - 100%  2 - 60%  5 - 40%  7 - 20%
extern_wavetable( Sn368 )              // 1 -  50%  3 - 80%  6 - 20%  8 - 12%

extern_wavetable( Sn2345 )             // 1 - 100%  2 - 40%  3 - 60%  4 - 20%  5 - 30%
extern_wavetable( Sn2346 )             // 1 - 100%  2 - 40%  3 - 24%  4 - 20%  6 - 12%
extern_wavetable( Sn2468 )             // 1 - 100%  2 - 80%  3 - 60%  4 - 40%  5 - 20%
extern_wavetable( Sn2479 )             // 1 - 100%  2 - 33%  4 - 20%  7 - 40%  9 - 12%
extern_wavetable( Sn2568 )             // 1 - 100%  2 - 24%  5 - 12%  6 - 16%  8 - 08%
extern_wavetable( Sn3456 )             // 1 - 100%  3 - 33%  4 - 25%  5 - 33%  6 - 50%
extern_wavetable( Sn3579 )             // 1 - 100%  3 - 80%  5 - 60%  7 - 40%  9 - 20%
extern_wavetable( Sn4567 )             // 1 - 100%  4 - 40%  5 - 60%  6 - 20%  7 - 30%
extern_wavetable( Sn5678 )             // 1 - 100%  5 - 40%  6 - 60%  7 - 20%  8 - 30%

// complex waveforms:

extern_wavetable( dVox )
extern_wavetable( Rood )
extern_wavetable( Ether )
extern_wavetable( SynStr )
extern_wavetable( PanFlute )

/* ----------------------     public functions     ------------------------- */


#endif   // ifndef WAVEBANK_H_INCLUDED