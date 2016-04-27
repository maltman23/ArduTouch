/*
    Filter.h  

    Declaration of Filter classes.

    ---------------------------------------------------------------------------
 
    Copyright (C) 2016, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
   ---------------------------------------------------------------------------
*/

#ifndef BSFILTER_H_INCLUDED
#define BSFILTER_H_INCLUDED

#include "Effect.h"

class Filter : public Effect
{
   protected:

   boolean complement;              // if true, complement filter output

   public:

   void   charEv( char );           // process a character event
   void   info();                   // display object info to console
   char   menu( key );              // map key event to character 

} ;  // Filter


class BSFilter : public Filter
{
   static const byte MaxClip = 7;   // max # high bits to clip after shift

   int     last;                    // last output value   

   byte    shift;                   // # of bits to (left) shift last value
   byte    mltShift;                // equivalent multiplier for shift 

   byte    numClip;                 // # of high bits to clip after shift
   byte    clipMask;                // bitMask corresponding to numClip

   public:

   BSFilter(); 

   void   charEv( char );           // process a character event
   void   info();                   // display object info to console
   char   menu( key );              // map key event to character 
   void   process( char* );         // process an input buffer 
   char  *prompt();                 // return object's prompt string
   void   setClip( byte );          // set # bits to clip avg
   void   setShift( byte );         // set # bits to (left) shift

} ;  // BSFilter


class XMFilter : public Filter
{
   private:

   char  last;                      // last output value   
   byte  weight;                    // weighting (n/255) to input term 

   public:

   XMFilter(); 

   void   charEv( char );           // process a character event
   void   info();                   // display object info to console
   char   menu( key );              // map key event to character 
   void   process( char* );         // process an input buffer 
   char  *prompt();                 // return object's prompt string

} ;  // XMFilter

#endif   // ifndef BSFILTER_H_INCLUDED
