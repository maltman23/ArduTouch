//
//  _70_Quantum_Oscillators.ino
//
//  Target:   ArduTouch board
// 
//  ---------------------------------------------------------------------------
// 
//  Copyright (C) 2018, Cornfield Electronics, Inc.
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

about_program( Quantum_Oscillators, 1.00 )   // specify sketch name & version

class QuantumOscSynth : public TwoVoxSynth
{
   typedef TwoVoxSynth super;                // super class is TwoVoxSynth

   public:

   Osc *newOsc( byte ith )                   // callback function for creating oscillators
   {
      switch ( ith )
      {
         case 0:  return new Square();       // left channel is normal square wave
         case 1:  return new PureSquare();   // right channel is pure square wave
      }
   }

   bool evHandler( obEvent e )               // event handler
   {
      switch ( e.type() )
      {                                      
         case POT0:                          // set voice relative volumes
         {
            vox[0]->setVol( 255-e.getPotVal() );
            vox[1]->setVol( e.getPotVal() );
            break;
         }
         default:       
            return Synth::evHandler(e);      // pass other events through
      }
      return true;
   }

} myQuantumOscSynth;


void setup()
{
   ardutouch_setup( &myQuantumOscSynth );    // initialize ArduTouch resources
}

void loop()
{
   ardutouch_loop();                         // perform ongoing ArduTouch tasks  
}                                             

//  ---------------------------------------------------------------------------
//
//  If you compile and upload this sketch to the ArduTouch you should hear a 
//  synthetic-choir like tone at 440 Hz. Pressing the keys will play tones in an
//  equal temperament tuning. Tapping the right button will raise the keyboard
//  by 1 octave. Tapping the left button will lower the keyboard by 1 octave.
//
//   --------------------------------------------------------------------------



