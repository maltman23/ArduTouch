/*
    Model.h  

    Declares which runtime model is in effect.

    There are 4 recognized runtime models:

         FULLHOST - two-way communications with host via serial port
         LITEHOST - one-way host-to-board communications via serial port 
         STNDLONE - "standalone" : no communications with host
         BAREBONE - no host communications, internal console, or keybrd menus

    Note: In the "barebones" runtime model, which has no internal console,
          only implicit system character events are processed. Any sketch 
          which relies on macros, or expects coreLib base classes to process
          explicit character events should be run in STNDLONE model or above.

    Note: Onboard LEDs are not functional in FULLHOST and LITEHOST models.

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

// ---------  define your model using one of the lines below  ----------------

                        #define __FULLHOST__
//                      #define __LITEHOST__
//                      #define __STNDLONE__
//                      #define __BAREBONE__

/* ---------------------------------------------------------------------------

   Depending on which model is chosen, a number of symbolic constants are
   defined below, which control the conditional compilation of coreLib 
   modules. None of the statements below should be changed, with the 
   exception of "#define KEYBRD_MENUS", which can be commented out to 
   save space in sketches which have no need for keyboard menus.

   ------------------------------------------------------------------------ */

#define USE_SERIAL_PORT                
#define CONSOLE_OUTPUT    
#define INTERN_CONSOLE             
#define KEYBRD_MENUS                 // comment this out for no keybrd menus

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
