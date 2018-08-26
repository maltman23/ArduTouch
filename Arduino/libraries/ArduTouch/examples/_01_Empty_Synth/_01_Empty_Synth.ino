//
//  _01_Empty_Synth.ino
// 
//  Example 01 of introductory examples
//     to learn basics of programming your own ArduTouch synthesizers
//  To get the most of these exmples, please go through them in sequence. 
//
// 
//  This example sketch provides a template for the bare minimum ArduTouch synthesizer.
//  This example synthesizer produces no sound.
//  This example sketch shows the structure of all ArduTouch sketches.
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

// the following line is required for every ArduTouch sketch

about_program( Empty Synth, 1.00 )           // specify sketch name & version

class EmptySynth : public Synth              // define your synthesizer
{
   // this synthesizer has no contents and therefore makes no sound

} mySynth;                                    

// every ArduTouch sketch has only one line in the setup() section 
// with a pointer to your synthesizer -- in this case:  mySynth 


void setup()
{
   ardutouch_setup( &mySynth );              // initialize ArduTouch resources
}


// every ArduTouch sketch has exactly this loop() section

void loop()
{
   ardutouch_loop();                         // perform ongoing ArduTouch tasks  
}                                             
