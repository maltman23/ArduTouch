//
//  _04_Pot_Control.ino
// 
//  Example 04 of introductory examples
//     to learn basics of programming your own ArduTouch synthesizers
//  To get the most of these exmples, please go through them in sequence. 
//
// 
//  In this sketch, rather than have the volume dynamically changing with the 
//  dynamics() method (as we did in Example 03) we will use a pot on the board 
//  to control the volume of a sawtooth waveform .
//
//  This example sketch introduces evHandler(), which allows you to respond
//  to user activity (called an "event"), such as changing a pot, pressing a 
//  button, or touching a key on the keyboard.
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

#include "ArduTouch.h"                    // use the ArduTouch library 

about_program( Pot Control, 1.00 )        // specify sketch name & version

class PotControlSynth : public Synth      // output a sawtooth waveform whose volume
{                                         // is controlled by the top pot
   public:

   char  sample;                          // sample value 
   byte  volume;                          // volume level

   //  evHandler() is called by the system whenever an "event" occurs on the 
   //  ArduTouch board, such as pressing or releasing a key, adjusting a pot
   //  or pressing a button. 
   //
   //  Here we are only interested in POT0 events (top pot was moved).

   boolean evHandler( obEvent e )         // event handler
   {
      switch ( e.type() )                 // branch on event type
      {
         case POT0:                       // top pot was moved

            volume = e.getPotVal();       // set volume from pot value (0-255)
            return true;                  // event was handled
            break;

         default:                         // pass all other events to normal Synth handler

            return Synth::evHandler(e); 
      }
   }

   void output( char *buf )               // output 1 buffer of audio
   {
      byte icnt = audioBufSz;             // audioBufSz is a library constant
      while ( icnt-- )                    // for each byte in buffer
      {                                   // ...
         int sampleXV = sample * volume;  // multiply sample value by volume level 
         *buf++ = sampleXV >> 8;          // write high byte of multiplied sample to buffer
         sample++;                        // bump sample value
      }
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

