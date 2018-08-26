//
//  _55_Pot_Events.ino
// 
//  This sketch shows how to process ArduTouch pot events. 
//   
//  A pot-handling synth is derived from the ArithmeticSynth of example _04_.
//  The ArithmeticSynth is totally unchanged -- we will just add the ability to 
//  use the two pots to change frequencies played when touching keys.
//  This new synth uses the top pot to control the frequency of low C,
//  and the bottom pot to control the frequency interval between keys.
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

about_program( Pot Events, 1.00 )            // specify sketch name & version

class ArithmeticSynth : public Synth         // synth from example _04_
{
   public:

   WaveOsc osc;                              // use a wave oscillator 
   double  baseHz;                           // frequency for low C
   double  tuneHz;                           // freq interval between keys
   boolean keyDown;                          // if true, a key is pressed

   void  setup();                            // ... see bottom of file
   boolean evHandler( obEvent e );           //     for implementation
   void output( char *buf );                 //     of these methods
} ;


// This PotSynth simply adds functionality to the ArithmeticSynth from example _04_

class PotSynth : public ArithmeticSynth
{
   boolean evHandler( obEvent e )            // event handler
   {
      switch ( e.type() )                    // branch on event type
      {
         case POT0:                          // top pot was moved
         {
            // set baseHz from 10.0 to 520.0 Hz, based on pot position

            byte potVal = e.getPotVal();     // get pot value (which will be between 0-255)
            baseHz = 10.0 + potVal*2.0;      // set baseHz
            return true;                     // event was handled
         }
         case POT1:                          // bottom pot was moved

            // set tuneHz from 0.5 to 128.0 Hz, based on pot position

            tuneHz = .5 + e.getPotVal()*.5;  // set tuneHz
            return true;                     // event was handled

         // pass all other events to ArithmeticSynth handler

         default:                            

            return ArithmeticSynth::evHandler(e); 
      }
   }

} myPotSynth ;                                   

/*----------------------------------------------------------------------------*/      

void setup()
{
   ardutouch_setup( &myPotSynth );           // initialize ArduTouch resources
}

void loop()
{
   ardutouch_loop();                         // perform ongoing ArduTouch tasks  
}                                             

/*----------------------------------------------------------------------------*/      

void ArithmeticSynth::setup() 
{                                         
   baseHz  = 200.0;                          // low C = 200.0 Hz
   tuneHz  = 25.0;                           // 25.0 Hz arithmetic tuning
   keyDown = false;                          // begin with no key pressed
   osc.reset();                              // initialize osc state
   osc.setTable( wavetable(Sine) );          // use Sine table from library
}

boolean ArithmeticSynth::evHandler( obEvent e ) 
{
   switch ( e.type() )
   {
      case KEY_DOWN:                         // a key has been pressed
      {
         keyDown = true;                     // update keyDown status
         key  k = e.getKey();                // get key info from event
         byte p = k.position();              // get key's position in octave
         double t = baseHz + (p*tuneHz);     // calculate tuning for key
         osc.setFreq( t );                   // set oscillator frequency
         return true;                        // KEY_DOWN event was handled
      }
      case KEY_UP:                           // key has been released
         keyDown = false;                    // update keyDown status
         return true;                        // KEY_UP event was handled
      default:       
         return false;                       // pass over all other events
   }
}

void ArithmeticSynth::output( char *buf )    // output 1 buffer of audio
{
   if ( keyDown )                            // if a key is currently down
      osc.output( buf );                     // pass buffer to oscillator
   else                                      // else no key is down 
   {                                         // so clear the buffer:
      byte icnt = audioBufSz;                // icnt = # bytes in buffer
      while ( icnt-- )                       // for each byte in buffer
         *buf++ = 0;                         // clear byte
   }
}

//  ---------------------------------------------------------------------------
//
//  If you compile and upload this sketch to the ArduTouch you should hear a 
//  sine wave tone when a key is pressed. The tone's frequency will increase
//  by 25 Hz per key, from left to right. Low C will play a 200 Hz tone. By
//  turning the top pot you can change the frequency of low C. By turning the
//  bottom pot you can change the frequency interval between keys.
//
//  ---------------------------------------------------------------------------      



