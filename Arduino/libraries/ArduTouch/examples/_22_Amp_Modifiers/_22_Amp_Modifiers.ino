/*
 *             ************************************************
 *             *                                              *
 *             *               The Voice Class                *
 *             *                                              *
 *             ************************************************
 *
 * _22_Amp_Modifiers
 *
 * This synth shows how to set up, use, and access multiple amplitude 
 * modifiers for a Voice.
 *
 * In this synth, the amplitude of each note will be modified both by
 * the Voice's built-in envelope and by an added tremolo control. 
 *
 *             ##############################################
 *             #######            ampMods             #######
 *             ##############################################
 *
 * The Voice class has a chain of controls, ampMods, which are used to modify
 * the instantaneous output level ("amplitude") of the voice. The Voice class
 * also has a method, addAmpMod(), for adding a control to the chain of 
 * amplitude modifiers.
 * 
 *    class Voice  ...
 *    {
 *       AmpMods  ampMods;                // chain of amplitude modifiers
 * 
 *       void     addAmpMod( Factor* );   // add an amplitude modifier  
 *
 *       ... lots of other stuff ...
 *
 *    } ;
 *
 *             ##############################################
 *             #######              envAmp            #######
 *             ##############################################
 *
 * The Voice class has a built-in ADSR envelope, named envAmp:
 * 
 *    class Voice  ...
 *    {
 *       Envelope    envAmp;              // built-in envelope
 *
 *       ... lots of other stuff ...
 *      
 *    } ;
 *                                      
 * This envelope is automatically added to the chain of amplitude modifiers 
 * in the Voice constructor:
 *
 *   Voice()
 *   {
 *     addAmpMod( &this->envAmp );
 *
 *       ... other stuff ...
 *   }
 *
 * You can set the envelope's parameters using its public methods, such as:
 *
 *   envAmp.setAttack( 10 );              // set attack time to 10
 *   envAmp.setDecay( 200 );              // set decay time to 200
 *   envAmp.setSustain( 128 );            // set sustain level to 128
 *   envAmp.setRelease( 45 );             // set release time to 45
 *
 * By default the envelope is set to trigger each time a note is played.
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
 *             #######       Accessing ampMods        #######
 *             ##############################################
 *
 * There are four ways to access the chain of amplitude modulators:
 *
 *   1) From the Console, when you are at the voice prompt, type 'A'.
 *      This pushes the ampMods onject to the top of the mode stack.
 *      You will now be "in" the ampMods object, with a "Amp>" prompt 
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
 *      mode stack), then the following macro will push the ampMods 
 *      object of voice 0 to the top of the mode stack:
 *
 *                   console.exe( PSTR("0A") );
 *
 *   3) From the ArduTouch keyboard, if the voice is the current mode 
 *      (i.e. the top mode on the mode stack), then:
 *
 *          > double-tap the right button to bring up the voice's menu
 *          > press the C# key to push the ampMods object
 *
 *      [Note: How you get to the voice mode in the first place
 *             depends on how the synth was constructed. If your
 *             synth is derived from the VoxSynth class then double-
 *             tapping the right button will bring up the synth's menu.
 *             At this point, if your synth is derived from OneVoxSynth,
 *             simply press the C# key. If it is a multi-voice synth
 *             then pressing the C# key brings up a number menu, and
 *             you select the voice you want to push by number (pressing C 
 *             pushes voice 0, pressing C# pushes voice 1, etc.) ]
 *
 *      Please note that keyboard menus are not available if your sketch
 *      is compiled using the __BAREBONE__ runtime model.
 *
 *   4) From the code, explicitly push the ampMods object onto the 
 *      mode stack. For example, from within a Voice method:
 *
 *                    pushMode( &this->ampMods );
 *      
 *             ##############################################
 *             #######  Accessing a specific control  #######
 *             ##############################################
 *
 *   Once you are in ampMods (i.e., the Amp> prompt is displayed in the
 *   Console, which means that ampMods is the current -- top -- mode) you
 *   can access a specific control in the chain by:
 *
 *   1) From the Console, when at the 'Amp>' prompt, type the shortcut for 
 *      the control (usually the 1st letter of its name). For example, in 
 *      this synth, type 'e' to push the envelope control, type 't' to push 
 *      the tremolo control.
 *
 *   2) From the Console, when at the 'Amp>' prompt, type the number (0-based)
 *      for the control which corresponds to its order in the chain. For 
 *      example, in this synth, type '0' to push the envelope control, type '1'
 *      to push the tremolo control. This is the general way to get to any
 *      control. You might have 2 controls with the same shortcut, in fact you
 *      might be using 2 or 3 tremolos on the same voice (which are set to
 *      different frequencies). In this case typing 't' would push the 1st
 *      tremolo. To get to the 2nd and 3rd tremolo you would have to use their
 *      (0-based) ordinal number. Note that the Console displays the controls 
 *      in the order they were added to the chain.
 *
 *   3) From the ArduTouch keyboard, when ampMods is the current mode, double-
 *      tap the right button to bring up the ampMods menu. Now the keys 
 *      correspond to numbers, C = 0, C# = 1, D = 2, etc ... Press the key
 *      that corresponds to the ordinal of the control. For example, in this
 *      synth press C to push the envelope control, press C# to push the
 *      tremolo control.
 *
 *   4) As always anything that can be done by typing at the console can
 *      be incorporated into a macro and executed in the code. For example,
 *      in this synth, executing the following macro while in the Voice mode 
 *      would push the tremolo control:
 *
 *                   console.exe( PSTR("At") );
 *
 *   5) From the code, the control can be addressed directly, as long as you
 *      declare it, or a pointer to it, as a member of your voice. For example,
 *      in this synth:
 *
 *                    tremolo.setDepth( .75 );
 *
 */

#include "ArduTouch.h"                       

about_program( Amp Nodifiers, 1.00 )       

/******************************************************************************
 *
 *                              AmpModVoice
 *
 ******************************************************************************/

// We define a custom voice which contains an added tremolo
//
// An instance of this class will be created via the call-back method newVox()
// in the synth class (see below)

class AmpModVoice : public Voice
{
   typedef Voice super;                // superclass is Voice
   
   public:

   // the following statement declares a tremolo control as a member of
   // this class

   Tremolo  tremolo;                   // tremolo control from the library

   AmpModVoice()
   {
      // The following call adds the tremolo control to the chain 
      // of amplitude modulators. (If you skip this step the tremolo
      // control will not be updated by the system, and will not be 
      // applied to the output level). 

      addAmpMod( &this->tremolo );
   }

} ;

/******************************************************************************
 *
 *                              AmpModSynth 
 *
 ******************************************************************************/

// Our custom synth uses OneVoxSynth as a base class. This means there will be 
// one voice automatically created at startup time, and a pointer to it will be 
// stored in vox[0]. The vox[] array is a public member of the VoxSynth base 
// class.
//
// By overriding the newVox() call-back method (see below) the system will create 
// an AmpModVoice object in place of the default Voice object.

class AmpModSynth : public OneVoxSynth
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

            // start keyboard in octave 5,
            // push voice 0 
            // set envelope (decay=68, sustain=64, release=100)
            // push amdMods
            // initialize tremolo control (freq=2.5hz, depth=1.0, unmute)
            // pop ampMods and pop voice 0

            console.exe( PSTR( "k5\\`"
                               "0"
                               "ed68\\s64\\r100\\`"
                               "A"
                               "tf2.5\\d1\\<`"
                               "``" ) );

            break;
         }
         default:
            return super::charEv( code );
      }
      return true;
   }

   // The newVox() call-back method is where we can specify a custom voice.
   // Here, we use our AmpModVoice class, with its added tremolo control.
   // (if we didn't write our own newVox() method, the system default would 
   // be used, which is a standard Voice.)

   Voice *newVox( byte nth )
   {
      return new AmpModVoice();
   }

} mySynth ;

void setup()
{
   ardutouch_setup( &mySynth );                   
}

void loop()
{
   ardutouch_loop();                            
}                                             

                                             
