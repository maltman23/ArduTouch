//
//  _01_Bare_Minimum.ino
// 
//  This sketch provides a template for the bare minimum ArduTouch synthesizer.
//
//  Target:   ArduTouch board
// 
//  ---------------------------------------------------------------------------
// 
//  Copyright (C) 2016, Cornfield Electronics, Inc.
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

#include "ArduTouch.h"                       // use the ArduTouch library 

// the following two lines are required for every ArduTouch sketch

about_program( Bare Minimum, 1.00 )          // specify sketch name & version
set_baud_rate( 115200 )                      // specify serial baud-rate

// here is where you create a synthesizer (in this case, an empty one, that makes no sound)

class EmptySynth : public Synth              // define your synthesizer
{
} myEmptySynth;                              // instantiate synthesizer 


// every ArduTouch sketch has exactly this setup() section

void setup()
{
   ardutouch_setup( &myEmptySynth );         // initialize ArduTouch resources
}


// every ArduTouch sketch has exactly this loop() section

void loop()
{
   ardutouch_loop();                         // perform ongoing ArduTouch tasks  
}                                             

//  ----------------------------------------------------------------------------
//
//  The crux of every ArduTouch sketch is the definition and instantiation
//  of a synthesizer object to be used by the system. The ArduTouch library
//  supplies several different base classes from which to derive a synthesizer.
//  In this example we use the very simplest: Synth
//   
//  In this sketch we have defined an "empty" synthesizer, that makes no sound.
//
//  In the next tutorial we will define a synth that makes some sound.
//
//  ----------------------------------------------------------------------------      


