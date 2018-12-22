//
//  _81_ParamSend.ino
//
//  The ParamSend class inherits from VirtualPot. It translates pot positions into
//  a custom output range. Each time its pot position is set, an output value is
//  calculated and sent as an argument to a previously specified method of a 
//  previously specified target object.
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

about_program( ParamSend, 1.00 )             // specify sketch name & version

// In this synth we will use two ParamSend objects to control the frequency of a 
// PureSquare oscillator by changing it's edge length. Each ParamSend will
// be initialized to yield a different range of output values. Each ParamSend
// will also be initialized with the address of the target object to send its
// output to (in this case the PureSquare oscillator) and the offset to a
// method in the target object (in this case setEdgLen()) which it will call
// with the output value as the argument to the target method. 
//
// Two inherited VirtualPot methods are used here:
//
//    void setRange( byte left, byte right ) // specify range of output values
//    void setPos( byte pos )                // set the pot position
//

class ParamSendSynth : public Synth
{
   typedef Synth super;                      // super class is Synth

   public:

   PureSquare osc;                           // use 1 pure square wave oscillator 

   ParamSend send0;                          // this send will be called from POT0 
   ParamSend send1;                          // this send will be called from POT1 

   void welcome()
   {
      osc.reset();                           // initialize osc 
      osc.setFreq( 440.0 );                  // start osc at (about) 440 Hz

      send0.reset();
      send0.setRange( 40, 50 );              // send0 outputs values between 40 and 50

      send1.reset();
      send1.setRange( 255, 1 );              // send1 outputs values between 255 and 1

      // the connect() method is used to specify the address of a target object 
      // (1st argument), and a method to be called within that object (2nd argument). 
      //
      // The method to be called in the target must be of return type void, and must 
      // take exactly 1 argument of type byte.
      //
      // ptr1ByteMethod() is a macro in the ArduTouch library which simplifies
      // the syntax of the 2nd argument.

      send0.connect( &osc, ptr1ByteMethod( &PureSquare::setEdgLen ) );
      send1.connect( &osc, ptr1ByteMethod( &PureSquare::setEdgLen ) );

      // the above connect() statements mean that whenever a send has its pot 
      // position set it will call osc.setEdgLen() with the output value as 
      // an argument to setEdgLen()
   }

   // Our evHandler() sets the oscillator's edge length indirectly simply by setting 
   // the pot positions of the sends
   //
   // Note: compare this with the previous example sketch

   bool evHandler( obEvent e )               // event handler
   {
      switch ( e.type() )
      {                                      
         case POT0:                          // set edge length through send0

            send0.setPos( e.getPotVal() );
            break;
                                             // set edge length through send1
         case POT1:

            send1.setPos( e.getPotVal() );
            break;

         default:       

            return Synth::evHandler(e);      // pass other events through
      }
      return true;
   }

   void output( char *buf )                  // output 1 buffer of audio
   {
      osc.output( buf );                     // pass buffer to oscillator
   }

} myParamSendSynth;


void setup()
{
   ardutouch_setup( &myParamSendSynth );     // initialize ArduTouch resources
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



