/*
    Model.h  

    Declares which runtime model is in effect, and which optional features
    are enabled.

    There are 4 recognized runtime models:

         FULLHOST - two-way communications with host via serial port
         LITEHOST - one-way host-to-board communications via serial port 
         STNDLONE - "standalone" : no communications with host
         BAREBONE - no host communications, internal console, or keybrd menus

    Note: In the "barebones" runtime model, which has no internal console,
          only implicit system character events are processed. Any sketch 
          which relies on macros, or the processing of explicit character 
          events in ArduTouch base classes will not run properly.

    ---------------------------------------------------------------------------
 
    Copyright (C) 2016, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
   ---------------------------------------------------------------------------
*/

#ifndef MODEL_H_INCLUDED
#define MODEL_H_INCLUDED

// ---------  declare your model using one of the lines below  ---------------

// #define __FULLHOST__
// #define __LITEHOST__
   #define __STNDLONE__
// #define __BAREBONE__

// ---------        specify the serial baud rate with host     ---------------

   #define __BAUDRATE__ 115200

/* ---------------------------------------------------------------------------

    The ArduTouch library and sample synths were developed using Arduino build
    1.6.6. For large-scale sketches, build 1.6.6 generates smaller executables 
    than the current hourly build (1.8.6). If you are using build 1.6.6 you  
    should uncomment the following define.

   ------------------------------------------------------------------------ */

//#define BUILD_166

/* ---------------------------------------------------------------------------

    The following defines enable the dynamic monitoring of CPU usage and free 
    RAM. When enabled, the current values of these metrics are displayed to 
    the console (along with normal information) whenever '?' is typed at 
    the '>' prompt. Because monitoring consumes CPU and memory it should be 
    disabled when not needed for development. (Monitoring is automatically 
    disabled in all runtime models except __FULLHOST__.)

   ------------------------------------------------------------------------ */

//#define MONITOR_CPU                  // monitor % CPU used rendering audio
//#define MONITOR_RAM                  // monitor free RAM 

/* ---------------------------------------------------------------------------

    The following AUTO_METRICS define should only be used by developers who 
    need to determine the CPU usage and free RAM of a sketch not running in 
    FULLHOST model (and therefore unable to display them to the console).
    The metrics are automatically computed and stored in NVS at the address
    given by AUTO_METRICS_ADDR. These metrics can then be subsequently read 
    back and displayed to the console via the library routine readMetrics() 
    by any sketch running in FULLHOST model. When AUTO_METRICS is defined
    it will override the FULLHOST model, if defined, and force the model to 
    STNDLONE.

   ------------------------------------------------------------------------ */

//#define AUTO_METRICS               // for developers only!

#define AUTO_METRICS_ADDR 0          // address in NVS to store/read metrics  

/* ---------------------------------------------------------------------------

    The KEYBRD_MENUS define enables a "legacy" feature of the library 
    which allows classes derived from the Mode class to use the onboard 
    keyboard to generate character events, turning the keyboard into
    a defacto menu by which to drive the Console interface for an object. 
    This is an admittedly obscure feature, currently used only by the 
    DuoPoly synth (which was the 1st synth ever written for the ArduTouch).

    Because KEYBRD_MENUS consumes valuable resources but is rarely if ever 
    used it is considered a legacy feature which is now deprecated. 
    
    The KEYBRD_MENUS statement below is by default commented out, and should
    remain that way unless for some reason you want to explore the possibility 
    of running the Console from the onboard keyboard.

    You will need to define KEYBRD_MENUS if you want to compile the DuoPoly 
    synth.    
    
   ------------------------------------------------------------------------ */

//#define KEYBRD_MENUS                 // Modes have built-in keyboard menus

/* ---------------------------------------------------------------------------

    The IMPLICIT_SEQUENCER feature can be disabled by commenting out the
    define below. This will free up extra memory (both RAM and ROM) for 
    synths which do not use sequencers.

    Unless you have completely run out of memory for a specific synth 
    (that does not use sequencers) it is recommended that you leave 
    IMPLICIT_SEQUENCER defined.

   ------------------------------------------------------------------------ */

#define IMPLICIT_SEQUENCER           // Instruments may use built-in sequencers

/* ---------------------------------------------------------------------------
       The following lines insure that one and only one model is declared
   ------------------------------------------------------------------------ */

#ifndef __FULLHOST__
   #ifndef __LITEHOST__
      #ifndef __STNDLONE__
         #ifndef __BAREBONE__
            #error No Runtime Model has been declared 
         #endif
      #endif
   #endif
#endif

#ifdef __FULLHOST__
   #ifdef __LITEHOST__
      #error Multiple Runtime Models have been declared 
   #endif
   #ifdef __STNDLONE__
      #error Multiple Runtime Models have been declared 
   #endif
   #ifdef __BAREBONE__
      #error Multiple Runtime Models have been declared 
   #endif
#endif

#ifdef __LITEHOST__
   #ifdef __STNDLONE__
      #error Multiple Runtime Models have been declared 
   #endif
   #ifdef __BAREBONE__
      #error Multiple Runtime Models have been declared 
   #endif
#endif

#ifdef __STNDLONE__
   #ifdef __BAREBONE__
      #error Multiple Runtime Models have been declared 
   #endif
#endif

/* ---------------------------------------------------------------------------
       Disable resource monitoring in all models except __FULLHOST__.
   ------------------------------------------------------------------------ */

#ifndef __FULLHOST__
   #undef MONITOR_CPU               
   #undef MONITOR_RAM               
#endif

/* ---------------------------------------------------------------------------
       Insure proper model configuration if AUTO_METRICS is defined
   ------------------------------------------------------------------------ */

#ifdef AUTO_METRICS
   #define MONITOR_CPU
   #ifdef __FULLHOST__               // override FULLHOST model
      #undef   __FULLHOST__
      #define  __STNDLONE__
   #endif
#endif

/* ---------------------------------------------------------------------------

   Depending on which model is chosen, a number of symbolic constants are
   defined below, which control the conditional compilation of library 
   modules. None of the statements below should be changed.

   ------------------------------------------------------------------------ */

#define USE_SERIAL_PORT                
#define CONSOLE_OUTPUT    
#define INTERN_CONSOLE             

#ifdef __FULLHOST__
#endif

#ifdef __LITEHOST__
   #undef CONSOLE_OUTPUT               
#endif

#ifdef __STNDLONE__
   #undef CONSOLE_OUTPUT               
   #undef USE_SERIAL_PORT                
#endif

#ifdef __BAREBONE__
   #undef CONSOLE_OUTPUT               
   #undef USE_SERIAL_PORT                
   #undef INTERN_CONSOLE                 
   #undef KEYBRD_MENUS                 
#endif

#endif   // ifndef MODEL_H_INCLUDED
