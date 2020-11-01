//
//  Hocus.ino
//
//  A synth organ with 4 transposable tone wheels, 29 built-in registrations,
//  2 rotary speaker effects with a simulated Leslie foot-switch and some
//  crazy multi-interval vibratos.
//
//  To get the most out of Hocus be sure to read these header comments first.
//
//  Target:   ArduTouch board
//
//  ---------------------------------------------------------------------------
//
//                ==== Audio Flow Diagram for the Hocus Organ ====
//
//
//     ----------------  ----------------  ----------------  ----------------
//     | Tone Wheel 0 |  | Tone Wheel 3 |  | Tone Wheel 1 |  | Tone Wheel 2 |
//     ----------------  ----------------  ----------------  ----------------
//            |                 |                 |                 |
//            |                 |                 |                 |
//            |                 |                 |                 |
//            v                 v                 v                 v
//         -------------------------          --------------------------
//         | <--- Rotary 0.3 --->  |          | <---  Rotary 1.2 --->  |
//         -------------------------          --------------------------
//              |             |                     |             |
//              |             |                     |             |
//              |             |                     |             |
//              v             v                     v             v
//         -------------------------------------------------------------
//         |   L0            R0                    L1            R1    |
//         |                                                           |
//         |                            Mixer                          |
//         |                                                           |
//         |                  L0 + L1           R0 + R1                |
//         -------------------------------------------------------------
//                               |                 |
//                               |                 |
//                               |    Audio out    |
//                               |                 |
//                               v                 v
//
//  The audio outputs of tone wheels 0 and 3 are fed into Rotary 0.3 (a
//  rotary speaker effect) which dynamically pans them in stereo and sends
//  the stereo output to the Mixer as inputs L0 and R0.
//
//  Likewise, the audio of tone wheels 1 and 2 are fed into Rotary 1.2,
//  which sends its output to the Mixer as inputs L1 and R1.
//
//  The Mixer combines inputs L0 and L1, and inputs R0 and R1, to produce
//  a master stereo signal out.
//
//  ---------------------------------------------------------------------------
//
//                          ====  The Tone Wheel ====
//
//
//  Each Tone Wheel contains a Registration control, a Transpose control,
//  a Vibrato, a Gain, and a Percussive Decay.
//
//
//                R E G I S T R A T I O N                   T R A N S P O S E
//
//   =================================================        ============
//   |                                               |        |  ^   +12 |
//   |                H a r m o n i c s              |        |  .       |
//   |       -------------------------------------   |        |  .    .  |
//   |      |  1   2   3   4   5   6   7   8   9  |  |        |  .       |
//   |      |-------------------------------------|  |        |  .   +6  |
//   |    1 |  .   .       .       .       .      |  |        |  .       |
//   |    2 |  .   .       .       .       .      |  |        |  .    .  |
//   |    3 |  .   .       .       .              |  |        |  .       |
//   |    4 |  .   .       .       .              |  |        |  .    0  |
//   |    5 |  .   .       .                      |  |        |  .       |
//   |    6 |  .   .       .                      |  |        |  .    .  |
//   |    7 |  .   .                              |  |        |  .       |
//   |    8 |  .   .                              |  |        |  .   -6  |
//   |    9 |  .                                  |  |        |  .       |
//   |   10 |  .                                  |  |        |  .    .  |
//   |       -------------------------------------   |        |  .       |
//   |                                               |        |  v   -12 |
//   =================================================        ============
//
//
//                     V I B R A T O                             G A I N
//
//   =================================================        =============
//   |                                               |        |           |
//   |         FREQ                DEPTH             |        |   Level   |
//   |                                               |        |           |
//   |                    FADE                       |        |           |
//   |         --------------------------            |        | Overdrive |
//   |         |   Time   |  In or Out  |            |        | --------- |
//   |         --------------------------            |        | On or Off |
//   |                                               |        |           |
//   |      SHAPE           RANGE        INTERVAL    |        =============
//   |   ------------   -------------   ----------   |
//   |   | Triangle |   |  Positive |   | 1 . 12 |   |          D E C A Y
//   |   | Rising   |   |  Negative |   ----------   |
//   |   | Falling  |   | Pos & Neg |                |        =============
//   |   | Square   |   -------------                |        |           |
//   |   ------------                                |        |   Time    |
//   |                                               |        |           |
//   =================================================        =============
//
//
//  The Registration control is what produces the basic sound of the tone wheel.
//  It mixes the fundamental frequency for the tone with one or more harmonics,
//  each at its own volume level. This is similar to the drawbars found on some
//  electric organs. Unlike electric organs Hocus does not allow you to inter-
//  actively move individual drawbars. Instead it pre-figures them in a number
//  of set registrations which can be selected via a pot (see below). In the
//  diagram above the selected registration mixes the fundamental at 100% with
//  the 2nd harmonic at 80% with the 4th harmonic at 60% with the 6th harmonic
//  at 40% with the 8th harmonic at 20%.
//
//
//  The Transpose control shifts the tone wheel's pitch, in semi-tone increments,
//  from the note played. For example, if the transpose for a given tone wheel is
//  set to +7, then a 'C' played on the keyboard will sound as the 'G' above it.
//
//
//  The Vibrato has the following seven parameters:
//
//    1) FREQ      - frequency of vibrato
//    2) DEPTH     - intensity of vibrato
//    3) FADE TIME - the amount of time over which to fade the vibrato in or out.
//                   (if this value is 0 then the fade is disabled).
//    4) FADE DIR  - direction of fade: fade in or fade out
//    5) SHAPE     - the vibrato waveform:
//
//            triangle wave
//            rising sawtooth
//            falling sawtooth
//            square wave
//
//    6) RANGE     - the range over which the vibrato flucuates:
//
//            positive : flucuate from above the note played to the note played
//            negative : flucuate from below the note played to the note played
//           pos & neg : flucuate from above the note played to below the note played
//
//    7) INTERVAL  - the maximum number of semitones (1 to 12) over which to range
//                   (assuming DEPTH is 100%)
//
//    Note: a "normal" organ vibrato would have a FREQ of around 3, a DEPTH of 25%,
//    a FADE TIME of 0 (meaning no fade), a SHAPE of Triangle, a RANGE of Pos & Neg,
//    and an INTERVAL of 1.
//
//
//  The Gain control adds distortion to the tone. When Overdrive is on, the
//  effect becomes nonlinear :)
//
//
//  If the Time parameter for the Percussive Decay is non-zero then the tone
//  volume will decay to silence over that period of time once played. This
//  mimics the action of percussive stops found on some electric organs.
//
//
//  ---------------------------------------------------------------------------
//
//                            PLAYING THE KEYBOARD
//
//  ---------------------------------------------------------------------------
//
//  Hocus is a mono-touch instrument: you can play one note at a time.
//
//  To raise the keyboard by an octave, tap the right button once.
//  To lower the keyboard by an octave, tap the left button once.
//
//  ---------------------------------------------------------------------------
//
//                                AUTO-SUSTAIN
//
//  ---------------------------------------------------------------------------
//
//  Like a normal organ Hocus will stop sustaining a note once you lift your
//  finger from the key.
//
//  Hocus has an auto-SUSTAIN feature which, when enabled, is like having a
//  sustain pedal constantly depressed. Notes will sustain indefinitely.
//
//  You can stop a sustaining note by pressing it's key again.
//
//  The auto-SUSTAIN feature is enabled and disabled via the Settings Menu
//  (see below).
//
//  ---------------------------------------------------------------------------
//
//                         SIMULATED LESLIE FOOT-SWITCH
//
//  ---------------------------------------------------------------------------
//
//  The classic Leslie rotary speaker came equipped with a dual foot-switch
//  for ramping the rotation speed of the speaker. By stepping on the right
//  foot-switch the rotation of the speaker would gradually accelerate to a
//  fast speed. By stepping on the left foot-switch the rotation of the speaker
//  would gradually decelerate to a slow speed.
//
//  Hocus simulates this foot-switch via the TAP-PRESS gesture on the two
//  on-board buttons. (Think of a tap-press as a double-tap, but with the
//  2nd tap held for alonger period of time).
//
//  To accelerate the speed of the rotary speakers, tap-press the right button
//
//  To decelerate the speed of the rotary speakers, tap-press the left button.
//
//  You can continue playing while the rotary speakers are changing speeds.
//  In fact, it can really liven up the sound if you change the rotation speed
//  back and forth as you play.
//
//  Note: Certain presets (see below) internally restrict the rotary position
//        of the speakers. When these presets are being used the simulated
//        Leslie foot-switch will have little audible effect.
//
//  ---------------------------------------------------------------------------
//
//                     USING THE POTS TO CONTROL PARAMETERS
//
//  ---------------------------------------------------------------------------
//
//  The pots can be used to control 18 different parameters, a pair at a time.
//
//  By pressing (not tapping) the left and right buttons you can change which
//  pair of parameters the pots control. Each time a button is pressed, its
//  associated LED (red LED for left button, blue LED for right button) will
//  change state (from OFF to ON to BLINKING, back to OFF etc.)
//
//  Below is a chart showing which pair of parameters the pots control when
//  the LEDs are in a given state.
//
//  Note: ' before a parameter name indicates that changes to the parameter
//          are applied to the currently SELECTED tone wheel only. The
//          selected tone wheel can be changed via the Settings Menu (see
//          below).
//
//
//     red LED    blue LED          top POT                 bot POT
//     -------    --------     ------------------     -------------------
//       OFF         OFF         'REGISTRATION            'TRANSPOSE
//        ON         OFF            'VOLUME                 'DETUNE
//     BLINK         OFF       'percussive DECAY       'vibrato INTERVAL
//       OFF          ON        'vibrato FREQ           'vibrato DEPTH
//        ON          ON        'vibrato SHAPE**       'vibrato RANGE***
//     BLINK          ON        'vibrato TIME          'vibrato FADE****
//       OFF       BLINK         'gain LEVEL           'gain OVERDRIVE*
//        ON       BLINK        rotary 0.3 FREQ        rotary 0.3 DEPTH
//     BLINK       BLINK        rotary 1.2 FREQ        rotary 1.2 DEPTH
//
//    ' changes are applied to the SELECTED tone wheel only
//
//    * left-of-middle = overdrive OFF, right-of-middle = overdrive ON
//
//    ** from left to right, in 45-degree increments:
//       Triangle / Rising Sawtooth / Falling Sawtooth / Square.
//
//    *** from left to right, in 60-degree increments:
//        positive and negative / positive / negative
//
//    **** left-of-middle = FADE In, right-of-middle = FADE Out
//
//  ---------------------------------------------------------------------------
//
//                            ENSEMBLE vs ISO OUTPUT
//
//  ---------------------------------------------------------------------------
//
//  Hocus normally outputs audio from all 4 tone wheels in stereo as depicted
//  in the Audio Flow Diagram.
//
//  It is also possible to output the audio of just the SELECTED tone wheel,
//  bypassing the rotary effects and mixer. This is called ISO output.
//
//  ISO output is useful when you want to hear exactly how edits to a
//  tone wheel's parameters effect its sound, without being distracted by
//  the output from the other tone wheels.
//
//  ENSEMBLE and ISO output are set via the Settings Menu (see below).
//
//  ENSEMBLE output is automatically restored when a preset is loaded.
//
//  ---------------------------------------------------------------------------
//
//                           USING THE SETTINGS MENU
//
//  ---------------------------------------------------------------------------
//
//  You can change various aspects of Hocus by using the Settings Menu.
//
//  To access the Settings Menu double-tap the left button. The LEDs will now
//  start blinking on and off in alternation: this tells you that the Settings
//  Menu is active. When the Settings Menu is active the keys will no longer
//  play notes but each key will choose a particular setting as described below.
//  The Settings Menu is active until you press a key on the keyboard, after
//  which the Settings Menu is automatically exited (restoring the prior user
//  interface state).
//
//  You can exit the Settings Menu without choosing a setting by double-
//  tapping the left button a second time.
//
//  The list below describes what actions are taken when a key on the keyboard
//  is pressed:
//
//      Key   Action
//      ---   -----------------------------------------------------------
//
//       C    SELECT tone wheel 0
//       D    SELECT tone wheel 1
//       E    SELECT tone wheel 2
//       F    SELECT tone wheel 3
//       G    SUSTAIN  On
//       A    SUSTAIN  Off
//       A#   ISO      output
//       B    ENSEMBLE output
//
//
//  ---------------------------------------------------------------------------
//                           Guide to Hocus Presets
//  ---------------------------------------------------------------------------
//
//   Load presets by double-tapping the right button and then pressing any key
//   on the keyboard.
//
//   The presets are as follows:
//
//      Key   Name        Description
//      ---   ----        ----------------------------------------------------
//       C    Stock       stock "simple" organ sound
//       C#   Rocker*     distortion on lower 2 tone wheels
//       D    Perc        percussive decay on tone wheels 2 & 3
//       D#   Friction*   overdriven gain and octave-widening vibrato on tone wheels 0 & 3
//       E    Pipe        multiple registrations
//       F    Punchy      gain on tone wheel 2; 7 Hz vibrato on tone wheel 3
//       F#   Pipe2       multiple registrations
//       G    Murk        dark intervals; slow rising-sawtooth vibrato fading out on tone wheel 2
//       G#   Farfisa*    cheezy vibrato; registrations use a mix of odd and even harmonics
//       A    Perc2       percussive decay on tone wheels 2 & 3
//       A#   GiddyUp     fun with square-wave vibratos
//       B    area51      detuned tones and opposing saw-tooth vibratos with minor-3rd ranges
//
//    * restricted rotary positions: Leslie foot-switch has little effect
//
//  ---------------------------------------------------------------------------
//
//  Copyright (C) 2020, Cornfield Electronics, Inc.
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

#include "ArduTouch.h"

about_program( Hocus, 1.00 )

//
// If you've interested in hacking this sketch and want to use the console
// uncomment the following line. This will help you move between __FULLHOST__
// and __STNDLONE__ runtime models without generating compiler errors -- but
// you will need to use Build 1.6.6 of the Arduino IDE (otherwise the binary
// will be too large) and uncomment the "//#define BUILD_166" statement in
// Model.h of the library.
//
// NOTE: the "area51" preset will not sound quite right when DEVELOPER mode
//       is enabled because there isn't enough space in the ROM to fit all
//       of the sinusoid wavetables.
//

//#define DEVELOPER                       // for developers only!

// Make sure we're in the right runtime model / build ...

#ifdef DEVELOPER

   #ifdef CONSOLE_OUTPUT
      #ifndef BUILD_166
         #error This sketch requires BUILD 1.6.6 for the __FULLHOST__ runtime model (Model.h)
      #endif
   #endif

#else

   #ifndef __STNDLONE__
      #error This sketch requires the __STNDLONE__ runtime model (Model.h)
   #endif

#endif

/******************************************************************************
 *
 *                               Presets
 *
 ******************************************************************************/

// The following symbolic constants are equal to the indexes of their respective
// sinusoids within the wavetables bank below. They are provided as an aid during
// development so that preset macros can refer to them symbolically and do not
// have to be rewritten when the order of the sinusoids in the wavetables bank
// is altered.

#ifdef CONSOLE_OUTPUT               // limited set of sinusoids ("0" means use Sine)

   #define _Sine    "0"
   #define _Sn2     "1"
   #define _Sn3b    "0"
   #define _Sn5b    "0"
   #define _Sn6b    "0"
   #define _Sn23    "2"
   #define _Sn24    "3"
   #define _Sn26    "4"
   #define _Sn34    "5"
   #define _Sn35    "0"
   #define _Sn39    "0"
   #define _Sn46    "0"
   #define _Sn56    "0"
   #define _Sn234   "6"
   #define _Sn235   "7"
   #define _Sn245   "8"
   #define _Sn268   "9"
   #define _Sn345   "10"
   #define _Sn357   "11"
   #define _Sn368   "12"
   #define _Sn468   "13"
   #define _Sn2345  "0"             // -- this is used by area51 preset
   #define _Sn2346  "14"
   #define _Sn2468  "15"
   #define _Sn2568  "16"
   #define _Sn3456  "17"
   #define _Sn3578  "0"
   #define _Sn4567  "0"
   #define _Sn5678  "0"

#else                               // load full set of sinusoids

   #define _Sine    "0"
   #define _Sn2     "1"
   #define _Sn3b    "2"
   #define _Sn5b    "3"
   #define _Sn6b    "4"
   #define _Sn23    "5"
   #define _Sn24    "6"
   #define _Sn26    "7"
   #define _Sn34    "8"
   #define _Sn35    "9"
   #define _Sn39    "10"
   #define _Sn46    "11"
   #define _Sn56    "12"
   #define _Sn234   "13"
   #define _Sn235   "14"
   #define _Sn245   "15"
   #define _Sn268   "16"
   #define _Sn345   "17"
   #define _Sn357   "18"
   #define _Sn368   "19"
   #define _Sn468   "20"
   #define _Sn2345  "21"
   #define _Sn2346  "22"
   #define _Sn2468  "23"
   #define _Sn2568  "24"
   #define _Sn3456  "25"
   #define _Sn3578  "26"
   #define _Sn4567  "27"
   #define _Sn5678  "28"

#endif

define_preset( Empty,      "" )
define_preset( Stock,      "S" )

define_preset( Pipe,       "S0Ows"_Sn24"\\``1Ows"_Sn2568"\\``2Ows"_Sn2468"\\``3Ows"_Sn234"\\``"
                           "Pf.75\\d60\\`Xf1.25\\d68\\`" )

define_preset( Pipe2,      "S0Ows"_Sn34"\\``1Ows"_Sn23"\\``2x-5\\Ows"_Sn2468"\\``3x4\\Ows"_Sn234"\\``"
                           "Pf.6\\d64\\`Xf1.5\\d64\\`"  )

define_preset( Rocker,     "S0d0\\Ows"_Sn2468"\\`V<f2.5\\d12\\t78\\`G<g1.5\\``"
                           "1x-5\\Ows"_Sn26"\\`V<f3\\t72\\`G<g3.47\\``"
                           "2d-21\\Ows"_Sn24"\\``3x0\\Ows"_Sn245"\\``"
                           "Pf.2\\d100\\P40\\p`Xf.5\\d20\\P200\\p`" )

define_preset( Friction,   "S0VQm7\\<d128\\t100\\`G<af\\g8\\``"
                           "1Ows"_Sn3456"\\``"
                           "2v225\\x-5\\Ows"_Sn2568"\\``"
                           "3VQm12\\<d128\\t150\\`G<af\\g5.4\\``"
                           "Pf3.8\\d112\\`X.P0\\p`" )

define_preset( Farfisa,    "S0x0\\d0\\Ows"_Sn23"\\`V<f4\\``"
                           "1Ows"_Sn34"\\`V<f4\\``"
                           "2d0\\Ows"_Sn3456"\\`V<f6\\d24\\``"
                           "3x12\\v178\\Ows"_Sn3456"\\`V<f6\\d12\\``"
                           "P.pP64\\`X.pP96\\`" )

define_preset( Punchy,     "Sx-12\\1v160\\d12\\Ows"_Sn357"\\``"
                           "2Ows"_Sn2346"\\`V<Rrp\\m4\\t2\\-`G<g1.25\\``"
                           "3x12\\Ows"_Sn2568"\\`V<f7\\d16\\t12``"
                           "Pf6\\d96\\`Xf3\\d16\\`" )

define_preset( GiddyUp,    "S0Ows"_Sn234"\\`VQ-<f5\\d128\\t20\\rn\\m5\\``"
                           "1Ows"_Sn368"\\`V<f3.75\\t30\\``"
                           "2v160\\Ows"_Sn268"\\`VQ<f2.5\\d128\\t4\\rp\\m7\\``"
                           "3x0\\v216\\Ows"_Sn345"\\`VQ<f5\\d128\\t8\\rp\\m12\\``"
                           "Pf.6275\\D96\\Xf1.25\\D78\\`" )

define_preset( Perc,       "S"
                           "1Ows"_Sn23"\\``"
                           "2Ows"_Sn234"\\`D<t143\\``"
                           "3Ows"_Sn2"\\`D<``"
                           "Pd64\\`Xf1\\d96\\`" )

define_preset( Perc2,      "S0Ows"_Sn234"\\``"
                           "1x-5\\Ows"_Sn23"\\``"
                           "2x-5\\Ows"_Sn368"\\`D<t57\\``"
                           "3x-5\\Ows"_Sn268"\\`D<``"
                           "Pd64\\`Xf1\\d96\\`" )

define_preset( Murk,       "S0Ows"_Sn2346"\\``"
                           "1x-6\\Ows"_Sn468"\\`V<t160\\``"
                           "2x6\\Ows"_Sn2468"\\`VR-<f.3\\t10\\m12\\``"
                           "3x-2\\Ows"_Sn268"\\``"
                           "Pf.75\\d128\\`Xf.6\\d96\\`" )

define_preset( area51,     "S0x0\\d-118\\Ows"_Sn234"\\`V<t3\\Fd128\\rn\\m3\\f6\\``"
                           "1x-1\\Ows"_Sn234"\\`V<t3\\Fd128\\rn\\m3\\f3\\``"
                           "2x1\\d-35\\Ows"_Sn234"\\`V<t3\\Rd128\\rp\\m3\\f6\\``"
                           "3x0\\d127\\Ows"_Sn2345"\\`V<t3\\Rd128\\rp\\m3\\f.75\\``"
                           "Pf1\\d120\\`Xf2\\d126\\`" )

begin_bank( myPresets )

   _preset( Stock )                       // C
   _preset( Rocker )                      // C#
   _preset( Perc )                        // D
   _preset( Friction )                    // D#
   _preset( Pipe )                        // E
   _preset( Punchy )                      // F
   _preset( Pipe2 )                       // F#
   _preset( Murk )                        // G
   _preset( Farfisa )                     // G#
   _preset( Perc2 )                       // A
   _preset( GiddyUp )                     // A#
   _preset( area51 )                      // B

end_bank()


/******************************************************************************
 *
 *                              WaveBank
 *
 ******************************************************************************/

begin_bank( wavetables )            // "registrations"

   #ifdef CONSOLE_OUTPUT            // limited set of sinusoids

   _wavetable( Sine )               // 1 - 100%
   _wavetable( Sn2  )               // 1 - 100%  2 - 40%
   _wavetable( Sn23 )               // 1 - 100%  2 - 30%  3 - 15%
   _wavetable( Sn24 )               // 1 - 100%  2 - 25%  4 - 12%
   _wavetable( Sn26 )               // 1 - 100%  2 - 20%  6 - 40%
   _wavetable( Sn34 )               // 1 - 100%  3 - 25%  4 - 16%
   _wavetable( Sn234 )              // 1 - 100%  2 - 25%  3 - 40%  4 - 08%
   _wavetable( Sn235 )              // 1 - 100%  2 - 40%  3 - 20%  5 - 10%
   _wavetable( Sn245 )              // 1 - 100%  2 - 25%  4 - 40%  5 - 08%
   _wavetable( Sn268 )              // 1 -  50%  2 - 80%  6 - 20%  8 - 12%
   _wavetable( Sn345 )              // 1 - 100%  3 - 25%  4 - 40%  5 - 08%
   _wavetable( Sn357 )              // 1 - 100%  2 - 60%  5 - 40%  7 - 20%
   _wavetable( Sn368 )              // 1 -  50%  3 - 80%  6 - 20%  8 - 12%
   _wavetable( Sn468 )              // 1 - 100%  4 - 25%  6 - 40%  8 - 12%
   _wavetable( Sn2346 )             // 1 - 100%  2 - 40%  3 - 24%  4 - 20%  6 - 12%
   _wavetable( Sn2468 )             // 1 - 100%  2 - 80%  4 - 60%  6 - 40%  8 - 20%
   _wavetable( Sn2568 )             // 1 - 100%  2 - 24%  5 - 12%  6 - 16%  8 - 08%
   _wavetable( Sn3456 )             // 1 - 100%  3 - 33%  4 - 25%  5 - 33%  6 - 50%

   #else                            // full set of sinusoids

   _wavetable( Sine )               // 1 - 100%
   _wavetable( Sn2  )               // 1 - 100%  2 - 40%
   _wavetable( Sn3b )               // 1 - 100%  3 - 20%
   _wavetable( Sn5b )               // 1 - 100%  5 - 20%
   _wavetable( Sn6b )               // 1 - 100%  6 - 20%
   _wavetable( Sn23 )               // 1 - 100%  2 - 30%  3 - 15%
   _wavetable( Sn24 )               // 1 - 100%  2 - 25%  4 - 12%
   _wavetable( Sn26 )               // 1 - 100%  2 - 20%  6 - 40%
   _wavetable( Sn34 )               // 1 - 100%  3 - 25%  4 - 16%
   _wavetable( Sn35 )               // 1 - 100%  3 - 25%  5 - 12%
   _wavetable( Sn39 )               // 1 - 100%  3 - 12%  9 - 24%
   _wavetable( Sn46 )               // 1 - 100%  4 - 25%  6 - 12%
   _wavetable( Sn56 )               // 1 - 100%  5 - 25%  6 - 16%
   _wavetable( Sn234 )              // 1 - 100%  2 - 25%  3 - 40%  4 - 08%
   _wavetable( Sn235 )              // 1 - 100%  2 - 40%  3 - 20%  5 - 10%
   _wavetable( Sn245 )              // 1 - 100%  2 - 25%  4 - 40%  5 - 08%
   _wavetable( Sn268 )              // 1 -  50%  2 - 80%  6 - 20%  8 - 12%
   _wavetable( Sn345 )              // 1 - 100%  3 - 25%  4 - 40%  5 - 08%
   _wavetable( Sn357 )              // 1 - 100%  2 - 60%  5 - 40%  7 - 20%
   _wavetable( Sn368 )              // 1 -  50%  3 - 80%  6 - 20%  8 - 12%
   _wavetable( Sn468 )              // 1 - 100%  4 - 25%  6 - 40%  8 - 12%
   _wavetable( Sn2345 )             // 1 - 100%  2 - 40%  3 - 60%  4 - 20%  5 - 30%
   _wavetable( Sn2346 )             // 1 - 100%  2 - 40%  3 - 24%  4 - 20%  6 - 12%
   _wavetable( Sn2468 )             // 1 - 100%  2 - 80%  4 - 60%  6 - 40%  8 - 20%
   _wavetable( Sn2568 )             // 1 - 100%  2 - 24%  5 - 12%  6 - 16%  8 - 08%
   _wavetable( Sn3456 )             // 1 - 100%  3 - 33%  4 - 25%  5 - 33%  6 - 50%
   _wavetable( Sn3579 )             // 1 - 100%  3 - 80%  5 - 60%  7 - 40%  9 - 20%
   _wavetable( Sn4567 )             // 1 - 100%  4 - 40%  5 - 60%  6 - 20%  7 - 30%
   _wavetable( Sn5678 )             // 1 - 100%  5 - 40%  6 - 60%  7 - 20%  8 - 30%

   #endif

/* list of all sinusoids in library:

   _wavetable( Sine )               // 1 - 100%
   _wavetable( Sn2  )               // 1 - 100%  2 - 40%
   _wavetable( Sn2b )               // 1 - 100%  2 - 20%
   _wavetable( Sn3  )               // 1 - 100%  3 - 40%
   _wavetable( Sn3b )               // 1 - 100%  3 - 20%
   _wavetable( Sn4  )               // 1 - 100%  4 - 40%
   _wavetable( Sn4b )               // 1 - 100%  4 - 20%
   _wavetable( Sn5  )               // 1 - 100%  5 - 40%
   _wavetable( Sn5b )               // 1 - 100%  5 - 20%
   _wavetable( Sn6  )               // 1 - 100%  6 - 40%
   _wavetable( Sn6b )               // 1 - 100%  6 - 20%
   _wavetable( Sn8  )               // 1 - 100%  8 - 40%
   _wavetable( Sn8b )               // 1 - 100%  8 - 20%
   _wavetable( Sn23 )               // 1 - 100%  2 - 30%  3 - 15%
   _wavetable( Sn24 )               // 1 - 100%  2 - 25%  4 - 12%
   _wavetable( Sn24b )              // 1 - 100%  2 - 50%  4 - 25%
   _wavetable( Sn25 )               // 1 - 100%  2 - 25%  5 - 16%
   _wavetable( Sn26 )               // 1 - 100%  2 - 20%  6 - 40%
   _wavetable( Sn34 )               // 1 - 100%  3 - 25%  4 - 16%
   _wavetable( Sn35 )               // 1 - 100%  3 - 25%  5 - 12%
   _wavetable( Sn35b )              // 1 - 100%  3 - 50%  5 - 25%
   _wavetable( Sn36 )               // 1 - 100%  3 - 18%  6 - 24%
   _wavetable( Sn38 )               // 1 - 100%  3 - 12%  8 - 24%
   _wavetable( Sn39 )               // 1 - 100%  3 - 12%  9 - 24%
   _wavetable( Sn45 )               // 1 - 100%  4 - 25%  5 - 16%
   _wavetable( Sn46 )               // 1 - 100%  4 - 25%  6 - 12%
   _wavetable( Sn56 )               // 1 - 100%  5 - 25%  6 - 16%
   _wavetable( Sn234 )              // 1 - 100%  2 - 25%  3 - 40%  4 - 08%
   _wavetable( Sn235 )              // 1 - 100%  2 - 40%  3 - 20%  5 - 10%
   _wavetable( Sn245 )              // 1 - 100%  2 - 25%  4 - 40%  5 - 08%
   _wavetable( Sn258 )              // 1 - 100%  2 - 20%  5 - 30%  8 - 20%
   _wavetable( Sn267 )              // 1 - 100%  2 - 25%  6 - 40%  7 - 08%
   _wavetable( Sn268 )              // 1 -  50%  2 - 80%  6 - 20%  8 - 12%
   _wavetable( Sn345 )              // 1 - 100%  3 - 25%  4 - 40%  5 - 08%
   _wavetable( Sn346 )              // 1 - 100%  3 - 60%  4 - 40%  6 - 20%
   _wavetable( Sn456 )              // 1 - 100%  4 - 25%  5 - 40%  6 - 08%
   _wavetable( Sn468 )              // 1 - 100%  4 - 25%  6 - 40%  8 - 12%
   _wavetable( Sn357 )              // 1 - 100%  2 - 60%  5 - 40%  7 - 20%
   _wavetable( Sn368 )              // 1 -  50%  3 - 80%  6 - 20%  8 - 12%
   _wavetable( Sn2345 )             // 1 - 100%  2 - 40%  3 - 60%  4 - 20%  5 - 30%
   _wavetable( Sn2346 )             // 1 - 100%  2 - 40%  3 - 24%  4 - 20%  6 - 12%
   _wavetable( Sn2468 )             // 1 - 100%  2 - 80%  3 - 60%  4 - 40%  5 - 20%
   _wavetable( Sn2479 )             // 1 - 100%  2 - 33%  4 - 20%  7 - 40%  9 - 12%
   _wavetable( Sn2568 )             // 1 - 100%  2 - 24%  5 - 12%  6 - 16%  8 - 08%
   _wavetable( Sn3456 )             // 1 - 100%  3 - 33%  4 - 25%  5 - 33%  6 - 50%
   _wavetable( Sn3579 )             // 1 - 100%  3 - 80%  5 - 60%  7 - 40%  9 - 20%
   _wavetable( Sn4567 )             // 1 - 100%  4 - 40%  5 - 60%  6 - 20%  7 - 30%
   _wavetable( Sn5678 )             // 1 - 100%  5 - 40%  6 - 60%  7 - 20%  8 - 30%

*/

end_bank()


/******************************************************************************
 *
 *                                Decay
 *
 ******************************************************************************/

class Decay : public Factor
{
   typedef Factor super;         // superclass is Factor

   public:

   Decay()
   {
      shortcut = 'd';
   }

   boolean charEv( char );       // process a character event
   void    dynamics();           // update object dynamics
   boolean evHandler( obEvent ); // handle an onboard event

   byte    getTime()   { return time; }    // get decay time

   void    setTime( byte );     // set decay time

   PROMPT_STR( decay )

   protected:

   byte    time;                 // decay time

   double  decStep;              // change in decay per dynamic update

} ;

/*----------------------------------------------------------------------------*
 *
 *  Name:  Decay::charEv
 *
 *  Desc:  Process a character event.
 *
 *  Args:  code             - character to process
 *
 *  Memb: +flags.DONE       - control is ready to be triggered
 *        +time             - decay time
 *        +value            - current output value
 *
 *  Rets:  status           - true if character was handled
 *
 *----------------------------------------------------------------------------*/

bool Decay::charEv( char key )
{
   switch ( key )
   {
      case chrTrigger:                 // trigger the decay

         flags &= ~DONE;
         value = 1.0;
         break;

      #ifdef INTERN_CONSOLE

      case 't':                        // set decay time

         console.getByte( CONSTR("time"), &time );
         setTime( time );
         break;

      #endif

      #ifdef CONSOLE_OUTPUT

      case chrInfo:                    // display object info to console

         super::charEv( chrInfo );
         console.infoByte( CONSTR("time"), time );
         console.infoDouble( CONSTR("decStep"), decStep );
         break;

      #endif

      case '.':                        // mute
      case '<':                        // unMute

         super::charEv( key );
         value = 1.0;
         break;

      case '!':                        // reset

         super::charEv( key );
         value = 1.0;
         flags |= DONE;
         setTime(32);
         break;

      default:

         return super::charEv( key );
   }
   return true;
}


/*----------------------------------------------------------------------------*
 *
 *  Name:  Decay::dynamics
 *
 *  Desc:  Perform a dynamic update.
 *
 *  Memb:  decStep          - change in decay per dynamic update
 *        +value            - current output value
 *
 *----------------------------------------------------------------------------*/

void Decay::dynamics()
{
   if ( (flags&(MUTE|DONE)) != 0 )
      return;

   value -= decStep;
   if ( value < 0.0 )
   {
      value = 0.0;
      flags |= DONE;
   }
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Decay::evHandler
 *
 *  Desc:  Handle an onboard event.
 *
 *  Args:  ev               - onboard event
 *
 *  Rets:  status           - true if the event was handled
 *
 *----------------------------------------------------------------------------*/

boolean Decay::evHandler( obEvent ev )
{
   switch ( ev.type() )
   {
      case POT0:

         setTime( ev.getPotVal() );
         return true;

      default:

         return super::evHandler(ev);
   }
}

void Decay::setTime( byte val )
{
   time = val;
   if ( val )
      decStep = 1.0 / val;
   else
      decStep = 0.0;
}


/******************************************************************************
 *
 *                                MaxiVibrato
 *
 ******************************************************************************/

#define MAX_MULT 12

class MaxiVibrato : public Vibrato
{
   typedef Vibrato super;              // superclass is Vibrato

   protected:

   static const double ratio[];        // freq ratio per # semitones
   static const double invratio[];     // 1/ratio[]

   typedef byte vibRange ; enum
   {
      rangePOS,
      rangeNEG,
      rangeBOTH
   } ;

   vibRange range;                     // vibrato range (pos/neg/pos-and-neg)
   byte     multiple;                  // multiply normal vibrato range by this many semi-tones

   char     desiredSaw;                // 'F' or 'R'

   public:

   bool charEv( char code )
   {
      switch ( code )
      {
         #ifdef INTERN_CONSOLE

         case 'R':                     // invert saw-tooth on positive range to get expected results

            desiredSaw = code;
            if ( range == rangePOS )
               super::charEv('F');
            else
               super::charEv('R');
            break;

         case 'F':                     // invert saw-tooth on positive range to get expected results

            if ( range == rangePOS )
               super::charEv('R');
            else
               super::charEv('F');
            break;

         case 'm':                     // set range multiple (in semi-tones)
         {
            byte val;
            if ( console.getByte( CONSTR("multiple"), &val ) )
               setMultiple( val );
            break;
         }

         case 'r':
         {
            char val;
            if ( console.getChar( CONSTR("range"), &val ) )
            switch ( val )
            {
               case 'p':
                  setRange( rangePOS );
                  break;
               case 'n':
                  setRange( rangeNEG );
                  break;
               case 'b':
                  setRange( rangeBOTH );
                  break;
               default:
                  break;
            }
            break;
         }

         #endif

         #ifdef CONSOLE_OUTPUT

         case chrInfo:                 // display object info to console

            super::charEv( code );
            console.newlntab();
            console.infoChar( CONSTR("range"),
                              range == rangePOS ? 'p' : ( range == rangeNEG ? 'n' : 'b' ) );
            console.infoByte( CONSTR("multiple"), multiple );
            break;

         #endif

         case '!':                     // perform a reset

            super::charEv( code );
            setRange( rangeBOTH );
            setMultiple(1);
            break;

         default:

            return super::charEv( code );
      }
      return true;
   }

   void evaluate()                     // compute output value from osc position
   {
      FadeLFO::evaluate();

      double scale;

      if ( ! isSigned() )
         value = 1.0 - value;

      if ( range == rangeNEG )
      {
         scale  = pgm_read_float_near(invratio + multiple);
         value *= (scale - 1.0);
      }
      else if ( value >= 0.0 )
      {
         scale = pgm_read_float_near(ratio + multiple);
         value *= (scale - 1.0);
      }
      else
      {
         scale = pgm_read_float_near(invratio + multiple);
         value *= (1.0 - scale);
      }

      value += 1.0;
   }

   void setMultiple( byte m )
   {
      if ( m < 1 )         m = 1;
      if ( m > MAX_MULT )  m = MAX_MULT;

      multiple = m;
   }

   void setRange( vibRange r )
   {
      switch ( r )
      {
         case rangePOS:

            setSigned( false );
            switch ( getWaveform() )      // invert saw-tooth on positive range to get expected results
            {
               case LFO_RSAW_WF:
               case LFO_FSAW_WF:
                  super::charEv( desiredSaw == 'F' ? 'R' : 'F' );
            }
            break;

         case rangeNEG:                  // insure saw-tooth is not inverted

            setSigned( false );
            switch ( getWaveform() )
            {
               case LFO_RSAW_WF:
               case LFO_FSAW_WF:
                  super::charEv( desiredSaw );
            }
            break;

         case rangeBOTH:

            setSigned( true );
            break;
      }
      range = r;
   }

};


const double MaxiVibrato::ratio[] PROGMEM =
{
   1.000000, 1.059463, 1.122461, 1.189206,
   1.259920, 1.334839, 1.414212, 1.498306,
   1.587399, 1.681791, 1.781795, 1.887746,
   2.000000
} ;

const double MaxiVibrato::invratio[] PROGMEM =
{
   1.000000, 0.943874, 0.890898, 0.840895,
   0.793699, 0.749152, 0.707105, 0.667418,
   0.629958, 0.594601, 0.561229, 0.529729,
   0.500000
} ;


/******************************************************************************
 *
 *                                ToneWheel
 *
 ******************************************************************************/

class ToneWheel : public TabOsc256
{
   typedef TabOsc256 super;               // superclass is TabOsc256

   public:

   const char* name;                      // wavetable name

   bool charEv( char code )
   {
      switch ( code )
      {
         #ifdef INTERN_CONSOLE

         case 'w':                        // select a waveform (registration)

            if ( wavebank.choose() )
               setWaveform( wavebank.choice() );
            break;

         #endif

         #ifdef CONSOLE_OUTPUT

         case chrInfo:                 // display object info to console
         case chrInLnfo:               // display object info inline to console

            super::charEv( code );
            if ( name )
               console.infoStr( CONSTR("waveform"), name );
            break;

         #endif

         default:

            return super::charEv( code );
      }
      return true;
   }

   bool evHandler( obEvent ev )
   {
      switch ( ev.type() )
      {
         case POT0:                    // select a waveform (registration)
         {
            word nth = ev.getPotVal() * wavebank.numMembers() / 256;
            setWaveform( nth );
            break;
         }
         default:
            return super::evHandler(ev);
      }
   }

   void setWaveform( byte i )
   {
      setTable( (const desWavTab* )wavebank.dataPtr(i) );
      name = wavebank.name(i);
   }

} ;

/******************************************************************************
 *
 *                               HocusVox
 *
 ******************************************************************************/

class HocusVox : public Voice
{
   typedef Voice super;                   // superclass is Voice

   public:

   MaxiVibrato vibrato;                   // vibrato pitch modulator
   Decay       decay;                     // percussive decay
   Gain        gain;                      // gain effect
   ToneWheel  *toneWheel;                 // synonym for osc (typecast as ToneWheel)

   HocusVox()
   {
      toneWheel = new ToneWheel();
      useOsc( toneWheel );

      addPitchMod( &this->vibrato );
      addAmpMod( &this->decay );
      addEffect( &this->gain );
   }

   bool charEv( char code )               // process a character event
   {
      switch ( code )
      {
         #ifdef INTERN_CONSOLE            // compile cases needed by macros

         case 'D':                        // push decay control

            console.pushMode( &this->decay );
            break;

         case 'G':                        // push gain control

            console.pushMode( &this->gain );
            break;

         case 'V':                        // push vibrato control

            console.pushMode( &this->vibrato );
            break;

         #endif

         default:

            super::charEv( code );
      }
      return true;
   }

} ;

/******************************************************************************
 *
 *                                Settings
 *
 ******************************************************************************/

class Settings : public Mode           // Runtime settings menu
{
   typedef Mode super;                 // superclass is Mode

   public:

   bool charEv( char code )            // process a character event
   {
      switch ( code )
      {
         case focusPUSH:

            blinkLED(0);               // set up alternating blinking LEDs
            blinkLED(1, true);
            break;

         default:

            return super::charEv( code );
      }
      return true;
   }

   bool evHandler( obEvent ev );       // handle an onboard event

   PROMPT_STR( Settings )

} settings;


/******************************************************************************
 *
 *                               HocusSynth
 *
 ******************************************************************************/

ByteMenu presetMenu;                      // keybrd menu for selecting presets

// the following defines relate to dynamic acceleration/deceleration of rotaries

#define MIN_ROTARY_FREQ    0.3
#define MAX_ROTARY_FREQ    4.4
#define ROTARY_RELOAD      8
#define HALF_ROTARY_RELOAD 4
#define ROTARY_LAG        .75             // ratio between rotary 1.2/0.3 speeds
#define ROTARY_INCREMENT  .03             // increment per reload

class HocusSynth : public QuadDualPanSynth
{
   typedef QuadDualPanSynth super;        // superclass is QuadDualPanSynth

   public:

   bool  sustain;                         // if true sustain notes indefinitely
   bool  ensemble;                        // if true output all voices (else just working voice)
   byte  workNum;                         // working voice # (0..3)

   HocusVox *workVox;                     // ptr to working voice

   byte rotaryDC;                         // downcounter to next dynamic rotary speed update

   typedef byte DynaRotary ; enum
   {
      rotaryACCEL,                        // dynamic acceleration of rotary speakers is in progress
      rotaryDECEL,                        // dynamic deceleration of rotary speakers is in progress
      rotaryCONSTANT                      // speed of rotary speakers is constant

   } rotaryAction;

   void config()
   {
      wavebank.load( wavetables );        // load wave tables (tonewheel "registrations")
      super::config();
      setFrameDimensions( 2, 2 );
   }

   // Note: newOsc() will never be called (back) by the system because all
   // oscillators in this sketch are allocated and registered via useOsc()
   // in the HocusVox() connstructor, and the system only calls newOsc()
   // for voices that have a NULL osc ptr after newVox() for that voice
   // has been called. However, we override newOsc() here simply to avoid
   // having the newOsc() method of the super class linked in --  which
   // costs an extra 450 bytes of ROM!

   Osc *newOsc( byte nth )                // create nth osc (NEVER CALLED)
   {
      return NULL;
   }

   Voice *newVox( byte nth )              // create nth voice
   {
      return new HocusVox();
   }

   bool charEv( char code )               // process a character event
   {
      switch ( code )
      {
         #ifdef INTERN_CONSOLE

         case 'A':                        // accelerate rotaries

            accelerateRotaries();
            break;

         case 'C':                        // constant rotary speed

            constantRotaries();
            break;

         case 'D':                        // decelerate rotaries

            decelerateRotaries();
            break;

         case 'e':                        // set ensemble on/off

            console.getBool( CONSTR("ensemble"), &ensemble );
            break;

         case 'M':                        // push parameter menu

            console.pushMode( &settings );
            break;

         case 's':                        // set sustain
         {
            bool val;
            if ( console.getBool( CONSTR("sustain"), &val ) )
               setSustain( val );
            break;
         }
         case 'S':                        // load stock preset

            stockPreset();
            break;

         case 'w':                        // set workNum
         {
            byte val;
            if ( console.getByte( CONSTR("workNum"), &val ) )
               setWorkVox( val );
            break;
         }

         #endif

         #ifdef CONSOLE_OUTPUT

         case chrInfo:

            super::charEv( code );
            console.newlntab();
            console.infoBool( CONSTR("sustain"), sustain );
            console.newlntab();
            console.infoBool( CONSTR("ensemble"), ensemble );
            console.infoByte( CONSTR("workNum"), workNum );
            break;

         #endif

         case '!':                           // perform a reset

            super::charEv( code );

            enableFrame();                   // use embedded user-interface frame
            setSustain( false );             // sustain off
            setWorkVox(0);                   // voice 0 is working voice
            stockPreset();                   // start with stock preset

            break;

         default:

            return super::charEv( code );
      }
      return true;
   }

   void dynamics()                           // perform a dynamic update
   {
      super::dynamics();

      double freq;

      switch ( rotaryAction )
      {
         case rotaryACCEL:
         {
            if ( --rotaryDC == 0 )
            {
               freq = xpanControl->getFreq() + ROTARY_INCREMENT;
               if ( freq > MAX_ROTARY_FREQ )
               {
                  xpanControl->setFreq( MAX_ROTARY_FREQ );
                  constantRotaries();
                  return;
               }
               else
               {
                  xpanControl->setFreq( freq );
                  panControl->setFreq( freq * ROTARY_LAG );
                  updateRotaryAction();
               }
            }
            break;
         }
         case rotaryDECEL:
         {
            if ( --rotaryDC == 0 )
            {
               freq = xpanControl->getFreq() - ROTARY_INCREMENT;
               if ( freq < MIN_ROTARY_FREQ )
               {
                  xpanControl->setFreq( MIN_ROTARY_FREQ );
                  constantRotaries();
                  return;
               }
               else
               {
                  xpanControl->setFreq( freq );
                  panControl->setFreq( freq * ROTARY_LAG );
                  updateRotaryAction();
               }
            }
            break;
         }
      }
   }

   bool evHandler( obEvent e )               // event handler
   {
      if ( handlePots(e) ) return true;      // handle any pot events

      switch ( e.type() )
      {
         case BUT0_TPRESS:                   // decelerate rotary speakers

            decelerateRotaries();
            break;

         case BUT1_TPRESS:                   // accelerate rotary speakers

            accelerateRotaries();
            break;

         case BUT0_DTAP:                     // push settings menu

            console.pushMode( &settings );
            break;

         case BUT1_DTAP:                     // run a preset

            presetMenu.waitKey();            // user selects preset by pressing key
            runPreset( (const char *)presets.dataPtr( presetMenu.value ) );
            break;

         default:

            return super::evHandler(e);      // pass other events through
      }
      return true;
   }

   bool handlePots( obEvent ev )             // handle pot events
   {
      if ( ! ev.amPot() )                    // return false if not a pot event
         return false;

      byte   potVal   = ev.getPotVal();      // cache pot value
      evType potFrame = ev.type();           // cache event type

      switch ( potFrame )
      {
         /*             FRAME 00             */

         case POT0_F00:                      // registration

            workVox->osc->potEv(ev);
            break;

         case POT1_F00:                      // transposition (-24 to +24 semitones)
         {
            int transpose  = potVal;

            if (transpose >= 254 )
               transpose = 256;
            transpose -= 128;
            transpose *= 48;
            transpose >>= 8;

            workVox->xpose = transpose;
            workVox->noteOn( lastNote );
            break;
         }

         /*             FRAME 10             */

         case POT0_F10:                      // volume
         case POT1_F10:                      // detune

            workVox->potEv(ev);
            break;

         /*             FRAME 20             */

         case POT0_F20:                      // decay

            workVox->decay.setTime( potVal );
            workVox->decay.setMute( ! potVal );
            break;

         case POT1_F20:                      // vibrato INTERVAL
         {
            word interval = 1 + (( potVal * (MAX_MULT) ) >> 8);
            workVox->vibrato.setMultiple( interval );
            break;
         }

         /*             FRAME 01             */

         case POT1_F01:                      // vibrato DEPTH

            workVox->vibrato.setMute( ! potVal );  // turn vibrato off if depth == 0

            // fall thru

         case POT0_F01:                      // vibrato FREQ

            workVox->vibrato.potEv(ev);
            break;

         /*             FRAME 11             */

         case POT0_F11:                      // vibrato SHAPE
         {
            char shape;
            if ( potVal < 64 )
               shape = 'T';                  // triangle
            else if ( potVal < 128 )
               shape = 'R';                  // rising saw-tooth
            else if ( potVal < 172 )
               shape = 'F';                  // falling saw-tooth
            else
               shape = 'Q';                  // square
            workVox->vibrato.charEv( shape );
            break;
         }

         case POT1_F11:                      // vibrato RANGE
         {
            if ( potVal < 85 )
               workVox->vibrato.execute( PSTR("rb\\") );  // range both positive and negative
            else if ( potVal < 170 )
               workVox->vibrato.execute( PSTR("rp\\") );  // positive range only
            else
               workVox->vibrato.execute( PSTR("rn\\") );  // negative range only
            break;
         }

         /*             FRAME 21             */

         case POT0_F21:                      // vibrato TIME

            workVox->vibrato.setFadeTime( potVal );
            break;

         case POT1_F21:                      // vibrato FADE

            if ( potVal < 128 )
               workVox->vibrato.fadeIn();
            else
               workVox->vibrato.fadeOut();
            break;

         /*             FRAME 02             */

         case POT0_F02:                      // gain AMOUNT

            workVox->gain.potEv( ev );
            workVox->gain.setMute( ! potVal );
            break;
                                             // gain OVERDRIVE
         case POT1_F02:

            workVox->gain.autoClip = ( potVal < 128 );
            break;

         default:

            ev.setPotVal( potVal >> 1 );     // reduce pot magnitude by half

            if ( potFrame < POT0_F22 )

         /*             FRAME 12             */

         // case POT0_F12:                   // rotary 0.3 FREQ
         // case POT1_F12:                   // rotary 0.3 DEPTH

               xpanControl->potEv(ev);

            else

         /*             FRAME 22             */

         // case POT0_F22:                   // rotary 1.2 FREQ
         // case POT1_F22:                   // rotary 1.2 DEPTH

               panControl->potEv(ev);

      }
      return true;
   }

   void accelerateRotaries()
   {
      rotaryAction = rotaryACCEL;
      rampRotaries();
   }

   void decelerateRotaries()
   {
      rotaryAction = rotaryDECEL;
      rampRotaries();
   }

   void constantRotaries()
   {
      rotaryAction = rotaryCONSTANT;
   }

   void updateRotaryAction()
   {
      double freq = xpanControl->getFreq();
      rotaryDC    = ROTARY_RELOAD;
      if ( freq < 2.25 )
         rotaryDC *= 2;
      if ( freq < .6 )
      {
         rotaryDC *= 2;
         rotaryDC -= HALF_ROTARY_RELOAD;
      }

      byte depth = xpanControl->getDepth();
      if ( depth < 126 )
         xpanControl->setDepth( depth+2 );
      depth = panControl->getDepth();
      if ( depth < 126 )
         panControl->setDepth( depth+2 );
   }

   void rampRotaries()
   {
      panControl->setFreq( xpanControl->getFreq() );
      updateRotaryAction();
   }

   void noNote()
   {
      setVol( 0 );
      lastNote.reset();
   }

   void noteOn( key note )                // turn a note on
   {
      if ( sustain && ( note == lastNote ) )
         noNote();
      else
      {
         setVol( 255 );
         super::noteOn( note );
      }
   }

   void noteOff( key note )               // turn a note off
   {
      if ( ! sustain )
         setVol(0);
      super::noteOff( note );
   }

   void output( char *bufL, char *bufR )
   {
      if ( ensemble )
         super::output( bufL, bufR );
      else
      {
         workVox->output( bufL );
         for ( byte i = 0 ; i < audioBufSz; i++ )
            bufR[i] = bufL[i];
      }
   }

   void setSustain( bool turnOn )
   {
      if ( (! turnOn) || ( turnOn && ! sustain ) )
         noNote();                                       // turn off any note playing

      sustain = turnOn;
   }

   void setWorkVox( byte nth )
   {
      if ( nth < numVox )
      {
         workNum = nth;
         workVox = (HocusVox *)vox[workNum];
      }
   }

   void stockPreset()
   {
      execute( PSTR("Cet\\") );                          // constant rotaries/ensemble audio
      forEach( PSTR("v255\\Ow0`V!`A!`G!.m8\\") );        // stock volume/waveform/vibrato/decay/gain
      execute( PSTR("x0\\0x-12\\`1x0\\`2x0\\`3x7\\") );  // stock transpositions
      execute( PSTR("0d2\\`1d0\\`2d-3\\`3d-1\\") );      // stock detunings
      execute( PSTR("P!f.8\\d32\\<`X!f1.2\\d36\\<") );   // stock pannings
   }

   void welcome()
   {
      presets.load( myPresets );
   }

} hocus;

bool Settings::evHandler( obEvent ev )
{
   switch ( ev.type() )
   {
      case KEY_DOWN:

         switch ( ev.getKey().position() )
         {
            case 0:                 // 'C'  select tone wheel 0

               hocus.setWorkVox(0);
               break;

            case 1:                 // 'C#'

               break;

            case 2:                 // 'D'  select tone wheel 1

               hocus.setWorkVox(1);
               break;

            case 3:                 // 'D#'

               break;

            case 4:                 // 'E'  select tone wheel 2

               hocus.setWorkVox(2);
               break;

            case 5:                 // 'F'  select tone wheel 3

               hocus.setWorkVox(3);
               break;

            case 6:                 // 'F#'

               break;

            case 7:                 // 'G'  set sustain on

               hocus.setSustain( true );
               break;

            case 8:                 // 'G#'

               break;

            case 9:                 // 'A'  set sustain off

               hocus.setSustain( false );
               break;

            case 10:                // 'A#' ISO audio

               hocus.ensemble = false;
               break;

            case 11:                // 'B'  ENSEMBLE audio

               hocus.ensemble = true;
               break;
         }

         break;

      case KEY_UP:

         console.popMode();
         break;

      default:                      // pass other events to superclass

         return super::evHandler(ev);
   }

   return true;
}

void setup()
{
   ardutouch_setup( &hocus );
}

void loop()
{
   ardutouch_loop();
}

