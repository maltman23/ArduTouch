/*
    Systems.h  

    Declarations for the Systems module

    ---------------------------------------------------------------------------
 
    Copyright (C) 2016, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
   ---------------------------------------------------------------------------
*/

#ifndef SYSTEM_H_INCLUDED
#define SYSTEM_H_INCLUDED

#include "Audio.h"

/* ----------------------     public functions     ------------------------- */

void device_io();                      // manage device I/O
void system_loop();                    // perform ongoing system tasks
void system_setup( Config * );         // initialize system resources

#endif   // ifndef SYSTEM_H_INCLUDED
