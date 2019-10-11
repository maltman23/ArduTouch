                         ArduTouch Library 
                            README.TXT

1) Make sure that the PCB REV of your board (printed near the bottom of the
   ArduTouch card) corresponds to the revision declared in System.h 

2) Arduino build 1.6.6 generates smaller executables for large scale synths
   than the current hourly builds (1.8.6 as of this writing). If you are 
   running out of program storage space we recommend you use build 1.6.6,
   (and also uncomment the "//#define BUILD_166" statement in Model.h, if
   you are running synths supplied by Cornfield Electronics.)
   

                     What's New in Version 1.12
                     --------------------------

1) A new synth, Quadrant, is released.

   Quadrant features a hybrid stereo effect called an Echotron, which is part
   echo, part arpeggiator, and part pitch shifter.
   
   For audio demos of Quadrant go to https://www.youtube.com/c/mitchaltman23

2) A new Envelope class, AutoADSR, has been added to the library. 

   AutoADSR inherits from the ADSR emvelope class, and adds a sustain time 
   parameter. The AutoADSR envelope differs from the ADSR envelope in that
   once it reaches the sustain stage it will automatically release after
   the amount of time specified by the sustain time parameter (instead of
   being held indefinitely until explicitly released). This is a very useful
   feature for envelopes that are not being directly controlled by the user
   via the keyboard.

   For an example of using the AutoADSR class see the new Quadrant synth, 
   which uses AutoADSRs to envelope the oscillators in the Echotron unit.

3) Two additional envelope classes, MasterADSR and MasterAutoADSR have been added
   to the library. These are special "master" envelopes which allow you to control
   up to four "slave" envelopes residing in other objects. This can be useful when
   you want to sync the envelope parameters of multiple voices. 
   
   For an example of using both of these classes see the new Quadrant synth, 
   which uses a MasterADSR to sync the envelopes of the two lead voices, 
   and a MasterAutoADSR to sync the envelopes of the four oscillators in 
   the Echotron unit.

4) The system function blinkLED() now takes an optional 2nd argument that allows
   you to invert the phase of the blinking LED:

      void blinkLED( byte nth, bool invert = false );  // blink nth LED, invert phase

    An LED blinking with an inverted phase will be ON when a normally blinking LED
    is off, and vice-versa. This allows you to set up the two onboard LEDs to blink
    alternatively instead of in unison. See the example sketch _13_LEDs.ino for a
    demo.

5) A new callback method Synth::tuning() has been added to facilitate overriding
   the default system tuning in a standard and efficient way.

   For an example of how to create your own custom tuning, and how to register it
   with the system via the tuning() callback method see the new example sketch:

         _35_Custom_Tuning.ino

6)  The library constant KEYBOARD_MENUS is now undefined by default.
    
    The KEYBRD_MENUS define enables a "legacy" feature of the library 
    which allows classes derived from the Mode class to use the onboard 
    keyboard to generate character events, turning the keyboard into
    a defacto menu by which to drive the Console interface for an object. 
    This is an admittedly obscure feature, currently used only by the 
    DuoPoly synth (which was the 1st synth ever written for the ArduTouch).

    Because KEYBRD_MENUS consumes valuable resources but is rarely if ever 
    used it is considered a legacy feature and is now deprecated. 
    
    If you want to compile the DuoPoly synth you will need to uncomment the 
    "#define KEYBRD_MENUS" statement in Model.h of the library. Once you have 
    compiled DuoPoly, you should make sure to comment out "#define KEYBRD_MENUS" 
    so that subsequent compilations of other synths do not unnecessarily include 
    the keyboard menus code.   
    
7) New versions of the following synths sync with changes to the library:

      Arpology
      Beatitude
      Dronetic
      DuoPoly
      Mantra




