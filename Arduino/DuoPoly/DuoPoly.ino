/*
    DuoPoly.ino
 
    A duophonic macro-capable stereo wavetable synthesizer with dual sequencers.

    Target:   ArduTouch board
 
    ---------------------------------------------------------------------------
 
    Copyright (C) 2016, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
    ---------------------------------------------------------------------------
 */

#include "System.h"                          // declarations for system module
#include "WaveBank.h"                        // declarations for wavebank module 
#include "DuoSynth.h"                        // declarations for synthesizer

about_program( DuoPoly, 2.05 )               // specify program name & version
set_baud_rate( 115200 )                      // specify serial baud-rate

DuoSynth synth;                              // instantiate synthesizer 

void setup()
{
   system_setup( &synth );                   // initialize system resources
   synth_setup();                            // initialize synthesizer
   audio::enable();                          // enable audio output
}

void loop()
{
   system_loop();                            // perform ongoing system tasks  
}                                             

/*----------------------------------------------------------------------------*
 *                                 presets
 *----------------------------------------------------------------------------*/      

define_preset( Hive, "#!w1rebs6c3<``k3z``leb-s3c6<``t488\\S3jnhn`[`#" )

define_preset( Keltos, "#!w2r*1.5\\g230\\Ea62\\d153\\s192\\<`ex-w30\\<```"
                       "lt275\\ebs3<``"
                        "S3c.m...c.m.....4gcx...s...3nm4sxs.3n.c.m...c.m.....4gcx...s...3nm4s3nm...`[`#" )

define_preset( Industry, "#!lw1t32.5\\eb-s5c4<``S2zsvc``reb-s2c1<``t325\\S7zc,bjbchvs``l`[#" )

define_preset( Omission, "#!rt205\\v205\\Tf.64\\d.375\\t1\\L<`Vf5.3\\d.46\\t26\\-<`"
                         "S4.cb5x.........4cb5s.........4cb5z........z4m..........``"
                         "lw1t205\\Tf4\\d1\\t3\\~<`S2c..3c.x.2b.nx.``[#" )

define_preset( Cadiz, "#!rw1t30\\V<f4.4\\d.13\\`"
                      "S1j......2j...3v..g.v.d.x.2m1j`f54\\g14\\`" 
                      "lw1t240\\g248\\"
                      "S2j4xv3j4jx3j4h2j4xv3j4jx3j4h2j4dg3j4jd3j4h2j4dg3j4gv3m4d``[#" )

define_preset( DemoPlay, "!rw2t205\\Tf.64\\d.375\\t1\\<`Vf5.3\\d.46\\t26\\-<`k3b``"                      
                         "lw1t205\\Tf3\\d1\\t3\\<`k3z``/l" )

/*----------------------------------------------------------------------------*
 *
 *  Name:   synth_setup
 *
 *  Desc:   Initialize the synthesizer:
 *
 *             1) load wavetables
 *             2) load any presets 
 *             3) call synthesizer's startup method
 *
 *  Note:   The following pre-defined wavetables are available:
 *
 *             Sine / dVox / Rood / Ether / SynStr
 *
 *----------------------------------------------------------------------------*/      

begin_bank( wavetables )               // these wavetables will be loaded
                         
   _wavetable( Sine )
   _wavetable( dVox )
   _wavetable( Rood )

   #ifndef CONSOLE_OUTPUT              // if console output is left out, there   
      _wavetable( SynStr )             // is room to load an extra wavetable :)
   #endif

end_bank()

begin_bank( presets )                  // these presets will be loaded

   _preset( Hive )
   _preset( Keltos )
   _preset( Industry )
   _preset( Omission )
   _preset( Cadiz )
   _preset( DemoPlay )

end_bank()

void synth_setup()
{
   wavebank.load( wavetables );        // load wavetables
   synth.load( presets );              // load presets
   synth.startup();                    // and start it up
}


