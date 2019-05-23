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


/* just scale:
Unison        1/1   1.0000
Minor Second  25/24 1.0417 
Major Second   9/8  1.1250
Minor Third    6/5  1.2000
Major Third    5/4  1.2500 
Fourth         4/3  1.33483
Diminish 5th  45/32 1.4063 
Fifth          3/2  1.5000
Minor Sixth    8/5  1.6000
Major Sixth    5/3  1.68179
Minor Seventh  9/5  1.8000
Major Seventh 15/8  1.8750
Octave         2/1  2.0000
*/

const double HarmonicTuning::intervals[] PROGMEM = 
{ 
   1.0, 1.0417, 1.1250,
   1.2, 1.25, 1.33483,
   1.4063, 1.5, 1.6,
   1.68179, 1.8, 1.8750,
   2.0
} ;

double HarmonicTuning::pitch( key k )
{
   double ratio = pgm_read_float_near( intervals + k.position() );
   return tonic0 * ratio * ( 1 << k.octave() );
}
