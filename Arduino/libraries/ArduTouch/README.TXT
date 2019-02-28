                         ArduTouch Library 
                            README.TXT

1) Make sure that the PCB REV of your board (printed near the bottom of the
   ArduTouch card) corresponds to the revision declared in System.h 

2) Arduino build 1.6.6 generates smaller executables for large scale synths
   than the current hourly builds (1.8.6 as of this writing). If you are 
   running out of program storage space we recommend you use build 1.6.6,
   (and also uncomment the "//#define BUILD_166" statement in Model.h, if
   you are running synths supplied by Cornfield Electronics.)
   

                     What's New in Version 1.10
                     --------------------------

1) The ArduTouch library now works with the latest release of the Arduino IDE
   (1.8.8).

   There is a known bug -- recently introduced -- in the Arduino linker, that 
   causes an error if a class method is given the same name as a global function.
   In order to circumvent this bug, the method Synth::setup() has been renamed 
   Synth::config(). (Thanks to Charles for suggesting this fix.)
   
   Also, VoxSynth::setupVoices() has been renamed VoxSynth::configVoices()
   to keep the naming conventions consistent.

2) Two new oscillator classes have been added for playing back samples:

         class ModSampleOsc
         class TunedSampleOsc

   ModSampleOsc inherits from SampleOsc and adds 2 new features: the sample 
   can be played forwards or backwards, and the duration of the playback can
   be stretched.

   For a demo and more info see the new example sketch: 
   
         _31_Modified_DrumPad.ino

   TunedSampleOsc inherits from WaveOsc. The wave table for a TunedSampleOsc
   is played only once when triggered (instead of looping continuously). 
   Unlike a SampleOsc, the wavetable for a TunedSampleOsc is considered to
   be tonal, to have a definite wavelength, and so can be played back at 
   specific musical pitches like a normal oscillator.

   For a demo and more info see the new example sketch: 
   
         _32_TunedSampleOsc.ino

3) A new sample, Wood01, has been added to the Drums library.
   As with all library samples there is a "lo-fi" version (lofi_Wood01).
   Wood01 is the sound of a wood block being struck.

4) A new subclass, ADSRVoice, has been broken out from the Voice class.
   
   Voice's built-in .envAmp (an ADSR envelope) has been moved to ADSRVoice.

   The StockVoice class now inherits from ADSRVoice (which inherits from Voice).

   This change impacts all code which referenced the ADSR envelope via the vox[]
   pointers used by synths which have VoxSynth as a super class. These pointers
   must now be typecast (to ADSRVoice*) in order to reference .envAmp
   
   See the example sketch _61_Voice_Part_1.ino for more details.

5) The Envelope class has been renamed ADSR.

6) To sync with the changes in the library as described in (1) (4) and (5) 
   there are new versions of the following synths:

         Arpology
         Beatitude
         DuoPoly
         Mantra
         Thick
         Xoid
         ZapShot

7) The following example sketches have been rewritten because they wrongly
   portrayed the relationship between the Voice, StockVoice, and OneVoxSynth
   classes:

         _08_Envelope.ino
         _09_Vibrato.ino

   If you worked through these example sketches in a prior release you might want
   to revisit them.


