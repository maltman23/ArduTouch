/*
    Tuning.cpp  

    Implementation of the Tuning classes.

    ---------------------------------------------------------------------------
 
    Copyright (C) 2016, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
   ---------------------------------------------------------------------------
*/

#include "Tuning.h"

Tuning *masterTuning = new EqualTemperament();

const double EqualTemperament::freq[] PROGMEM = 
{ 
   16.351601, 17.323906, 18.344023,
   19.445429, 20.601718, 21.826757,
   23.124648, 24.499726, 25.956562,
   27.500000, 29.135234, 30.867695, 
   32.703202
} ;

double EqualTemperament::pitch( key k )
{
   double f = pgm_read_float_near( freq + k.position() );
   return f * ( 1 << k.octave() );
}

