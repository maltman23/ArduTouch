/*
    DuoPoly.ino

    A duophonic, stereo wavetable synthesizer.

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
#include "Console_.h"                        // declarations for console module
#include "DuoSynth.h"                        // declarations for synthesizer

DuoSynth synth;                              // instantiate synthesizer

Config config =                              // program configuration record
{
   CONSTR("DuoPoly"),                        // program title
   CONSTR("1.70"),                           // program version
   &synth,                                   // object which generates audio
   150.0,                                    // dynamic update rate
   115200                                    // serial baud rate
};

void setup()
{
   system_setup( &config );                  // initialize system resources
   synth_setup();                            // initialize synthesizer
   audio::enable();                          // enable audio output
}

void loop()
{
   system_loop();                            // perform ongoing system tasks
}

/*----------------------------------------------------------------------------*
 *
 *  Name:   synth_setup
 *
 *  Desc:   Initialize the synthesizer:
 *
 *             1) load any presets
 *             2) reset synthesizer
 *
 *----------------------------------------------------------------------------*/

const char name0[] PROGMEM = { "Hive" };
const char code0[] PROGMEM = { "!w1rebs7c3<``k3z``leb-s3c6<``t488\\S3jnhn`[`" };

const char name1[] PROGMEM = { "Keltos" };
const char code1[] PROGMEM = { "!w2r*1.5\\eb-<```lt275\\ebs3<``S3c.m...c.m.....4gcx...s...3nm4sxs.3n.c.m...c.m.....4gcx...s...3nm4s3nm...`[`" };

const char name2[] PROGMEM = { "Industry" };
const char code2[] PROGMEM = { "!lw1t32.5\\eb-s4c4<``S2zsvc``reb-s2c1<``t325\\S7zc,bjbchvs``l`[" };

const char name3[] PROGMEM = { "Omission" };
const char code3[] PROGMEM = { "!rt205\\v.8\\Tf.64\\d.375\\t1\\L<`Vf5.3\\d.46\\t26\\-<`"
                                "S4.cb5x.........4cb5s.........4cb5z........z4m..........``"
                                "lw1t205\\Tf4\\d1\\t3\\L<`S2c..3c.x.2b.nx.``["
                               };

const char name4[] PROGMEM = { "DemoPlay" };
const char code4[] PROGMEM = { "!rw2t205\\v1.0\\Tf.64\\d.375\\t1\\S<`Vf5.3\\d.46\\t26\\-<`k3b``"
                                "lw1t205\\Tf3\\d1\\t3\\S<`k3z``"
                                "/l"
                               };

const bankmem myPresets[] PROGMEM =
{
   { name0, code0 },
   { name1, code1 },
   { name2, code2 },
   { name3, code3 },
   { name4, code4 },
   { NULL , NULL  }                    // end-of-list marker
} ;

void synth_setup()
{
   synth.setup( myPresets );           // load presets
   synth.reset();                      // and reset
}
