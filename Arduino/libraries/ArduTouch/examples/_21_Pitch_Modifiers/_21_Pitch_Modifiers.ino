/*
 * _21_Pitch_Modifiers
 *
 * This example synth demonstrates how to use multiple pitch modifiers on
 * a Voice to create complex pitch effects.
 *
 * It also introduces control chains.
 *
 * In this synth, each time a note is played it will quickly "ramp" up
 * an octave and back down to the original pitch while a vibrato is also 
 * active. 
 *                                NOTE
 *
 *  The Voice class used in this example serves as a base class for more
 *  sophisticated voice classes:
 *
 *          StockVoice <---- ADSRVoice <----- Voice
 *
 *  The differences between these 3 classes is that ADSRVoice adds an ADSR envelope
 *  to Voice, and StockVoice adds a vibrato control to ADSRVoice.
 * 
 *
 *             ##############################################
 *             #######        Control Chains          #######
 *             ##############################################
 *
 * The Voice class contains 3 built-in control "chains", and 3 corresponding
 * methods by which a control can be added to its respective chain:
 * 
 *    class Voice  ...
 *    {
 *       ... lots of other stuff ...
 *      
 *       AmpMods     ampMods;             // chain of amplitude modifiers
 *       PitchMods   pitchMods;           // chain of pitch modifiers
 *       Effects     effects;             // chain of effects
 * 
 *       ... lots of other stuff ...
 *
 *       void  addAmpMod( Factor* );      // add an amplitude modifier  
 *       void  addPitchMod( Factor* );    // add a pitch modifier  
 *       void  addEffect( Effect* );      // add an effect 
 *
 *       ... lots of other stuff ...
 *
 *    } ;
 *
 * In this example we are only interested in the pitchMods chain.
 * The other chains will be explained in latter examples.
 *
 *             ##############################################
 *             #######   PitchMods technical details   ######
 *             ##############################################
 *
 * The PitchMods class is derived from the ControlChain class, which 
 * is in turn derived from the Mode class:
 *
 *    class ControlChain : public Mode    // a chain of controls of type TControl
 *    {
 *       ... lots of other stuff ...
 *      
 *       TControl* chain;                 // pts to 1st control in chain
 *    } ;
 *
 *
 * Notice that the ControlChain class contains a ptr to the first control
 * in the chain. This will be NULL if there are no controls in the chain.
 *
 * TControl is a generic base class for dynamic controls that can be chained 
 * together, and contains a ptr, named 'next', to the next control in the chain 
 * (or NULL if there is no next control):
 *
 *    class TControl : public DynaControl  // a chainable, dynamic control 
 *    {
 *       ... lots of other stuff ...
 *      
 *       TControl *next;                   // ptr to next control in chain
 *
 *    } ;
 *
 * Given this line of code: 
 *
 *    Voice *x = new Voice();
 *
 * then:
 *
 *   ( x->pitchMods.chain == NULL ) is true
 *
 * because there are initially no effects in the pitchMods chain.
 *
 * To add a control to the chain we use Voice::addPitchMod( Factor * )
 *
 * Notice that the argument to addPitchMod() is a pointer to a "Factor".
 *
 * A Factor is a TControl which contains the variable "value" :
 *
 *    class Factor : public TControl 
 *    {
 *        double  value;                   // current output value 
 *    } ;
 *
 *     --------------------------------------------------------------------
 *
 * In summary, the pitchMods object in the Voice class contains a ptr to the 
 * 1st control (if any) in the chain of pitch modulators. Each control in the 
 * pitchMods chain is derived from the Factor base class, which means 
 * that each control in the chain: 
 *
 *    1) has a ptr "next" to the next control (if any) in the chain
 *    2) has a double "value" which contains its current output value
 *
 * The Voice method addPitchMod( Factor * ) is used to add a control to the
 * chain of pitch modulators.
 *
 *                        How Pitch Modulation Works
 *
 * At each dynamic update the system will iterate through all the controls
 * in the pitchMods chain (skipping those that are muted), and multiply all
 * their output values together, and then apply this result to the current
 * frequency of the oscillator, yielding an "instantaneous frequency".
 *
 *   --------------------------------------------------------------------
 *
 *             ##############################################
 *             #######     Accessing pitchMods        #######
 *             ##############################################
 *
 * There are three ways to access the chain of pitch modulators:
 *
 *   1) From the Console, when you are at the voice prompt, type 'P'
 *      You will now be "in" the pitchMods object, with a "Pitch>"
 *      prompt showing on the Console.
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
 *      mode stack), then the following macro will push the pitchMods 
 *      object of voice 0 to the top of the mode stack:
 *
 *                   console.exe( PSTR("0P") );
 *
 *   3) From the ArduTouch keyboard, if the voice is the current mode 
 *      (i.e. the top mode on the mode stack), then:
 *
 *          > double-tap the right button to bring up the voice's menu
 *          > press the D# key to push the pitchMods object
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
 *   4) From the code, explicitly push the pitchMods object onto the 
 *      mode stack. For example, from within a Voice method:
 *
 *                    pushMode( &this->pitchMods );
 *      
 */

#include "ArduTouch.h"                       

about_program( Pitch Modifiers, 1.00 )       

/******************************************************************************
 *
 *                                  Ramp
 *
 * This simple "Ramp" control will oscillate in value from 1.0 to 2.0.
 *
 * We derive it from the library's TermLFO class ("Terminating LFO").
 * The TermLFO class implements a Low Frequency Oscillator that can be 
 * configured to traverse a specific number of half cycles after being
 * triggered and then stop oscillating.
 *
 * (In the "reset" code for our synth further down in this file, we will 
 * initialize our Ramp control so that it traverses 2 half cycles. 
 * This means that it will start at 1.0, move to 2.0, return to 1.0, and 
 * then stop oscillating, each time a note is played).
 *
 ******************************************************************************/

class Ramp : public TermLFO            // ramp frequency effect
{
   typedef TermLFO super;              // superclass is TermLFO

   public:

   // We set the shortcut state variable to 'r'. 
   // This allows us to access our control by typing 'r' 
   // when we are at the "Pitch>" prompt in the Console.

   Ramp() 
   {
      shortcut = 'r';                  
   }

   // The evaluate() method is automatically called by the system at each dynamic
   // update. This is where we set the output value of our control.

   // The LFO state variable "pos", which is referenced here, oscillates between 
   // 0.0 and 1.0 and is automatically computed at each dynamic update before 
   // evaluate() is called.

   void evaluate()
   {
      value = 1.0 + pos;      // value will be between 1.0 and 2.0
   }

   // we set the prompt for our control to "ramp". 
   // this is also the name which will be displayed for our control in the
   // Console when we type '?' at the "Pitch>" prompt.
   // (typing '?' at the "Pitch>" prompt lists all the pitch modifiers).

   #ifdef CONSOLE_OUTPUT
   const char *prompt()
   {
      return CONSTR( "ramp" );
   }
   #endif

} ;

/******************************************************************************
 *
 *                              RamplerVoice
 *
 ******************************************************************************/

// We define a custom voice which contains an added vibrato control, and an
// added ramp control.
//
// An instance of this class will be created via the call-back method newVox()
// in the synth class (see below)

class RamplerVoice : public Voice
{
   typedef Voice super;                // superclass is Voice
   
   public:

   // the following statements declare a vibrato and a ramp control as members
   // of our class

   Vibrato  vibrato;                   // vibrato effect from the library
   Ramp     ramp;                      // custom ramp effect

   RamplerVoice()
   {
      // The following calls add the vibrato and ramp controls
      // to the chain of pitch modulators. (If you skip this
      // step the vibrato and ramp effect will not be updated 
      // by the system, and will not be applied to the oscillator
      // frequency). 

      addPitchMod( &this->vibrato );
      addPitchMod( &this->ramp );
   }

} ;

/******************************************************************************
 *
 *                              RamplerSynth 
 *
 ******************************************************************************/

// Our custom synth uses OneVoxSynth as a base class. This means there will be 
// one voice automatically created at startup time, and a pointer to it will be 
// stored in vox[0]. The vox[] array is a public member of the VoxSynth base 
// class.
//
// By overriding the newVox() call-back method (see below) the system will create 
// an AmpModVoice object in place of the default Voice object.

class RamplerSynth : public OneVoxSynth
{
   typedef OneVoxSynth super;          // superclass is Voice

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

            //       the following macro will

            // start keyboard in octave 6,
            // select voice 0 
            // set envelope release to 100
            // select Pitch Modifiers
            // initialize vibrato control (freq=8hz, depth=.5, time=10)
            // initialize ramp control (freq=2hz, traversal_count=2)

            console.exe( PSTR( "k6\\`"
                               "0"
                               "P"
                               "vf8\\d.5\\t10\\<`"
                               "rf2\\t2\\<`"
                               "``" ) );

            break;
         }
         default:
            return super::charEv( code );
      }
      return true;
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
   
   // The newVox() method is where we can specify a custom voice.
   // Here, we use our RamplerVoice class, with its special pitch
   // controls. (if we didn't write our own newVox() method, the system 
   // default would be used, which is a standard Voice)

   Voice *newVox( byte nth )
   {
      RamplerVoice *v = new RamplerVoice;
      return v;
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

                                             
