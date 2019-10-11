/*
    Envelope.h  

    Declaration of envelope classes.

    Two classes are available:

      ADSR     - traditional Attack / Decay / Sustain / Release envelope.

      AutoADSR - an ADSR with a fixed sustain time. The release stage occurs 
                 automatically after the sustain stage has finished, so that 
                 once triggered this envelope runs through the ADSR sequence 
                 without regards to any incoming release event.

    ---------------------------------------------------------------------------
 
    Copyright (C) 2016, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
   ---------------------------------------------------------------------------
*/

#ifndef ENVELOPE_H_INCLUDED
#define ENVELOPE_H_INCLUDED

#include "Console_.h"
#include "Control.h"
#include "Scroller.h"

/******************************************************************************
 *
 *                                  ADSR 
 *
 ******************************************************************************/

class ADSR : public Factor, public Scroller
{
   typedef Factor super;         // superclass is Factor

   public:

   ADSR()
   {
      setScrollable(4);
      shortcut = 'e';
   }

   boolean charEv( char );       // process a character event
   void    dynamics();           // update object dynamics
   boolean evHandler( obEvent ); // handle an onboard event
   void    finish();             // set phase to final ("done")

   byte    getAttack()  { return attack; }   // get attack time
   byte    getDecay()   { return decay; }    // get decay time
   byte    getSustain() { return sustain; }  // get sustain level
   byte    getRelease() { return relTime; }  // get release time

   void    setAttack( byte );    // set attack time
   void    setDecay( byte );     // set decay time
   void    setSustain( byte );   // set sustain level
   void    setRelease( byte );   // set release time

   #ifdef KEYBRD_MENUS
   char    menu( key );          // given a key, return a character 
   #endif

   PROMPT_STR( envADSR ) 

   protected:

   byte    attack;               // attack time
   byte    decay;                // decay time
   byte    sustain;              // sustain level
   byte    relTime;              // release time

   double  attStep;              // change in attack per dynamic update
   double  decStep;              // change in decay per dynamic update
   double  susLevel;             // sustain level
   double  relStep;              // change in release per dynamic update

   byte    phase;                // current phase of envelope
                                 // enumerated values for phase:

   static const byte attPhase = 4;  
   static const byte decPhase = 3;
   static const byte susPhase = 2;
   static const byte relPhase = 1;
   static const byte finPhase = 0;

   word    exptime( byte );      // expand time to # of dynamic updates

} ;

/******************************************************************************
 *
 *                                AutoADSR 
 *
 ******************************************************************************/

class AutoADSR : public ADSR     // ADSR with sustain time, automatic release 
{
   typedef ADSR super;           // superclass is ADSR

   public:

   AutoADSR()
   {
      setScrollable(5);
   }

   bool  charEv( char );         // process a character event
   void  dynamics();             // update object dynamics
   bool  evHandler( obEvent );   // handle an onboard event

   byte  getSusTime() { return susTime; }    // get sustain time

   void  setSusTime( byte );     // set sustain time

   PROMPT_STR( envAADSR ) 

   protected:

   byte  susTime;                // sustain time
   word  susExpTime;             // sustain time (expanded pseudo-exponentially)
   word  susDC;                  // downcounter to end of sustain

} ;

/******************************************************************************
 *
 *                                MasterADSR 
 *
 ******************************************************************************/

#define MAXSLAVES 4              // MasterADSR can control up to this many slaves

class MasterADSR : public Control // Master envelope control for multiple slaves
{
   typedef Control super;        // superclass is Control

   protected:

   byte  maxSlave;               // max valid index for slave[] (-1 means "no slaves")
   ADSR *slave[ MAXSLAVES ];     // ptrs to slave envelopes

   virtual void copyParams();    // copy slave 0 envelope parameters to other slaves

   public:

   MasterADSR()
   {
      maxSlave = -1;             // no slaves
   }
                                           
   bool  charEv( char );         // process a character event
   void  charParam( char );      // process a char event that sets an envelope param
   byte  getAttack();            // get envelope attack time
   byte  getDecay();             // get envelope decay time
   byte  getRelease();           // get envelope release time
   byte  getSustain();           // get envelope sustain level
   void  setAttack( byte );      // get envelope attack time
   void  setDecay( byte );       // get envelope decay time
   void  setNumSlaves( char );   // set the number of slave envelopes
   void  setRelease( byte );     // get envelope release time
   void  setSlave( byte, ADSR* );// register a slave envelope
   void  setSustain( byte );     // get envelope sustain level

   PROMPT_STR( MastEnv ) 

} ;

/******************************************************************************
 *
 *                              MasterAutoADSR 
 *
 ******************************************************************************/

class MasterAutoADSR : public MasterADSR
{
   typedef MasterADSR super;     // superclass is MasterADSR

   void copyParams();            // copy slave 0 envelope parameters to other slaves

   public:

   bool  charEv( char );         // process a character event
   byte  getSusTime();           // get envelope sustain time
   void  setSusTime( byte );     // set envelope sustain time

} ;

#endif   // ifndef ENVELOPE_H_INCLUDED
