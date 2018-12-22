                        ArduTouch Library 
                           README.TXT

1) Make sure that the PCB REV of your board (printed near the bottom of the
   ArduTouch card) corresponds to the revision declared in System.h 

2) Arduino build 1.6.6 generates smaller executables for large scale synths
   than the current hourly builds (1.8.6 as of this writing). If you are 
   running out of program storage space we recommend you use build 1.6.6,
   (and also uncomment the "//#define BUILD_166" statement in Model.h, if
   you are running synths supplied by Cornfield Electronics.)
   

                    What's New in Version 1.09
                    --------------------------

1) Two new modules have been added: 

   QuantumOsc 
   VirtualPot

2) The QuantumOsc module contains the following new classes:

   QuantumOsc      - an abstract base class for oscillators that run only at 
                     specific quantized frequencies

   PureSquare      - a quantum square wave oscillator with even periods

   SweepOsc        - a PureSquare oscillator that performs quantized frequency sweeps 
                     between any two integral periods

3) The VirtualPot module contains the following new classes:

   VirtualPot      - an abstract base class for constructing "soft" pots which
                     translate pot positions into a custom output range of values 
                     (either increasing or decreasing)

   ParamSend       - a VirtualPot whose output values are sent as arguments to any
                     method of type void myMethodName( byte ) in any class object
                     
   PotSend         - a VirtualPot whose output values are converted to POT events
                     and sent to the evHandler() method of any class object

   VirtualPotDemux - a VirtualPot whose output can to sent to up to 8 other 
                     VirtualPots or VirtualPotDemuxes 

4) The following example sketches have been added: 

      _70_Quantum_Oscillators
      _71_PureSquare
      _80_VirtualPot 
      _81_ParamSend 
      _82_PotSend
      _83_VirtualPotDemux 

5) a new synth, ZapShot, shows these new classes in action