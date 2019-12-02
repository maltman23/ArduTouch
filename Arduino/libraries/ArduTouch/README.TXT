                         ArduTouch Library 
                            README.TXT

1) Make sure that the PCB REV of your board (printed near the bottom of the
   ArduTouch card) corresponds to the revision declared in System.h 

2) Arduino build 1.6.6 generates smaller executables for large scale synths
   than the current hourly builds (1.8.9 as of this writing). If you are 
   running out of program storage space we recommend you use build 1.6.6,
   (and also uncomment the "//#define BUILD_166" statement in Model.h, if
   you are running synths supplied by Cornfield Electronics.)
   

                     What's New in Version 1.13
                     --------------------------

1) LFO internals have been rewritten to use fixed-point 16 bit math instead 
   of floating point math, significantly reducing the code footprint.
  
   There are now four different LFO waveforms available (triangle, square, 
   rising sawtooth and falling sawtooth) instead of just one (triangle).

   As part of the LFO rewrite, the following method:

      LFO::setDepth( double x ) -- where x is between 0.0 and 1.0

   has been changed to

      LFO::setDepth( byte x ) -- where x is between 0 and 128

   So if you've previously written any code that calls this method you will
   have to convert the argument. Also, if you've created any presets or macros
   that set the depth via a string (i.e. "d.5\\") those will have to be edited
   too (i.e. "d.5\\" becomes "d64\\")
 
2) Two new example sketches have been added:

      _60_LFO.ino
      _61_Signed_LFO.ino

3) A bug in the Mantra and Beatitude synths which caused weird instabilities
   has been fixed.

4) A new version of the Quadrant synth makes use of previously reserved pot 
   assignments to control the detuning and transposition of lead voice 1 
   relative to lead voice 0. 

5) All the synths have been revised to sync with changes to the LFO class.






