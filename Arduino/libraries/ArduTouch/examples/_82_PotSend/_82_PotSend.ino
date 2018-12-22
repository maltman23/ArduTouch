//
//  _82_PotSend.ino
//
//  The PotSend class inherits from VirtualPot. It translates pot positions into
//  a custom output range. Each time its pot position is set, an output value is
//  calculated and sent as a POT event (either a POT0 or POT1 event) to the 
//  evHandler() of a previously specified target object.
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

about_program( PotSend, 1.00 )               // specify sketch name & version

// In this synth we will use two PotSend controls to change the frequency and
// depth of a Sine wave. Each PotSend will be initialized to yield a different 
// range of output values. Each PotSend will also be initialized with the 
// address of a target object to send its output to as a POT event.
//
// Two inherited VirtualPot methods are used here:
//
//    void setRange( byte left, byte right ) // specify range of output values
//    void setPos( byte pos )                // set the pot position
//

class PotSendSynth : public OneVoxSynth
{
   typedef Synth super;                      // super class is Synth

   public:

   PotSend send0;                            // this send will be called from POT0 
   PotSend send1;                            // this send will be called from POT1 

   void welcome()
   {
      // OneVoxSynth creates a StockVoice and sets a ptr to it in vox[0] 
      // A StockVoice is a Voice with an added vibrato control.
      // Because vox[] is typed as an array of generic Voice ptrs, we typecast
      // a ptr to vox[0] so we can reference its vibrato control.

      StockVoice *voice = (StockVoice *)vox[0]; // type cast generic vox[] ptr

      voice->setFreq( 440.0 );               // start osc at 440 Hz
      voice->vibrato.setMute( false );       // turn vibrato on
      voice->trigger();                      // and start it playing

      // initialise PotSend controls

      send0.reset();
      send0.setRange( 250, 100 );            // send0 outputs values between 250 and 100

      send1.reset();
      send1.setRange( 10, 200 );             // send1 outputs values between 10 and 200

      // the connect() method is used to specify the address of a target object 
      // (1st argument) to send a POT event to, and which pot number to use for
      // the event (2nd argument)   

      send0.connect( &voice->vibrato, 0 );
      send1.connect( &voice->vibrato, 1 );

      // the above connect() statements mean that whenever a send has its pot 
      // position set it will map that position into its output range and 
      // send that output value as a POT event (POT0 in the case of send0, 
      // POT1 in the case of send1) to the vibrato control of vox[0]

      // Most objects in the ArduTouch library have built-in event handling.
      // In the case of Vibrato objects, POT0 events will control the vibrato's 
      // frequency, and POT1 events will control the vibrato's depth.
   }

   // Our evHandler() sets the voice's vibrato depth and frequency indirectly 
   // by setting the pot positions of the sends

   bool evHandler( obEvent e )               // event handler
   {
      switch ( e.type() )
      {                                      
         case POT0:                          // set vibrato freq through send0

            send0.setPos( e.getPotVal() );
            break;
                                             // set vibrato depth through send1
         case POT1:

            send1.setPos( e.getPotVal() );
            break;

         default:       

            return Synth::evHandler(e);      // pass other events through
      }
      return true;
   }

} myPotSendSynth;


void setup()
{
   ardutouch_setup( &myPotSendSynth );       // initialize ArduTouch resources
}

void loop()
{
   ardutouch_loop();                         // perform ongoing ArduTouch tasks  
}                                             

//  ---------------------------------------------------------------------------
//
//  If you compile and upload this sketch to the ArduTouch you will hear a 
//  tone at 440 Hz. Turning the top pot will change the frequency of the
//  vibrato in a range from high to low, and turning the bottom pot will 
//  change the depth of the vibrato in a range from low to high.
//
//   --------------------------------------------------------------------------

