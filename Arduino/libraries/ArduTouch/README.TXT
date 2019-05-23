                         ArduTouch Library 
                            README.TXT

1) Make sure that the PCB REV of your board (printed near the bottom of the
   ArduTouch card) corresponds to the revision declared in System.h 

2) Arduino build 1.6.6 generates smaller executables for large scale synths
   than the current hourly builds (1.8.6 as of this writing). If you are 
   running out of program storage space we recommend you use build 1.6.6,
   (and also uncomment the "//#define BUILD_166" statement in Model.h, if
   you are running synths supplied by Cornfield Electronics.)
   

                     What's New in Version 1.11
                     --------------------------

1) A new synth, Dronetic.ino, is available.

   Dronetic uses 2 drones, each drone is made up of 4 quantum square wave 
   oscillators, and the drones are panned dynamically in stereo. You can 
   then play a lead over the drones, and the pots control a bunch of 
   parameters in real time as you play.

   For an audio demo of Dronetic go to https://www.youtube.com/c/mitchaltman23

2) A HarmonicTuning class has been added to the library, as an alternative
   to EqualTemperament.

   For a discussion of Harmonic Tuning and an example of how to override
   the system default tuning of EqualTemperament see Dronetic.ino.

3) A Gain effect has been added to the StdEffects module of the library.

   For a demo and more info see the new example sketch: 
   
         _45_Gain.ino

4) A new version of the Xoid synth (0.96) syncs with changes to the library.
   The new version of Xoid loads presets quietly -- without an audible 'click',
   as in older versions. Otherwise there are no changes to it. 
   Older versions of Xoid will not compile with version 1.11 of the library.



