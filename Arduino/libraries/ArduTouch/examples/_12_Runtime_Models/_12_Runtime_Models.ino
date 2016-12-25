//
//  _12_Runtime_Models.ino
//
//  This sketch describes how the ArduTouch firmware can be compiled in
//  different ways to enable different feature sets.
//
//  A particular feature set is called a "Runtime Model".
//
//  A sketch which requires fewer system features can be compiled using a
//  simpler Runtime Model, and this will free up additional memory for use
//  by the sketch.
//
//  There are four Runtime Models available in the ArduTouch firmware. These
//  are listed below, along with their salient features (which will be fully
//  explained in later examples).
//
//        Full Host    two-way serial communications enabled with host
//                     library classes support macros
//                     ArduTouch onboard keyboard menus are supported
//                     (onboard LEDs are not functional)
//
//        Lite Host    one-way host-to-board serial communications
//                     library classes support macros
//                     ArduTouch onboard keyboard menus are supported
//                     (onboard LEDs are not functional)
//
//        Stand-Alone  no host serial communication
//                     library classes support macros
//                     ArduTouch onboard keyboard menus are supported
//                     (onboard LEDs *are* functional)
//
//        Bare Bones   no host serial communications
//                     library classes *do not* support macros
//                     ArduTouch onboard keyboard menus *not* supported
//                     (onboard LEDs *are* functional)
//
//  The Runtime Model is specified by a #define statement in the file Model.h,
//  which is a part of the ArduTouch library. In order to change the Runtime Model
//  you must therefore first locate the directory that the ArduTouch library source
//  code resides in. The location of this directory will vary depending on what OS
//  you are using, and even what version of Arduino you are running. So before you
//  proceed any further please take the time to locate the library directory
//  containing Model.h.
//
//  Towards the top of Model.h you will see the following four statements (and
//  they will read exactly as follows if you've just loaded the library for the
//  1st time.)
//
//          #define __FULLHOST__
//       // #define __LITEHOST__
//       // #define __STNDLONE__
//       // #define __BAREBONE__
//
//  Whichever of these lines is uncommented will specify your Runtime Model.
//
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

#include "ArduTouch.h"                    // use the ArduTouch library

about_program( Runtime Models, 1.00 )     // specify sketch name & version
set_baud_rate( 115200 )                   // specify serial baud-rate

//  ---------------------------------------------------------------------------
//
//  The purpose of this sketch is to get you acquainted with the Runtime Model
//  concept. The sketches that follow this one may require that a certain
//  Runtime Model be declared.
//
//  Our synth here is incidental: we will just re-use the BareMinimumSynth
//  from example _01_.
//
//  ---------------------------------------------------------------------------


class RuntimeModelSynth : public MonoSynth   // an "empty" synth
{
} myRuntimeModelSynth;                       // instantiate synthesizer


//  ---------------------------------------------------------------------------
//
//                       Some Tips on Runtime Models
//
//  If you are a casual user of the ArduTouch library you may never need to
//  change the Runtime Model from its "factory preset" of __FULLHOST__.
//
//  If you never intend to use the host interface you can safely set the
//  Runtime Model to __STNDLONE__, and still be assured that "all the
//  onboard stuff works."
//
//  If you are a hardcore hacker who wants to squeeze every available cycle
//  and byte from the system and are not interested in the nicities of host
//  communications and macros and keyboard menus (more on these later) then
//  you can set the Runtime Model to __BAREBONE__ and roll with it.
//
//  You may want to set the Runtime Model differently for different sketches
//  depending on their individual requirements.
//
//  ---------------------------------------------------------------------------

void setup()
{
   ardutouch_setup( &myRuntimeModelSynth );  // initialize ardutouch resources
}

void loop()
{
   ardutouch_loop();                         // perform ongoing ardutouch tasks
}

//  --------------------------------------------------------------------------
//
//  Try changing the Runtime Model in Model.h, and then compiling this sketch
//  to see how much memory is used. On the developer's machine the following
//  memory usages were observed:
//
//       __FULLHOST__  9496 bytes of ROM, 570 bytes of RAM
//       __LITEHOST__  8060 bytes of ROM, 536 bytes of RAM
//       __STNDLONE__  7104 bytes of ROM, 366 bytes of RAM
//       __BAREBONE__  5870 bytes of ROM, 275 bytes of RAM
//
//  For more complex synthesizers the differences will be more dramatic
//  than those seen for this simplest ArduTouch synthesizer.
//
//  --------------------------------------------------------------------------

