/*
    Tremolo.h  

    Standard Tremolo class.

    ---------------------------------------------------------------------------
 
    Copyright (C) 2018, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
   ---------------------------------------------------------------------------
*/

#ifndef TREMOLO_H_INCLUDED
#define TREMOLO_H_INCLUDED

#include "LFO.h"

/******************************************************************************
 *
 *                               Tremolo 
 *                                                                            
 ******************************************************************************/

class Tremolo : public TermLFO         // stock tremolo
{
   typedef TermLFO super;              // superclass is TermLFO

   public:

   Tremolo()
   {
      shortcut = 't';
   }

   PROMPT_STR( tremolo ) 

} ;

#endif   // ifndef TREMOLO_H_INCLUDED
