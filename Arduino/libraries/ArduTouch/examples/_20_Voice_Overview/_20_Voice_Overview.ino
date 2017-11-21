/*
 *             ************************************************
 *             *                                              *
 *             *               The Voice Class                *
 *             *                                              *
 *             ************************************************
 *
 * _20_Voice_Overview
 *
 * This sketch introduces the Voice class, and constructs a simple one-voice
 * synth with a wave table oscillator using a Sine wave table.
 *
 * The ArduTouch library contains a number of high-level classes which
 * you can use to quickly build customizable, multi-voice synths.
 *
 * There are two base classes which form the foundation for all of these
 * synths:
 *
 *      1) class VoxSynth       - a synth using 1 or more voices
 *      2) class Voice          - a synth voice
 *
 * The VoxSynth class allows you to define a synth with an arbitrary number
 * of voices (up to 8, but in practice around 5 maxes out the CPU.)  In later
 * examples this class will be fully explored. The intention of this and the
 * following sketches is to explain the Voice class. To keep things simple 
 * the example synths will all derive from a class which inherits from VoxSynth:
 *
 *             ##############################################
 *             #######         OneVoxSynth            #######
 *             ##############################################
 *
 * OneVoxSynth is a VoxSynth which has been initialized to use 1 voice.
 *
 *  -------------------------------------------------------------------------
 *
 * The Voice class provides a basic operational framework for an oscillator.
 * It has built-in volume, detuning, portamento and transposition features. 
 * Most importantly it has:
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
 * These chains will be explained in the following sketches.
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
 * When in Voice mode, the buttons do the following:
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

#include "ArduTouch.h"                       

about_program( Voice Overview, 1.00 )       
set_baud_rate( 115200 )                      

/* Our synth in this example is not customized.
 * We simply instantiate a OneVoxSynth object from the library.
 * This will give us a single voice synth.
 * The voice's oscillator wil be of type WaveOsc, using a sine wave table. 
 *
 * At startup the current mode will be the synth itself.
 * Here is a quick survey of some commands that can be typed at the synth>
 * prompt in the console for OneVoxSynth.
 *
 *      0    - push voice 0
 *      k    - push console's virtual keyboard
 *      p    - load a preset (there are no presets in this example)
 *      x    - set note transposition (in semi-tones) for synth
 *      .    - mute synth
 *      <    - unmute synth
 *      !    - reset synth
 *      ?    - display synth info to console
 *      ESC  - exit sketch
 *      `    - exit sketch
 *
 * Here is the OneVoxSynth keyboard menu, which can be brought up by double-
 * tapping the right button:
 *
 *           -------------------------------------------------
 *           |   |   |   |   |   |   |   |   |   |   |   |   |
 *           |   |   |   |   |   |   |   |   |   |   |   |   |
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |   | 0 |   |   |   |   |   |   | k |   | . |   | 
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |    ___     ___    |    ___     ___     ___    | 
 *           |     |       |     |     |       |       |     |
 *           |     |       |     |     |       |       |     |
 *           |  p  |       |     |     |   x   |   <   |  !  |
 *           |     |       |     |     |       |       |     |
 *           |     |       |     |     |       |       |     |
 *           -------------------------------------------------
 *
 */

OneVoxSynth synth;

void setup()
{
   ardutouch_setup( &synth );                   
}

void loop()
{
   ardutouch_loop();                            
}                                             

                                             
