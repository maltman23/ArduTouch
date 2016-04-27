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
#include "Console_.h"

typedef struct {                 // desWavTab : WaveTable descriptor

   const signed char* table;     // ptr to table of waveform pts
   unsigned long      length;    // # of samples in table 
   double             period;    // # of samples per wavelen

   }  desWavTab;

class WaveBank : public Bank           // bank of wave tables
{                                      
   public:

   WaveBank();

   desWavTab&  operator[]( byte );     // return descriptor of nth wave table   
   char*       prompt();               // return object's prompt string

} ;

/* ------------------------      public vars      -------------------------- */

extern WaveBank wavebank;              // bank of all instantiable wave tables

/* ----------------------     public functions     ------------------------- */


#endif   // ifndef WAVEBANK_H_INCLUDED