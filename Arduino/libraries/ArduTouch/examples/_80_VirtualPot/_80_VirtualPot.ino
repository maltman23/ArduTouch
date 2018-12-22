//
//  _80_VirtualPot.ino
//
//  The VirtualPot class is an abstract base class for constructing "soft" pots 
//  which translate pot positions into a custom output range of values (either 
//  increasing or decreasing).
//
//  For purposes of illustrating the functionality of the VirtualPot class
//  this sketch will instantiate VirtualPot objects, but in practice the 
//  VirtualPot class is meant to be used as a base class, from which a derived 
//  class can automate its connection to other elements of the synth -- as
//  will be shown in the example sketches that follow this one.
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

about_program( VirtualPot, 1.00 )            // specify sketch name & version

// In this synth we will use two virtual pots to control the frequency of a 
// PureSquare oscillator by changing it's edge length. Each virtual pot will
// be initialized to yield a different range of output values. The virtual pots 
// themselves will be called whenever a physical pot is moved.
//
// Four VirtualPot methods are demonstrated:
//
//    void setRange( byte left, byte right ) // specify range of output values
//    void setPos( byte pos )                // set the pot position
//    byte getPos()                          // get the current pot position
//    byte evaluate( byte pos )              // return output value for a given position

class VirtualPotSynth : public Synth
{
   typedef Synth super;                      // super class is Synth

   public:

   PureSquare osc;                           // use 1 pure square wave oscillator 

   VirtualPot vpot0;                         // this virtual pot will be called from POT0 
   VirtualPot vpot1;                         // this virtual pot will be called from POT1 

   void welcome()
   {
      osc.reset();                           // initialize osc 
      osc.setFreq( 440.0 );                  // start osc at (about) 440 Hz

      // The VirtualPot method setRange( byte left, byte right ) allows you to map 
      // the pot positions 0 thru 255 to the values left thru right

      // virtual pot 0 values will range from 40 at the leftmost position (pos=0)
      //                                   to 50 at the rightmost position (pos=255)

      vpot0.reset();
      vpot0.setRange( 40, 50 );

      // virtual pot 1 values will range from 255 at the leftmost position (pos=0)
      //                                   to   1 at the rightmost position (pos=255)

      vpot1.reset();
      vpot1.setRange( 255, 1 );
   }

   // Our evHandler() sets the virtual pot positions from the physical pots, then 
   // gets the virtual pot output values and uses them to set the oscillator edge 
   // length.
   //
   // Note: the next sketch will demonstrate how to do this in a simpler way.

   bool evHandler( obEvent e )               // event handler
   {
      byte vpotval;                          // vpot value at given position

      switch ( e.type() )
      {                                      
         case POT0:                          // set vpot 0 pos, get val, set edge len
         {
            vpot0.setPos( e.getPotVal() );
            vpotval = vpot0.evaluate( vpot0.getPos() );
            osc.setEdgLen( vpotval );
            break;
         } 
                                             // set vpot 0 pos, get val, set edge len
         case POT1:
         {
            vpot1.setPos( e.getPotVal() );
            vpotval = vpot1.evaluate( vpot1.getPos() );
            osc.setEdgLen( vpotval );
            break;
         }
         default:       

            return Synth::evHandler(e);      // pass other events through
      }
      return true;
   }

   void output( char *buf )                  // output 1 buffer of audio
   {
      osc.output( buf );                     // pass buffer to oscillator
   }

} myVirtualPotSynth;


void setup()
{
   ardutouch_setup( &myVirtualPotSynth );    // initialize ArduTouch resources
}

void loop()
{
   ardutouch_loop();                         // perform ongoing ArduTouch tasks  
}                                             

//  ---------------------------------------------------------------------------
//
//  If you compile and upload this sketch to the ArduTouch you will hear a 
//  tone at around 440 Hz. Turning the top pot wull change the frequency in
//  a medium range from high to low, and turning the bottom pot will change
//  the frequency in a wide range from low to high.
//
//   --------------------------------------------------------------------------



