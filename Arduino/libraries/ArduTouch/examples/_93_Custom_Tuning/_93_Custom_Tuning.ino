//
//  _35_Custom_Tuning.ino
// 
//  This sketch demonstates how to create a custom tuning and use it in
//  place of the default system tuning.
//
//  By default the system will use a standard tuning of equal temperament.
//  
//  In this sketch we will create a special tuning in which the black keys 
//  are tuned a quarter-tone higher than the white keys (instead of a semi-
//  tone higher, as is normal).
//
//  Target:   ArduTouch board
// 
//  ---------------------------------------------------------------------------
// 
//  Copyright (C) 2019, Cornfield Electronics, Inc.
// 
//  This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
//  Unported License.
// 
//  To view a copy of this license, visit
//  http://creativecommons.org/licenses/by-sa/3.0/
// 
//  Created by Bill Alessi & Mitch Altman.
// 
//  ---------------------------------------------------------------------------

#include "ArduTouch.h"                    // use the ArduTouch library 

about_program( Custom_Tuning, 1.00 )      // specify sketch name & version

// There are two steps involved in creating and using a custom tuning for your synth.
// These steps, which will be illustrated below, are: 
//
// 1) Implement a new tuning class by inheriting from the Tuning base class and
//    overriding its pitch() method.
//
// 2) Instantiate an instance of your new tuning class and register it with the
//    system via the tuning() call-back method for your synth.
//
//  ---------------------------------------------------------------------------
//  
// All tuning classes ultimately inherit from the Tuning base class.
// The Tuning class is very simple -- it consists of a single virtual method:
//
//    virtual double Tuning::pitch( key ) 
//
// whose input is a key struct, and whose output is the frequency for that key.
//
// Given a key struct x you can find out the octave and position of the key
// relative to low 'C' in that octave by the following 2 functions:
//
//    key x;
//    byte octave   = x.octave();
//    byte position = x.position();
//
// 
//  

class QuarterToneBlackKeys : public Tuning
{
   protected:

   // the lookup table in ROM which specifies frequencies for notes in octave 0
   // is defined after the declaration of this class

   static const double freqs[ numNotes ];  // frequencies for notes in octave 0

   public:

   // return frequency for a given key

   double pitch( key k )
   {
      double f = pgm_read_float_near( freqs + k.position() );
      return f * ( 1 << k.octave() );
   }

} ;

#define QUARTERTONE_INTERVAL 1.02930188   // frequency ratio between quarter-tones

// Here are the standard frequencies for the white notes in octave 0

#define C 16.351601 
#define D 18.344023 
#define E 20.601718
#define F 21.826757
#define G 24.499726
#define A 27.500000
#define B 30.867695

// We define the frequency of the black notes in octave 0 to be a quarter tone 
// above the white notes that precede them:

#define C_SHARP C * QUARTERTONE_INTERVAL
#define D_SHARP D * QUARTERTONE_INTERVAL
#define F_SHARP F * QUARTERTONE_INTERVAL
#define G_SHARP G * QUARTERTONE_INTERVAL
#define A_SHARP A * QUARTERTONE_INTERVAL

// We create a lookup table in ROM for all the frequencies of notes in octave 0

const double QuarterToneBlackKeys::freqs[] PROGMEM = 
{ 
   C, C_SHARP, D, D_SHARP, E, F, F_SHARP, G, G_SHARP, A, A_SHARP, B, C*2
} ;


class CustomTuningSynth : public OneVoxSynth  
{                                         
   public:

   // The callback method tuning() is automatically called by the system during
   // setup, and allows you to override the standard system tuning

   Tuning *tuning()
   {
      return new QuarterToneBlackKeys();  // use custom tuning
   }
   
} mySynth;                                // instantiate synthesizer 


void setup()
{
   ardutouch_setup( &mySynth );           // initialize ArduTouch resources
}


void loop()
{
   ardutouch_loop();                      // perform ongoing ArduTouch tasks  
}                                             


// OneVoxSynth is a high-level class which allows you to use the keyboard
// to play notes. Tapping the left button lowers the keyboard by an octave,
// tapping the right button raises the keyboard by an octave.
// Also with OneVoxSynth, the top pot is a volume control by default.
