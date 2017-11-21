//
//  _07_Dynamics.ino
// 
//  This sketch introduces another important call-back method: dynamics().
//
//  The dynamics() method of your synth is called by the system at regular,
//  timed intervals of approximately 150 times per second. 
//    
//  Dynamic controls and effects include: envelopes, LFOs (see below), and 
//  just about anything that makes a sound interesting. Without dynamics, the
//  output of the synthesizer is static and unchanging.
//
//  You can use dynamic controls built into the ArduTouch library, or develop 
//  you own custom algorithms. To make use of them they should all have their 
//  state updated in dynamics(). We will see how to do this in this example.
//
//  The dynamics() method, together with the output() method, is where "all 
//  the action takes place" in forming the sound of your synth.
//
//  In this example, as an illustration of how to use the dynamics() method we 
//  take the EqualTempSynth of example _06_ and add vibrato to it.
//
//  Vibrato is the effect you hear when a guitarist or violin player or singer
//  repetitively bends a note back and forth.
//
//  Vibrato makes use of an LFO (Low Frequency Oscillator) to modify a note's 
//  frequency.  We can control the rate at which the note's frequency changes, as 
//  well as the "depth" (or amount) of the frequency change.  
//
//  An LFO is one of the basic modules used in many synthesizers, including 
//  analog synthesizers from the 1960s, through modern digital synthesizers.
//  An LFO is usually a low frequency sine wave (about 1Hz), but it can be any 
//  waveform, and run at any speed you want. The ArduTouch uses a triangle wave 
//  at a frequency between 0.01Hz and 20.0Hz.
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

about_program( Dynamics, 1.00 )              // specify sketch name & version
set_baud_rate( 115200 )                      // specify serial baud-rate

class EqualTempSynth : public Synth          // synth from example _06_
{
   public:

   WaveOsc osc;                              // use a wave oscillator 

   void    setup();                          // ... see bottom of file
   boolean evHandler( obEvent e );           //     for implementation
   void    output( char *buf );              //     of these methods

} ;

class VibratoSynth : public EqualTempSynth
{
   public:

   // The ArduTouch library comes with a ready-built Vibrato class.

   Vibrato vib;                              // use a vibrato control

   // The Vibrato class derives from a base class named Control.
   // In our setup code we initialize the vibrato by calling two 
   // methods inherited from Control:
   //
   //    reset()          -- resets the Control to an initial state 
   //    setMute( false ) -- unmutes (or "turns on") the Control

   void setup()
   {
      EqualTempSynth::setup();               // set up synth we inherited
      vib.reset();                           // initialize vibrato
      vib.setMute( false );                  // turn on vibrato  
   }

   // In this example we will take the vibrato's output (public Vibrato::val),
   // which oscillates around a median value of 1.0, and use it to modify the 
   // frequency of our oscillator.  The Vibrato class has a dynamics() method, 
   // which contains code that takes care of modifying its output (::value).

   // The following dynamics() method modifies the osillator frequency 150 times per second.

   void dynamics()                           // perform a dynamic update
   {
      vib.dynamics();                        // update vibrato output value
      osc.modFreq( vib.value );              // apply vibrato to ideal freq
   }

   // Notice that we have applied the vibrato's output to the oscillator 
   // frequency by calling the oscillator's modFreq(x) method. This method
   // takes the oscillator's "ideal" frequency (which is the frequency set 
   // via setFreq()) and multiplies it by the factor x, which is determined 
   // from vib.dynamics().  
   // Although these calculations create 150 discreet values per second, this
   // is fast enough for the result to sound like continuous change in sound.


   // The following event handler allows the use of the pots to alter the 
   // vibrato's frequency and depth.

   boolean evHandler( obEvent e )            // event handler
   {
      switch ( e.type() )                    // branch on event type
      {
         // We will make use of the Vibrato class' pre-written evHandler() method,
         // Vibrato::evHandler(), that uses the top pot to change the vibrato 
         // frequency, and the bottom pot to change the vibrato depth.

         case POT0:                          // top pot was moved
         case POT1:                          // bottom pot was moved

            return vib.evHandler(e);         // pass pot events to vibrato

         // pass all other events to the synth class that we inherited

         default:                            

            return EqualTempSynth::evHandler(e); 
      }
   }

} myVibratoSynth ;                                   


//  --------------------------------------------------------------------------
//
//                 A Note about the Dynamic Update Rate
//
//  When you write your own dynamic effects, to properly calibrate your 
//  algorithms you will need to know the exact rate at which the system calls 
//  your dynamics() method. This rate is given by the system constant:
//
//          double dynaRate;                 // system dynamic update rate
//
//  Later we will see an example on how to do this.
//
//  ---------------------------------------------------------------------------

void setup()
{
   ardutouch_setup( &myVibratoSynth );       // initialize ArduTouch resources
}

void loop()
{
   ardutouch_loop();                         // perform ongoing ArduTouch tasks  
}                                             

/*----------------------------------------------------------------------------*/      

void EqualTempSynth::setup() 
{                                         
   osc.setTable( wavetable(Rood) );          // use Rood wavetable from library
   osc.setFreq( 440.0 );
}

boolean EqualTempSynth::evHandler( obEvent e )  
{
   switch ( e.type() )
   {
      case KEY_DOWN:                         
      {
         key myKey = e.getKey();            // get key from event
         myKey.setOctave( 4 );              // set the key's octave to 4
         osc.setFreq( masterTuning->pitch( myKey ) ); 
         return true;                     
      }
      default:       
         return false;                       
   }
}

void EqualTempSynth::output( char *buf )     // output 1 buffer of audio
{
   osc.output( buf );                        // pass buffer to oscillator
}

//  ---------------------------------------------------------------------------
//
//  If you compile and upload this sketch to the ArduTouch you should hear a 
//  wood-wind like tone at 440 Hz with vibrato. Pressing the keys will play 
//  tones in an equal temperament tuning. Turning the top pot will change
//  the frequency of the vibrato. Turning the bottom pot will change the depth
//  of the vibrato.
//
//  ---------------------------------------------------------------------------



