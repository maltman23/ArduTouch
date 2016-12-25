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

   #define __FULLHOST__
// #define __LITEHOST__
// #define __STNDLONE__
// #define __BAREBONE__


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

   Depending on which model is chosen, a number of symbolic constants are
   defined below, which control the conditional compilation of library
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
