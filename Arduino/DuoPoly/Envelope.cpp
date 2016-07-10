/*
    Envelope.cpp

    Implementation of the Envelope class.

    ---------------------------------------------------------------------------

    Copyright (C) 2016, Cornfield Electronics, Inc.

    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0
    Unported License.

    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/

    Created by Bill Alessi & Mitch Altman.

   ---------------------------------------------------------------------------
*/

#include "Console_.h"
#include "Envelope.h"

Envelope::Envelope()
{
   setScrollable(4);
}

boolean Envelope::charEv( char key )
{
   switch ( key )
   {
      case chrTrigger:                 // trigger the envelope

         phase = attPhase;
         if ( attack )
            level = 0.0;
         else
         {
            --phase;
            if ( decay )
               level = 1.0;
            else
            {
               --phase;
               level = susLevel;
            }
         }
         break;

      case chrRelease:                 // release the envelope

         if ( ! amMute() )
            phase = relPhase;
         break;

      #ifdef INTERN_CONSOLE
      case 'a':                        // set attack time

         console.getByte( CONSTR("attack"), &this->attack );
         setAttack( attack );
         break;

      case 'd':                        // set decay time

         console.getByte( CONSTR("decay"), &this->decay );
         setDecay( decay );
         break;

      case 'r':                        // set release time

         console.getByte( CONSTR("release"), &this->release );
         setRelease( release );
         break;

      case 's':                        // set sustain level

         console.getByte( CONSTR("sustain"), &this->sustain );
         setSustain( sustain );
         break;

      case '.':                        // mute

         level = 1.0;
         phase = finPhase;
         Control::charEv( key );
         break;

      case '<':                        // unMute

         level = susLevel;
         phase = finPhase;
         Control::charEv( key );
         break;
      #endif

      #ifdef CONSOLE_OUTPUT
      case chrInfo:                    // display object info to console

         TControl::charEv( chrInfo );
         console.infoByte( CONSTR("attack"), attack );
         console.infoByte( CONSTR("decay"), decay );
         console.infoByte( CONSTR("sustain"), sustain );
         console.infoByte( CONSTR("release"), release );
         break;
      #endif

      case '!':                        // reset

         phase = finPhase;
         setRelease(0);
         setSustain(255);
         setDecay(0);
         setAttack(0);

         // fall thru to Control handler

      default:

         return TControl::charEv( key );
   }
   return true;
}

void Envelope::dynamics()
{
   switch ( phase )
   {
      case attPhase:

         level += attStep;
         if ( level > 1.0 )
         {
            --phase;
            if ( decay )
               level = 1.0 - decStep;
            else
            {
               --phase;
               level = susLevel;
            }
         }
         break;

      case decPhase:

         level -= decStep;
         if ( level < susLevel )
         {
            --phase;
            level = susLevel;
         }
         break;

      case relPhase:

         if ( release )
         {
            level -= relStep;
            if ( level < 0.0 )
            {
               --phase;
               level = 0.0;
            }
         }
   }
}

boolean Envelope::evHandler( obEvent ev )
{
   if ( byte param = getScrollParm( ev ) )
   {
      byte val = ev.getPotVal();
      switch ( param )
      {
         case 1:  setAttack( val );   break;
         case 2:  setDecay( val );    break;
         case 3:  setSustain( val );  break;
         case 4:  setRelease( val );  break;
      }
     return true;
   }

   switch ( ev.type() )
   {
      case BUT0_PRESS:

         scrollUp();
         return true;

      case BUT1_PRESS:

         scrollDn();
         return true;

      default:

         return TControl::evHandler(ev);
   }
}

word Envelope::exptime( byte t )
{
   // expands t { 1..255 } pseudo-exponentially to { 1 .. 2411 }

   word nbufs = t;

   if ( t > 96 )
   {
      nbufs += ( t - 96 ) << 2;
      if ( t > 160 )
         nbufs += ( t - 160 ) << 4;
   }

   return nbufs;
}

#ifdef KEYBRD_MENUS
char Envelope::menu( key k )
{
   switch ( k.position() )
   {
      case  0: return 'a';
      case  2: return 'd';
      case  4: return 's';
      case  5: return 'r';
      default: return TControl::menu( k );
   }
}
#endif

#ifdef CONSOLE_OUTPUT
const char *Envelope::prompt()                     
{
   return CONSTR("envelope");
}
#endif

boolean Envelope::ready()
{
   if ( flags&LEGATO )
      return ( phase <= susPhase ? true : false );
   else
      return true;
}

void Envelope::setAttack( byte val )
{
   attack = val;
   if ( val )
      attStep = 1.0 / exptime( val );
}

void Envelope::setDecay( byte val )
{
   decay = val;
   if ( val )
      decStep = (1.0 - susLevel) / exptime( val );
}

void Envelope::setRelease( byte val )
{
   release = val;
   if ( val )
      relStep = 1.0 / exptime( val );
}

void Envelope::setSustain( byte val )
{
   #define ONE_256TH  .00390625

   sustain = val;
   if ( val == 255 )
      susLevel = 1.0;
   else
      susLevel = (double )val * ONE_256TH;
   setDecay( decay );

   #undef ONE_256TH
}