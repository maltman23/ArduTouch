/*
    WaveTable.h  

    Declarations of wave table structure and macros.

    ---------------------------------------------------------------------------
 
    Copyright (C) 2016, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
   ---------------------------------------------------------------------------
*/

#ifndef WAVETABLE_H_INCLUDED
#define WAVETABLE_H_INCLUDED

#define begin_samples(x)  PROGMEM const signed char samples##x[] = {
#define end_samples(x)    };   

#define define_wavetable(x, numpts, wavlen)                                \
                                                                           \
   const char wavtabName##x[]    PROGMEM = { #x };                         \
   const desWavTab wavtabDesc##x PROGMEM = { samples##x, numpts, wavlen };

#define extern_wavetable(x)                                                \
                                                                           \
   extern const char wavtabName##x[];                                      \
   extern const desWavTab wavtabDesc##x;

#define wave_descriptor(x)  &wavtabDesc##x, CONSTR(#x)                                              

typedef struct {                       // desWavTab : WaveTable descriptor

   const signed char* table;           // ptr to table of waveform pts
   unsigned long      length;          // # of samples in table 
   double             period;          // # of samples per wavelen

   }  desWavTab;

#endif   // ifndef WAVETABLE_H_INCLUDED