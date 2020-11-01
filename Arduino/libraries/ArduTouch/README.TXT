                         ArduTouch Library 
                            README.TXT

1) Make sure that the PCB REV of your board (printed near the bottom of the
   ArduTouch card) corresponds to the revision declared in System.h 


                     What's New in Version 1.16
                     --------------------------

1) ADSR internals have been rewritten to use fixed-point 16 bit math instead 
   of floating point math, significantly reducing the code footprint.

2) A long-standing audio issue which effected all synths and which was 
   characterized by a "glitch" at the start of the initial note played
   after power-up or reset or after a preset was loaded, has been solved.
   Initial notes are now glitch-free for all synths, all presets :)

   (This glitch was most noticeable when the played note had a slow attack.
   Try Arpology, for example, under 1.15 and play an initial note, then try 
   the new version under 1.16)

3) A major new feature -- User Interface Frames -- has been incorporated
   into the low-level Control class. User Interface Frames provide a 
   customizable mechanism for controlling up to 18 different parameters 
   with just the 2 on-board pots. Since virtually every object in a synth,
   including the synth itself, inherits from the Control class, this opens
   up many possibilities for real-time control of all a synth's parameters.

4) There is a new example sketch which explains User Interface Frames:
   
      _57_UI_Frames.ino

5) The following synths have been updated to sync with the new UI Frames:

      Arpology
      Dronetic
      DuoPoly
      Hocus
      Quadrant
      Xoid
      ZapShot

7) The button interface for the DuoPoly Voice and ADSR objects has changed.
   Previously, the pots for the Voice and ADSR parameters were scrolled up
   and down using left-button-press and right-button-press gestures. Now, 
   using the built-in UI Frame for Voice and ADSR classes -- each of which 
   has a dimension of (0,1) -- the pot mappings are switched back and forth
   between FRAME00 and FRAME01 using the right-button-press gesture only. 
   See _57_UI_Frames.ino for details on frame dimensions, symbolic frame 
   numbers and frame buttons.
   





    






