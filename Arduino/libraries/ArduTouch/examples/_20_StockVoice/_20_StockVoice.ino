/*
 *             ************************************************
 *             *                                              *
 *             *            The StockVoice Class              *
 *             *                                              *
 *             ************************************************
 *
 * _20_StockVoice
 *
 * This sketch introduces the StockVoice class. 
 *
 * The ArduTouch library contains a number of high-level classes which
 * you can use to quickly build customizable, multi-voice synths.
 *
 * There are two base classes which form the foundation for most multi-voice synths:
 *
 *      1) StockVoice  - a standard synth voice
 *      2) VoxSynth    - a synth using 1 or more voices
 *
 * If you want just a one voice synth the following class can be used:
 *
 *      3) OneVoxSynth - a synth using exactly 1 voice.
 *
 *                                NOTE
 *
 *  The StockVoice class inherits from other simpler voice classes:
 *
 *          StockVoice <---- ADSRVoice <----- Voice
 *
 *  The differences between these 3 classes is that ADSRVoice adds an ADSR envelope
 *  to Voice, and StockVoice adds a vibrato control to ADSRVoice.
 *
 *  By default VoxSynth instantiates voices of the StockVoice class.
 *
 *  -------------------------------------------------------------------------
 *
 * The StockVoice class provides a basic operational framework for an oscillator.
 * It has built-in volume, detuning, ADSR envelope (for amplitude), vibrato, 
 * portamento and transposition controls. 
 *
 * Most importantly it has:
 *
 *             ##############################################
 *             #######        Control Chains          #######
 *             ##############################################
 *
 * The StockVoice class contains 3 built-in control "chains", and 3 corresponding
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
 * These chains will be explained in the following sketches.
 *
 *********************************************************************************
 *
 *      THE REMAINING SECTIONS ARE QUITE TECHNICAL AND CAN BE SKIPPED 
 *
 *********************************************************************************
 *
 *             ##############################################
 *             #######       Console Interface        #######
 *             ##############################################
 *
 * Here is a quick survey of some of the more important commands that can be 
 * typed into the console when the voice prompt is being displayed (meaning 
 * that the voice is currently the top mode on the mode stack). A command is
 * always one character long.
 *
 *      A    - push the chain of amplitude modifiers
 *      d    - detune oscillator 
 *      e    - push built-in envelope 
 *      E    - push the effects chain
 *      g    - set glide (i.e. portamento) speed 
 *      k    - push console's virtual keyboard (explained further down)
 *      P    - push the chain of pitch modifiers
 *      v    - set volume
 *      V    - push built-in vibrato
 *      x    - set note transposition (in semi-tones) for voice
 *      .    - mute 
 *      <    - unmute
 *      !    - reset
 *      ?    - display voice info to console
 *      ESC  - pop voice (from mode stack)
 *      `    - pop voice (from mode stack)
 *
 *      Note: the built-in envelope is actually a control which has been
 *            added to the chain of amplitude modifiers in the Voice()
 *            constructor. This will be explained in a later sketch.
 * 
 *      Note: the built-in vibrato is actually a control which has been
 *            added to the chain of pitch modifiers in the StockVoice()
 *            constructor. This will be explained in a later sketch.
 * 
 *             ##############################################
 *             #######         Keyboard Menu          #######
 *             ##############################################
 *
 * Many commands can be sent from the ArduTouch keyboard itself. 
 * With the voice as the current mode, double-tapping the right 
 * button will turn the Ardutouch keyboard into a temporary menu
 * which maps keys to some of the voice commands listed above:
 * 
 *           -------------------------------------------------
 *           |   |   |   |   |   |   |   |   |   |   |   |   |
 *           |   |   |   |   |   |   |   |   |   |   |   |   |
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |   | A |   | P |   |   |   |   | k |   | . |   | 
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |    ___     ___    |    ___     ___     ___    | 
 *           |     |       |     |     |       |       |     |
 *           |     |       |     |     |       |       |     |
 *           |  E  |       |  e  |     |   x   |   <   |  !  |
 *           |     |       |     |     |       |       |     |
 *           |     |       |     |     |       |       |     |
 *           -------------------------------------------------
 *
 * With the menu active, pressing a key will send its associated 
 * character to the console (where it will be handled as a command.)
 * For example, pressing D# will push the chain of pitch modifiers.
 *
 *      [Note: How you get to the voice mode in the first place
 *             depends on how the synth is constructed. In a synth
 *             which inherits from OneVoxSynth class, the sketch
 *             will begin with the synth as the current mode.
 *             Double-tapping the right button will bring up the 
 *             synth's menu (see below) then pressing the C# key 
 *             will push the voice.]
 *
 *      Please note that keyboard menus are not available if your sketch
 *      is compiled using the __BAREBONE__ runtime model.
 *
 *             ##############################################
 *             #######         OnBoard Buttons        #######
 *             ##############################################
 *
 * When in StockVoice mode, the buttons do the following:
 *
 *   action        button      action
 *  -----------------------------------------------------------------------
 *   tap           left        bump keyboard down by one octave 
 *   tap           right       bump keyboard up by one octave 
 *   double-tap    left        pop voice from mode stack
 *   double-tap    right       push voice's menu (for one key press)
 *   press         left        scroll pots up 1 parameter (see next section)
 *   press         right       scroll pots down 1 parameter (see next section)
 *
 *             ##############################################
 *             #######          OnBoard Pots          #######
 *             ##############################################
 *
 * The pots control the following parameters:
 *
 *   1) volume level
 *   2) detune amount
 *   3) glide (portamento) speed
 *
 * Since there are more parameters then pots, only 2 parameters are available
 * to be controlled at a time. Pressing the left and right buttons allows you
 * to scroll the parameter list. For example, upon entering the voice mode,
 * the top pot will control the volume level, and the bottom pot will control
 * the detune amount. If you press the right button then the pots will be scrolled
 * down by one parameter, so that the top pot will control the detune amount,
 * and the bottom pot will control the glide speed.
 *
 *             ##############################################
 *             #######    Console Virtual Keyboard    #######
 *             ##############################################
 *
 * The 'k' command in voice mode will push the console's virtual keyboard.
 * The keybrd> prompt will be displayed on the console. The following keys
 * on the host machine's ascii keyboard are then mapped to a musical octave:
 *
 *                      |   |   |   |   |   |   |   |  
 *                      | s | d |   | g | h | j |   |    
 *                      |   |   |   |   |   |   |   |     
 *                    ----------------------------------
 *                    |   |   |   |   |   |   |   |   |
 *                    | z | x | c | v | b | n | m | n |
 *                    |   |   |   |   |   |   |   |   | 
 *                    ----------------------------------
 *
 * Pressing these keys is the same as playing the ArduTouch keyboard.
 * You can also set the octave for the keyboard directly by typing the
 * octave number at the keybrd> prompt.
 *
 * Note: There are other important functions that can be accomplished via
 *       the virtual keyboard which will be explained in a subsequent
 *       sketch that deals with multi-voice synths.
 *
 */

/* 
 * Our synth in this example is not customized.
 * We simply instantiate a OneVoxSynth object from the library.
 * This will give us a single voice synth.
 * The voice's oscillator wil be of type WaveOsc, using a sine wave table. 
 *
 */

#include "ArduTouch.h"                       

about_program( StockVoice, 1.00 )       

OneVoxSynth mySynth;

void setup()
{
   ardutouch_setup( &mySynth );                   
}

void loop()
{
   ardutouch_loop();                            
}                                             

                                             
