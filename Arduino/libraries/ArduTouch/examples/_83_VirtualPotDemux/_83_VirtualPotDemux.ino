//
//  _83_VirtualPotDemux.ino
//
//  The VirtualPotDemux class is a demultiplexer of other virtual pots.
//
//  Up to 8 virtual pots (or any object of a class that inherits from VirtualPot)
//  can be connected to a VirtualPotDemux. 
//
//  In this two voice synth a single VirtualPotDemux will be commected to 3 
//  PotSend objects and 1 ParamSend object, which have been set up to control
//  the depth of the 2 voice vibratos, the frequency of the 2nd voice vibrato,
//  and the synth volume. The synth will handle POT0 events by calling the
//  setPos() method of the VirtualPotDemux -- which in turn will propagate an 
//  output value to its 4 connected objects. This is an example of how you
//  can control many parameters by changing just one input.
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

about_program( VirtualPotDemux, 1.00 )       // specify sketch name & version

class DemuxSynth : public TwoVoxSynth
{
   typedef TwoVoxSynth super;                // super class is TwoVoxSynth

   StockVoice *voice0;                       // type-cast of vox[0]
   StockVoice *voice1;                       // type-cast of vox[1]

   ParamSend sendVol;                        // controls synth volume
   PotSend   sendDepth0;                     // controls vibrato depth for voice 0
   PotSend   sendDepth1;                     // controls vibrato depth for voice 1
   PotSend   sendFreq1;                      // controls vibrato freq for voice 1

   VirtualPotDemux demux;

   public:

   void welcome()
   {
      voice0 = (StockVoice *)vox[0];         // type cast generic vox[] ptr
      voice1 = (StockVoice *)vox[1];         // type cast generic vox[] ptr

      // Initialize synth voices to play an octave and 1/2 apart in pitch,
      // with their vibratos unmuted

      voice1->xpose = 19;                    // voice 1 is octave+fifth higher

      // turn vibratos on

      voice0->vibrato.setMute( false );
      voice1->vibrato.setMute( false );

      // setup the various send controls (virtual pots)

      sendVol.reset();                      // init
      sendVol.setRange( 128, 255 );         // range from half vol to full vol
      sendVol.connect( this, ptr1ByteMethod( &TwoVoxSynth::setVol ) );

      sendDepth0.reset();
      sendDepth0.setRange( 255, 0 );            
      sendDepth0.connect( &voice0->vibrato, 1 ) ;

      sendDepth1.reset();
      sendDepth1.setRange( 255, 0 );            
      sendDepth1.connect( &voice1->vibrato, 1 ) ;

      sendFreq1.reset();
      sendFreq1.setRange( 20, 200 );            
      sendFreq1.connect( &voice1->vibrato, 0 ) ;

      // connect demux to the sends

      demux.reset();
      demux.connect( 0, &sendVol );
      demux.connect( 1, &sendDepth0 );
      demux.connect( 2, &sendDepth1 );
      demux.connect( 3, &sendFreq1 );

      demux.lineOn( 0 );         
      demux.lineOn( 1 );         
      demux.lineOn( 2 );         
      demux.lineOn( 3 );         
   }

   bool evHandler( obEvent e )               // event handler
   {
      switch ( e.type() )
      {                                      
         case POT0:                          // set demux from POT0 values
         {
            demux.setPos( e.getPotVal() );
            break;
         }
         default:       
            return Synth::evHandler(e);      // pass other events through
      }
      return true;
   }

} myDemuxSynth;


void setup()
{
   ardutouch_setup( &myDemuxSynth );         // initialize ArduTouch resources
}

void loop()
{
   ardutouch_loop();                         // perform ongoing ArduTouch tasks  
}                                             

//  ---------------------------------------------------------------------------
//
//   Play some notes, then try turning the top pot. It will change the synth
//   volume, the depth of both vibratos and the frequency of the 2nd vibrato,
//   all at once, each parameter having its own range.
//
//   --------------------------------------------------------------------------



