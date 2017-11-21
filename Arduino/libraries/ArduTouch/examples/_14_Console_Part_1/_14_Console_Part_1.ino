//
//  _14_Console_Part_1.ino
//
//  This sketch introduces the ArduTouch Console. 
//  
//  The ArduTouch Console provides a way of controlling your ArduTouch board
//  with your computer's keyboard, as well as providing a way to display info 
//  from your ArduTouch board.  (You don't need to use the Console, as all control
//  functionality is available from the onboard ArduTouch controls,
//  but the Console provides a really nice way to learn to control your ArduTouch,
//  since it gives the ability to display all sorts of useful feedback.)
//
//  (The ArduTouch Console also gives you the capability of using Macros,
//  which isn't possible using the onboard ArduTouch controls. But this is a topic 
//  for a later example.)
//
//  The ArduTouch Console is a library object which provides a serial interface
//  between a host computer and your synth. By running a terminal program (or using
//  the Arduino Serial Monitor) on the host to connect to the ArduTouch card you can 
//  interact with your synth using a "command-line" interface (in much the same way as 
//  you would interact with the operating system on a computer by typing commands in a 
//  "command" or terminal window). 
//
//  Each class in the ArduTouch library displays its own "command prompt" and 
//  responds to character input from the Console. There are standard Console 
//  routines for displaying and inputting the value of variables. 
//
//  In this introductory sketch the following topics will be covered:
//
//       1) How to compile your sketch to use the ArduTouch Console
//       2) How to configure a teriminal program to access the Console
//       3) How to use Arduino's Serial Port Monitor to access the Console
//       4) The layout of the Ardutouch Console display
//       5) The two types of Console prompts (Command and Input)
//       6) Character key equivalents for the [Enter] and [Esc] keys
//       7) The '?' Console command
//       8) The charEv() call-back method (for handling character events)
//       9) How to display the value of a variable to the Console
//      10) How to input the value of a variable from the Console
//      11) Using the CONSTR macro to place string constants in ROM
//
//  We will use the simple synth from example _03_ (which played a 
//  Triangle waveform at 440 Hz -- this time we'll load a Sine wave) and 
//  derive a new synth from it which allows the user to display and change 
//  the frequency of the oscillator from the host.
//
//  This sketch *requires* a Runtime Model of __FULLHOST__.
//  (please consult example _12_ on how to set the Runtime Model).
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

#include "ArduTouch.h"                       // use the ArduTouch library

//  ---------------------------------------------------------------------------
//
//  The ArduTouch Console is a built-in (but optional) feature of the firmware.
//  To include it in your sketch simply set the Runtime Model to  __FULLHOST__.
//
//  Once your sketch has been compiled and uploaded, the ArduTouch Console can
//  be accessed from the host computer either by using a terminal program (such 
//  as PuTTY under Windows or Linux), or by using the Arduino Serial Monitor. 
//
//  Although you can use either a terminal program or the Arduino Serial Monitor,
//  we strongly recommend using a terminal program.  (We'll explain more further down.)
//
//  If you use a terminal program you will need to know what serial port the 
//  ArduTouch is connected to. This can be determined in the Arduino IDE by 
//  clicking on [Tools > Port]. The terminal program should have its serial 
//  line settings configured as follows:
//
//          Data bits    : 8
//          Stop bits    : 1
//          Parity       : NONE
//          Flow Control : XON/XOFF
//          Speed        : same as that specified in set_baud_rate() below
//
//  If you use the Arduino Serial Monitor [Tools > Serial Monitor], you need to 
//  make sure that the baud rate shown in the combo box in the lower right-hand
//  corner of the Serial Monitor window is the same as that specified in 
//  set_baud_rate() below.
//
//  ---------------------------------------------------------------------------
 
about_program( Console_Part_1, 1.00 )        // specify sketch name & version
set_baud_rate( 115200 )                      // specify serial baud-rate

//  ---------------------------------------------------------------------------
//
//                    The Ardutouch Console Display Layout
//
//  When a serial connection is established to the ArduTouch card (either by 
//  starting a session on a terminal program, or opening the Arduino Serial 
//  Monitor) the ArduTouch card will reset, and the ArduTouch Console will 
//  display a title bar, followed by a prompt on the line below. The title bar
//  will contain the name and version  number specified in the about_program() 
//  statement above. For example, if this sketch has been uploaded to the 
//  ArduTouch card, the terminal window will appear as follows after a reset:
//
//  _______________________________________________________________________
//
//           Console_Part_1 [1.00]
//    synth> 
//
//  _______________________________________________________________________
//
//
//  The Console display is divided vertically by a(n invisible) "seam". 
//  Prompts are displayed to the left of the seam, and everything else 
//  to the right.
// 
//  ---------------------------------------------------------------------------
//
//                       The Two Types of Console Prompts
//
//  When using the ArduTouch Console from the host you will be presented with
//  two different types of prompts, depending on the situation. 
//
//                            The > (Command) Prompt
//
//  The normal prompt will end with the '>' character such as "synth>" in 
//  the above example. The '>' at the end of the prompt indicates that the 
//  system is waiting to respond to *individual keystrokes*. Each key you press 
//  will immediately be processed by the system, and generally corresponds to 
//  a single "command". If you are using the Arduino Serial Monitor you will 
//  need to hit the [Enter] key after each key you type at the '>' prompt, 
//  in order for the key to be sent to the Console (ugh -- kind of a bummer).
//  
//                             The : (Input) Prompt
//  
//  When a sketch needs the user to input a value, a prompt is displayed which
//  ends with the ':' character. The ':' at the end of the prompt indicates that 
//  the system will not process any keystrokes until a carriage return is typed,
//  at which time all the characters typed will be processed as a single string.
//  If you make a mistake while typing at the input prompt you can abort your
//  input by hitting the [Esc] key.
//
//         Character Key Equivalents for [Enter] and [Esc] keys: \ and `
//
//  The Arduino Serial Monitor does not send carriage returns through to the 
//  serial port (this is a drag for us). To get around this the ArduTouch Console 
//  interprets the '\' character as an [Enter] key. When using the Arduino 
//  Serial Monitor you will need to terminate your input strings with '\' 
//  (and then hit [Enter] to send the string).
//
//  The Arduino Serial Monitor does not send [Esc] keys through to the serial 
//  port (ugh). To get around this the ArduTouch Console interprets the '`' character 
//  as an [Esc] key. If you want to abort an input when using the Arduino 
//  Serial Monitor you will need to type '`' (and then hit [Enter] to send it).
//
//  One positive aspect (yea!) to the Arduino Serial Monitor is that you can
//  edit your input string before sending it. When using a terminal program,
//  if you make a mistake while typing an input value there is no going back 
//  to fix it (edit keys don't work) so the only way out is to abort the input 
//  (via the [Esc] or '`' keys) and try again.
//
//                               The ? Command
//
//  Whenever you are presented with a command prompt (a prompt ending with '>')
//  you can type '?', and this will cause relevant info to be displayed to the
//  Console regarding the object that issued the prompt (your synth in this 
//  sketch, but in subsequent examples we will see it can be any of the 
//  components that make up your synth).
//
//  ---------------------------------------------------------------------------

//  If the Runtime Model is not set to __FULLHOST__ the following statement 
//  will cause the compiler to abort, and to print an error message in the 
//  Arduino output window.

#ifndef __FULLHOST__
   #error This sketch requires a Runtime Model of __FULLHOST__
#endif

// Sine440HzSynth plays a Sine wave at 440 Hz 

class Sine440HzSynth : public Synth          // synth from example _03_, 
{                                            // using a Sine wave 
   public:

   WaveOsc  osc;                             // use wave-table oscillator

   void setup() 
   { 
      osc.setTable( wavetable( Sine ) );     // load Sine wave from library
      osc.setFreq( 440.0 );                  // play at 440 Hz
   }

   void output( char *buf )                  // output 1 buffer of audio
   {
      osc.output( buf );
   }
} ;                                           

// ConsoleSynth adds the ability to display and change the frequency of the 
// Sine wave from the Console.

class ConsoleSynth : public Sine440HzSynth
{
   //  -----------------------------------------------------------------------
   //
   //                       Calling Console Functions
   //
   //  The ArduTouch Console is instantiated as a library object with the 
   //  variable name console:
   //
   //    extern Console console;
   //
   //  Console functions are invoked using the standard C++ syntax for 
   //  structure member access, i.e., to print "hello, world":
   //
   //    console.print( "hello, world\n" );  
   //
   //  -----------------------------------------------------------------------

   public:

   // charEv() is an important call-back method. The characters you type on 
   // the host keyboard (when you are in the Console window) are transmitted
   // by the system so that it can be handled by the charEv() method of your synth.
   // In certain cases the system may issue special characters to charEv()
   // that were not explicitly typed into the Console (more on this later).
   //
   // charEv() returns a boolean indicating whether the character was handled.

   boolean charEv( char code )               // handle a character event
   {
      // Our method will process two character events: 
      //
      //    1) an 'f' character event will be handled by displaying a prompt 
      //       for a new oscillator frequency. If a frequency is input by the 
      //       user, then the oscillator's frequency will be set to that value.
      //     
      //    2) a chrInfo event (this is a special character event issued by the 
      //       system when a '?' is typed in the console window) will be handled 
      //       by displaying the current oscillator frequency.
      //
      //  ---------------------------------------------------------------------
      //
      //                   A Note Regarding String Constants
      //  
      //  Arduino has the unsavory habit of storing constants in RAM. In the
      //  case of strings (which the Console makes frequent use of) this can  
      //  chew up RAM quite quickly. To circumvent this the ArduTouch library 
      //  provides a macro CONSTR("your constant string here"), which places
      //  the string in ROM, and returns a pointer to it.
      //
      //  Important! CONSTR should only be used for string constants that are
      //  intended solely for display to the Console. If your string is meant 
      //  for any other purpose, and you want to place it in ROM, then use the
      //  Arduino environment's PSTR() macro instead.
      //
      //  ---------------------------------------------------------------------

      switch( code )
      {
         // The Console provides 5 methods for inputting data values of various
         // types (unsigned and signed byte, int, double, unsigned long). These 
         // methods are listed below. They all take two arguments. The 1st argument 
         // for  each methos is a prompt string to display (and must be a constant). 
         // The 2nd argument is the address of a variable to write the input to. The 
         // return type of each method is a boolean which indicates whether the 
         // input was successfully completed (the user may have escaped out from 
         // the input before completing it). If the return value is false, then 
         // the variable pointed to by the 2nd argument is not over-written (i.e., 
         // its original value is preserved).
         //
         //    boolean getByte( const char*, byte* );      // input an unsigned byte
         //    boolean getSByte( const char*, char* );     // input a signed byte
         //    boolean getInt( const char*, int* );        // input an int
         //    boolean getDouble( const char*, double* );  // input a double
         //    boolean getULong( const char*, unsigned long* ); // input an unsigned long

         case 'f':                           // set oscillator frequency
         {
            double newFreq;
            if ( console.getDouble( CONSTR("freq"), &newFreq ) )
               osc.setFreq( newFreq );
            return true;
         }

         // The Console provides 5 methods for displaying values of various
         // types (unsigned and signed byte, int, double, unsigned long). These 
         // methods  are listed below. The 1st argument for each method is a 
         // label string to display (and must be a constant). The 2nd argument
         // is the value to be displayed.
         //
         //    void infoByte( const char*, byte );       // display an unsigned byte
         //    void infoSByte( const char*, char );      // display a signed byte
         //    void infoInt( const char*, int );          // display an int
         //    void infoDouble( const char*, double );    // display a double
         //    void infoULong( const char*, unsigned long );  // display an unsigned long

         case chrInfo:                       // display object info

            console.rtab();                  // tab to right of console seam
            console.infoDouble( CONSTR("freq"), osc.getFreq() );  
            return true;

         default:

            return Sine440HzSynth::charEv( code );
      }
   }

} myConsoleSynth;


void setup()
{
   ardutouch_setup( &myConsoleSynth );       // initialize ArduTouch resources
}

void loop()
{
   ardutouch_loop();                         // perform ongoing ArduTouch tasks
}

//  ---------------------------------------------------------------------------
//
//   To see the above charEv() in action, after you compile and upload this 
//   sketch to the ArduTouch, try setting the frequency of the oscillator 
//   from the host to, say, 216.35 Hz. You will hear the pitch change. Then 
//   display the frequency of the oscillator. 
//
//   To do this from a terminal program on the host, start the 
//   terminal program. Then type '?' to see the default frequency.
//   Then type 'f' (for set frequency). A "freq:" prompt will appear.
//   Then type "216.35" followed by [Enter]. 
//   When a fresh prompt appears type '?' to display the oscillator's new frequency. 
//   The Console window should look as follows:
//
//  _______________________________________________________________________
//
//           Console_Part_1 [1.00]
//    synth> ?
//           {freq 440.00}
//    synth> f
//     freq: 216.35
//    synth> ?
//           {freq 216.35}
//    synth>
//  _______________________________________________________________________
//
//
//   To do the same from the Arduino Serial Monitor:
//   Start the monitor
//      [Tools > Serial Monitor]
//   then type '?' followed by [Enter] to display the oscillator's default frequency. 
//   Then type 'f' (for set frequency) followed by [Enter]. 
//   A "freq:" prompt will appear in the output window.
//   Then type "216.35\" followed by [Enter]. 
//   When a fresh prompt appears in the output window, 
//   type '?' followed by [Enter] to display the oscillator's new frequency. 
//   The Console window should look as follows:
//
//  _______________________________________________________________________
//
//           Console_Part_1 [1.00]
//    synth> ?
//           {freq 440.00}
//    synth> f
//     freq: 216.35\
//    synth> ?
//           {freq 216.35}
//    synth>
//  _______________________________________________________________________
//
//  ---------------------------------------------------------------------------
//
//             A Note About Using the Arduino Serial Monitor
//
//  The Arduino Serial Monitor is pretty cumbersome for controlling the ArduTouch.
//  We strongly recommend using a terminal program (such as PuTTY).
//  The Arduino Serial Monitor requires that you hit the [Enter] key before any 
//  characters are sent to the ArduTouch.  This is cumbersome.  (blah)
//  If you use a terminal program (such as PuTTY), every character you type from 
//  your computer's keyboard is instantly sent to the ArduTouch. This feels way 
//  better to use.
//
//  ---------------------------------------------------------------------------


