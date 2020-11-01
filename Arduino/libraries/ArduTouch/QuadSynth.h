/*
    QuadSynth.h  

    Declaration of the QuadSynth and related classes.

    ---------------------------------------------------------------------------
 
    Copyright (C) 2016, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
   ---------------------------------------------------------------------------
*/

#ifndef QUADSYNTH_H_INCLUDED
#define QUADSYNTH_H_INCLUDED

#include "Synth.h"

/* -------------------------------------------------------------------------- */

class XPanControl : public PanControl
{
   public:

   XPanControl( byte *panPos )            // constructor 
   {
      this->panPos = panPos;
   }
   
   PROMPT_STR( XPan ) 

} ;


/******************************************************************************
 *
 *                                QuadSynth 
 *
 ******************************************************************************/

class QuadSynth : public VoxSynth
{
   typedef VoxSynth super;                // VoxSynth is superclass

   public:

   void config();                         // configure synth 

   void output( char *, char * );         // write stereo output to pair of buffers

} ;                                     

/******************************************************************************
 *
 *                               QuadPanSynth 
 *
 ******************************************************************************/

class QuadPanSynth : public QuadSynth             
{
   typedef  QuadSynth super;              // superclass is QuadSynth

   protected:

   PanControl *panControl;                // dynamic controller of panPos

   Voice *pan[ 4 ];                       // voices in pan order

   byte  panPos;                          // pan position for voices pan[1/2] 

   public:

   void config();                         // configure synth 

   boolean charEv( char code );           // handle a character event
   void dynamics();                       // perform a dynamic update
   void output( char *, char * );         // write stereo output to pair of buffers
   void setMap( char *mapStr );           // set the voice panning order
   void setPanPos( byte panPos );         // set the pan position : pan[1/2]

   #ifdef KEYBRD_MENUS
   char menu( key k );                    // given a key, return a character
   #endif

} ;

/******************************************************************************
 *
 *                          QuadDualPanSynth 
 *
 ******************************************************************************/

class QuadDualPanSynth : public QuadPanSynth             
{
   typedef  QuadPanSynth super;           // superclass is QuadPanSynth

   protected:

   XPanControl *xpanControl;

   byte  xpanPos;                         // pan position for voices pan[0/3]

   public:

   void config();                         // configure synth 

   boolean charEv( char code );           // handle a character event
   void dynamics();                       // perform a dynamic update
   void output( char *, char * );         // write stereo output to pair of buffers
   void setXPanPos( byte xpanPos );       // set the xpan position : pan[0/3] 

   #ifdef KEYBRD_MENUS
   char menu( key k );                    // given a key, return a character
   #endif


} ;


#endif   // ifndef QUADSYNTH_H_INCLUDED
