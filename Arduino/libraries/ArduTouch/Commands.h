/*
    Commands.h  

    Defines for all ASCII codes which are conventionally used as "commands"
    for objects derived from the Mode class, and which are sent to them via 
    their charEv() method.

    Command codes are by convention negative to avoid clashing with the codes
    of alphanumeric characters.

    ---------------------------------------------------------------------------
 
    Copyright (C) 2018, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
   ---------------------------------------------------------------------------
*/

#ifndef COMMANDS_H_INCLUDED
#define COMMANDS_H_INCLUDED

// commands for displaying object info to console:

#define chrInfo         -2             // display object info
#define chrInLnfo       -3             // display object info inline
#define chrBrief        -4             // display an object brief

// commands issued by console on change of focus:

#define focusPUSH       -5             // mode is being pushed on stack
#define focusPOP        -6             // mode is being popped from stack
#define focusRESTORE    -7             // mode is restored after pop of child

// commands to trigger/release a control:

#define chrTrigger      -8             // trigger control
#define chrRelease      -9             // release control

// LFO sends following commands to itself 

#define lfoIniPos       -10            // set initial LFO cycle position

// status messages sent by sequencers 

#define sqncPLAYON      -12            // sequencer playback on
#define sqncPLAYOFF     -13            // sequencer playback off 
#define sqncRECON       -14            // sequencer recording on
#define sqncRECOFF      -15            // sequencer recording off

#define sqncCUE         -16            // sequencer is cueing to record 
#define sqncDNBEAT      -17            // sequencer is at a downbeat
#define sqncUPBEAT      -18            // sequencer is at an upbeat 

// if a Control returns true for the following cmds then the associated frame LED is enabled

#define frameLED0       -19            // is frame LED0 enabled?
#define frameLED1       -20            // is frame LED1 enabled?

#endif   // ifndef COMMANDS_H_INCLUDED