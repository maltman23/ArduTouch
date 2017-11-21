/*
 *             ************************************************
 *             *                                              *
 *             *               The Voice Class                *
 *             *                                              *
 *             ************************************************
 *
 * _23_Effects
 *
 * This synth demonstrates how to set up, use, and access multiple effects 
 * for a Voice.
 *
 * In this synth, the voice's oscillator output will be run through 2 
 * effects: an autowah and a bit-shift filter (used here for distortion).
 *
 *
 *             ##############################################
 *             #######            effects             #######
 *             ##############################################
 *
 * The Voice class has a chain of controls, effects, which are used to 
 * process the oscillator output. The Voice class also has a method, 
 * addEffect(), for adding a control to the chain of effects.
 * 
 *    class Voice  ...
 *    {
 *       Effects  effects;                // chain of effects
 * 
 *       void     addEffect( Effect* );   // add an effect  
 *
 *       ... lots of other stuff ...
 *
 *    } ;
 *
 *                      How Amplitude Modulation Works
 *
 * As with the controls in pitchMods (see prior tutorial), each control in
 * ampMods is a Factor, meaning that it has an output value -- a public 
 * variable of type double named "value".
 *
 * At each dynamic update the system will iterate through all the controls
 * in the ampMods chain (skipping those that are muted), and multiply all
 * their output values together, and then apply this result to the current
 * volume of the oscillator, yielding an "instantaneous output level".
 *
 *   --------------------------------------------------------------------
 *
 *             ##############################################
 *             #######       Accessing effects        #######
 *             ##############################################
 *
 * There are four ways to access the chain of effects:
 *
 *   1) From the Console, when you are at the voice prompt, type 'E'.
 *      This pushes the effects object to the top of the mode stack.
 *      You will now be "in" the effects object, with a "Effects" prompt 
 *      showing on the Console.
 *
 *      [Note: How you get to the voice prompt in the first place
 *             depends on how the synth was constructed. If your
 *             synth is derived from the VoxSynth class (or a
 *             class that inherits from it) then typing a number
 *             (0-based) at the synth> prompt will push that voice
 *             to the top of mode stack. For example, typing '0'
 *             at the synth> prompt will push the 1st voice.]
 *
 *   2) From the code, execute the equivalent macro. For example, if
 *      your synth derives from a VoxSynth, and you are currently at
 *      the "synth>" prompt (i.e., the synth is the top mode on the
 *      mode stack), then the following macro will push the effects 
 *      object of voice 0 to the top of the mode stack:
 *
 *                   console.exe( PSTR("0E") );
 *
 *   3) From the ArduTouch keyboard, if the voice is the current mode 
 *      (i.e. the top mode on the mode stack), then:
 *
 *          > double-tap the right button to bring up the voice's menu
 *          > press the C key to push the effects object
 *
 *      [Note: How you get to the voice mode in the first place
 *             depends on how the synth was constructed. If your
 *             synth is derived from the VoxSynth class then double-
 *             tapping the right button will bring up the synth's menu.
 *             At this point, if your synth is derived from OneVoxSynth,
 *             simply press the C# key. If it is a multi-voice synth
 *             then pressing the C# key brings up a number menu, and
 *             you select the voice you want by number (pressing C 
 *             pushes voice 0, pressing C# pushes voice 1, etc.) ]
 *
 *      Please note that keyboard menus are not available if your sketch
 *      is compiled using the __BAREBONE__ runtime model.
 *
 *   4) From the code, explicitly push the effects object onto the 
 *      mode stack. For example, from within a Voice method:
 *
 *                    pushMode( &this->effects );
 *      
 *             ##############################################
 *             #######  Accessing a specific control  #######
 *             ##############################################
 *
 *   Once you are in effects (i.e., the Effects prompt is displayed in the
 *   Console, which means that effects is the current -- top -- mode) you
 *   can access a specific control in the chain by:
 *
 *   1) From the Console, when at the 'Effects' prompt, type the shortcut for 
 *      the control (usually the 1st letter of its name). For example, in 
 *      this synth, type 'a' to push the autowah control, type 'b' to push 
 *      the bit-shift filter control.
 *
 *   2) From the Console, when at the 'Effects' prompt, type the number (0-based)
 *      for the control which corresponds to its order in the chain. For 
 *      example, in this synth, type '0' to push the bit-shift filter, type '1'
 *      to push the autowah control. This is the general way to get to any
 *      control. You might have 2 controls with the same shortcut, or perhaps
 *      you are using multiple instances of the same control. In this case 
 *      typing the shortcut for the control would push the 1st instance of that
 *      control in the chain. To get to the 2nd and 3rd instance of the control 
 *      you would need to access them by number. Note that the Console displays 
 *      the controls in the order they were added to the chain.
 *
 *   3) From the ArduTouch keyboard, when effects is the current mode, double-
 *      tap the right button to bring up the effects menu. Now the keys 
 *      correspond to numbers, C = 0, C# = 1, D = 2, etc ... Press the key
 *      that corresponds to the ordinal of the control. For example, in this
 *      synth press C (0) to push the bit-shift filter, or press C# (1) to push 
 *      the autowah.
 *
 *   4) As always anything that can be done by typing at the console can
 *      be incorporated into a macro and executed in the code. For example,
 *      in this synth, executing the following macro while in the Voice mode 
 *      would push the autowah control:
 *
 *                   console.exe( PSTR("Ea") );
 *
 *   5) From the code, the control can be addressed directly, as long as you
 *      declare it, or a pointer to it, as a member of your voice. For example,
 *      in this synth:
 *
 *                    autowah.setDepth( .90 );
 *
 */

#include "ArduTouch.h"                       

about_program( Effects, 1.00 )       
set_baud_rate( 115200 )                      

/******************************************************************************
 *
 *                              EffectVoice
 *
 ******************************************************************************/

// We define a custom voice which contains autowah and bit-shift controls
//
// An instance of EffectVoice will be created via the call-back method newVox()
// in the EffectSynth class (see below)

class EffectVoice : public Voice
{
   typedef Voice super;                // superclass is Voice
   
   public:

   // the following statements declare an autowah control and a bit-shift 
   // filter control as members of this class

   AutoWah  autowah;                   // autowah control from the library
   BSFilter bsfilter;                  // bit-shift filter control from library

   EffectVoice()
   {
      // The following calls add our controls to the chain of effects.
      // (If you skip this step they will not be updated by the system, 
      // and they will not modify tne output of the oscillator). 

      addEffect( &this->bsfilter );
      addEffect( &this->autowah );
   }

} ;

/******************************************************************************
 *
 *                              EffectSynth 
 *
 ******************************************************************************/

// Our custom synth uses OneVoxSynth as a base class. This means there will be 
// one voice automatically created at startup time, and a pointer to it will be 
// stored in vox[0]. The vox[] array is a public member of the VoxSynth base 
// class.
//
// By overriding the newVox() call-back method (see below) the system will create 
// an EffectVoice object in place of the default Voice object.

class EffectSynth : public OneVoxSynth
{
   typedef OneVoxSynth super;          // superclass is OneVoxSynth

   public:

   // We initialize our controls in the reset handler for our synth. 
   //
   // The reset handler is the code which is executed when our synth receives
   // a '!' character event. 
   //
   // Note: ardutouch_setup() resets the synth before returning.
 
   boolean charEv( char code )
   {
      switch ( code )
      {
         case '!':                     // reset
         {
            // perform generic reset of the superclass
            
            super::charEv('!');

            //       the following macro will (line-by-line):

            // start keyboard in octave 2
            // push voice 0 
            // push effects
            // init bit-shift filter (shift=2, clip=1, unmute)
            // init autowah (cutoff=120, lfo freq=7.0, unmute)
            // pop effects and pop voice 0

            console.exe( PSTR( "k2\\`"
                               "0"
                               "E"
                               "bs2\\c1\\<`"
                               "ac120\\lf7\\`<`"
                               "``" ) );
            break;
         }
         default:
            return super::charEv( code );
      }
      return true;
   }

   // The newVox() call-back method is where we can specify a custom voice.
   // Here, we use our EffectVoice class, with its added controls.
   // (if we didn't write our own newVox() method, the system default would 
   // be used, which is a standard Voice.)

   Voice *newVox( byte nth )
   {
      return new EffectVoice();
   }

   // The newOsc() method is where we can specify a custom oscillator.
   // Here, we use a fast wave oscillator with the SynStr wave table.
   // (if we didn't write our own newOsc() method, the system default 
   // would be used, which is a standard WaveOsc with a Sine wave table)

   Osc *newOsc( byte ith )
   {
      FastWaveOsc *o = new FastWaveOsc();
      o->setTable( wavetable( SynStr ) ); 
      return o;
   }

} synth ;

void setup()
{
   ardutouch_setup( &synth );                   
}

void loop()
{
   ardutouch_loop();                            
}                                             

                                             
