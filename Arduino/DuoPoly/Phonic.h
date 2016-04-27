/*
    Phonic.h  

    Declaration of the Phonic, MonoPhonic and StereoPhonic classes.

    ---------------------------------------------------------------------------
 
    Copyright (C) 2016, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
   ---------------------------------------------------------------------------
*/

#ifndef PHONIC_H_INCLUDED
#define PHONIC_H_INCLUDED

#include "Control.h"

class Phonic : public Control
{
   public:

   void charEv( char code )
   {
      Control::charEv( code );

      if ( code == '!' )                  // reset
         setMute( false );
   }

} ;

class MonoPhonic : public Phonic
{
   public:

   virtual void  generate(char*) {};      // generate one buffer of audio

} ;

class StereoPhonic : public Phonic
{
   public:

   virtual void generate(char*, char*) {}; // generate one stereo buffer of audio
} ;

#endif   // ifndef PHONIC_H_INCLUDED
