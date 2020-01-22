                         ArduTouch Library 
                            README.TXT

1) Make sure that the PCB REV of your board (printed near the bottom of the
   ArduTouch card) corresponds to the revision declared in System.h 

2) Arduino build 1.6.6 generates smaller executables for large scale synths
   than the current hourly builds (1.8.9 as of this writing). If you are 
   running out of program storage space we recommend you use build 1.6.6,
   (and also uncomment the "//#define BUILD_166" statement in Model.h, if
   you are running synths supplied by Cornfield Electronics.)
   

                     What's New in Version 1.14
                     --------------------------

1) Several bug fixes have been made to the RealTimeSequencer class.

2) A new version of the Beatitude synth has these added features:

   1. The drum kit is now panned in stereo.

   2. The top pot controls the balance between the drum kit and the bass.

   3. When recording a sequence with prior versions of Beatitude the 1st 
      drum hit was often missed if it occured "right before" the 1st beat 
      of the 1st measure. That no longer happens.
    






