/*
    VirtualPot.h  

    Declaration of VirtualPot and related classes.

    ---------------------------------------------------------------------------
 
    Copyright (C) 2018, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
   ---------------------------------------------------------------------------
*/

#ifndef VIRTUALPOT_H_INCLUDED
#define VIRTUALPOT_H_INCLUDED
  
#include "Control.h"
#include "System.h"

/******************************************************************************
 *
 *                                VirtualPot 
 *                                                                            
 ******************************************************************************/

class VirtualPot : public Control
{
   typedef Control super;           // super class is Control

   protected:

   byte  pos;                       // current position
                                    
   byte  min;                       // minimum permitted value
   byte  max;                       // maximum permitted value

   byte  left;                      // leftmost pot value
   byte  right;                     // rightmost pot value

   double unit;                     // change in value per unit change in position

   public:

   bool charEv( char );             // process a character event
   byte evaluate( byte );           // returns value at specified pot position
   bool evHandler( obEvent );       // handle an onboard event

   void setMinMax( byte, byte );    // set minimum and maximum permitted values
   void setRange( byte, byte );     // set values for left & rightmost pot positions

   byte getPos()                    // return pot position
   {
       return pos; 
   }    

   byte getVal()                    // return pot value
   { 
      return evaluate( pos );
   }

   virtual void setPos( byte pos )  // set the pot position
   {
      this->pos = pos;
   }

   #ifdef CONSOLE_OUTPUT
   const char *prompt() { return CONSTR("virtPot"); }
   #endif
};

/******************************************************************************
 *
 *                              VirtualPotDemux
 *                                                                            
 ******************************************************************************/

class VirtualPotDemux : public VirtualPot
{
   typedef VirtualPot super;        // superclass is VirtualPot

   static const byte MaxLines = 8;  // connect to up to 8 VirtualPots

   VirtualPot *lineOut[ MaxLines ]; // each line pts to a VirtualPot or NULL

   byte  mask;                      // 1 bit per line: if set, line is enabled

   public:

   bool charEv( char );             // process a character event
   void connect(byte, VirtualPot*); // connect line to a VirtualPot (or NULL)
   void lineOn( byte );             // enable output for a line
   void lineOff( byte );            // disable output off a line
   void setPos( byte );             // set pot position (and propagate it)
};

/******************************************************************************
 *
 *                                ParamSend 
 *                                                                            
 ******************************************************************************/

#define ptr1ByteMethod( x ) reinterpret_cast<void (Mode::*)( byte )>( x )
                                                                        
class ParamSend : public VirtualPot
{
   typedef VirtualPot super;        // super class is VirtualPot

   protected:

   Mode *targObj;                   // ptr to target class instance
   void (Mode::*targMeth)( byte );  // ptr to target class method

   public:

   ParamSend()
   {
      targObj == NULL;
   }

   void setPos( byte pos )          // set pot position and send to target
   {
      super::setPos( pos );
      if (targObj) (targObj->*targMeth)( evaluate( pos ) );
   }

   // connect( ptrTargetObject, ptrTargetMethod )
   //
   // Establish a send connection to a target object's method.
   // 
   // Target object must be an instance of a class which has Mode as a base class.
   // Target method must take 1 arg of type byte and have a return type of void.
   // Use the ptr1ByteMethod() macro to simplify syntax for ptrTargetMethod.
   //
   // example (connect a ParamSend p to the setVol() method of a Voice v)
   //
   //    ParamSend p; 
   //    Voice     v; 
   //    p.connect( &v, ptr1ByteMethod( &Voice::setVol )

   void connect( Mode *targObj, void (Mode::*targMeth)(byte) )
   {
      this->targObj  = targObj;
      this->targMeth = targMeth;
   }

   PROMPT_STR( parmSend )
} ;

/******************************************************************************
 *
 *                                PotSend 
 *                                                                            
 ******************************************************************************/

class PotSend : public VirtualPot
{
   typedef VirtualPot super;        // super class is VirtualPot

   protected:

   Mode *targObj;                   // ptr to target class instance
   byte targPotNum;                 // pot# in targObj to send to

   public:

   PotSend()
   {
      targObj == NULL;
   }

   void setPos( byte pos )          // set pot position and send to target 
   {
      super::setPos( pos );

      if ( targObj && targPotNum < NumPots) 
      {
         obEvent o;                       // create a pot event
         o.setPotVal( evaluate( pos ) );
         o.setType( POT0 + targPotNum );   
         targObj->evHandler( o );         // send to target
      }
   }

   // connect( ptrTargetObject, targPotNum )
   //
   // Establish a send connection to a target object's method.
   // 
   // Target object must be an instance of a class which has Mode as a base class.
   //
   // example (connect PotSend p to the pot 0 of Voice v)
   //
   //    PotSend p; 
   //    Voice     v; 
   //    p.connect( &v, 0 )

   void connect( Mode *targObj, byte targPotNum )
   {
      this->targObj    = targObj;
      this->targPotNum = targPotNum;
   }

   PROMPT_STR( potSend )
} ;

#endif   // ifndef VIRTUALPOT_H_INCLUDED
