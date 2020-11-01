/*
    Control.cpp

    Implementation of the Control classes.

    ---------------------------------------------------------------------------
 
    Copyright (C) 2016, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
   ---------------------------------------------------------------------------
*/

#include "System.h"
#include "Control.h"
#include "Console_.h"

/******************************************************************************
 *
 *                                  Control
 *
 ******************************************************************************/

/*----------------------------------------------------------------------------*
 *
 *  Name:  Control::charEv
 *
 *  Desc:  Process a character event.
 *
 *  Args:  code             - character to process
 *
 *  Rets:  status           - if true, event was handled
 *
 *  Memb: +flags.MUTE       - if set, control is currently muted 
 *         flags.RSTMUTE    - if set, control will be muted upon reset
 *        +frame            - embedded user interface frame
 *
 *----------------------------------------------------------------------------*/      

boolean Control::charEv( char code )
{
   switch ( code )
   {
      case '<':                  // unmute control

         flags &= ~MUTE;
         break;

      case '.':                  // mute control

         flags |= MUTE;
         break;

      case '!':                  // reset control

         setMute( flags&RSTMUTE );

         frame.led0 = 0;         // clear frame state
         frame.led1 = 0;

         break;

      case focusPUSH:            // display Frame LEDs on push

         displayLED(0);
         displayLED(1);
         break;

      #ifdef CONSOLE_OUTPUT

      case 'F':                  // set Frame via character string input
      {
         char *frameStr;
         if ( console.getStr( CONSTR("Frame"), &frameStr ) )
         {
            if ( frameStr[0] >= '0' && frameStr[0] <= '0'+frame.dim0 )
               frame.led0 = frameStr[0]-'0';
            else
               break;

            if ( frameStr[1] >= '0' && frameStr[1] <= '0'+frame.dim1 )
               frame.led1 = frameStr[1]-'0';
            else
               break;
         }
         break;
      }

      case chrBrief:             // display control brief to console

         super::charEv( chrBrief );
         console.space();
         console.print( (char)(muted() ? '.' : '<') );
         break;

      case chrInfo:              // display control info to console

         console.rtab();
         console.print( (char)(muted() ? '.' : '<') );
         console.space();
         if ( flags & UIFRAME )
         {
            char frameStr[3];

                 frameStr[0] = '0' + frame.led0;
                 frameStr[1] = '0' + frame.led1;
                 frameStr[2] =  0;

            console.infoRAMStr( CONSTR("Frame"), &frameStr[0] );
            console.newlntab();
         }
         break;

      #endif

      default:

         return super::charEv( code );
   }
   return true;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Control::displayLED
 *
 *  Desc:  Display a frame LED.
 *
 *  Args:  nth              - LED #
 *
 *  Memb:  flags.UIFRAME    - if set, frame is enabled
 *         frame            - embedded user interface frame
 *
 *----------------------------------------------------------------------------*/      

void Control::displayLED(byte nth)
{
   #ifdef USE_LEDS

   if ( ! (flags & UIFRAME) )          // do nothing if frame is not enabled
      return;

   byte stateLED;
   
   if ( nth )                          // display LED1
   {
      stateLED = frame.led1;
      if ( ! charEv( frameLED1 ) )     // do nothing if LED1 is not enabled 
         return;
   }
   else                                // display LED0
   {
      stateLED = frame.led0;
      if ( ! charEv( frameLED0 ) )     // do nothing if LED0 is not enabled 
         return;
   }

   switch ( stateLED )
   {
      case 0:    offLED( nth );  break;
      case 1:     onLED( nth );  break;
      case 2:  blinkLED( nth );  break;
   }

   #endif
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Control::evHandler
 *
 *  Desc:  Handle an onboard event.
 *
 *  Args:  ev               - onboard event
 *
 *  Memb:  -- none -- 
 *
 *  Rets:  status           - true if the event was handled
 *
 *----------------------------------------------------------------------------*/      

bool Control::evHandler( obEvent ev )
{
   switch ( ev.type() )
   {
      case BUT0_PRESS:                 // bump column 0 of frame

         ++frame.led0;
         if ( frame.led0 > frame.dim0 )
            frame.led0 = 0;
         displayLED(0);
         break;

      case BUT1_PRESS:                 // bump column 1 of frame

         ++frame.led1;
         if ( frame.led1 > frame.dim1 )
            frame.led1 = 0;
         displayLED(1);
         break;

      default:

         return super::evHandler(ev);
   }
   return true;
}

#ifdef KEYBRD_MENUS

/*----------------------------------------------------------------------------*
 *
 *  Name:  Control::menu
 *
 *  Desc:  Given a key, return a character (to be processed via charEv()). 
 *
 *  Args:  k                - key
 *
 *  Rets:  c                - character (0 means "no character")
 *
 *  Note:  If a sketch is compiled with KEYBRD_MENUS defined, then this method 
 *         can be used to map the onboard keys to characters which the system 
 *         will automatically feed to the charEv() method.
 *
 *         This method is only called by the system if the MENU flag in this
 *         object is set (in the ::flags byte inherited from Mode), or if the
 *         keyboard is in a "oneShot menu selection" state.
 *
 *----------------------------------------------------------------------------*/      

char Control::menu( key k )
{
   switch ( k.position() )
   {
      case  9: return '<';
      case 10: return '.';
      case 11: return '!';
      default: return 0;
   }
}

#endif

/*----------------------------------------------------------------------------*
 *
 *  Name:  Control::reset
 *
 *  Desc:  Reset the control.
 *
 *  Note:  This method is a wrapper for charEv('!'), and thus a defacto 
 *         virtual routine. A derived class can modify the behaviour of
 *         reset() by handling the '!' character event.
 *
 *----------------------------------------------------------------------------*/      

void Control::reset()
{
   charEv('!');
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Control::setMute
 *
 *  Desc:  Set the mute status of the control.
 *
 *  Note:  This method is a wrapper for charEv('.') and charEv('<'), and thus 
 *         a defacto virtual routine. A derived class can modify the behaviour 
 *         of setMute() by handling the '.' and '<' character events.
 *
 *----------------------------------------------------------------------------*/      

void Control::setMute( boolean mute )
{
   charEv( mute ? '.' : '<' );
}

/******************************************************************************
 *
 *                                 TControl
 *
 ******************************************************************************/

/*----------------------------------------------------------------------------*
 *
 *  Name:  TControl::charEv
 *
 *  Desc:  Process a character event.
 *
 *  Args:  code             - character to process
 *
 *  Rets:  status           - if true, event was handled
 *
 *  Memb: +flags.DONE       - control is ready to be triggered
 *        +flags.LEGATO     - control uses legato re-triggering 
 *
 *----------------------------------------------------------------------------*/      

boolean TControl::charEv( char code )
{
   switch ( code )
   {
      #ifdef INTERN_CONSOLE
      case '~':                        // set legato triggering

         flags   |= (LEGATO|DONE);            
         break;

      case '\'':                       // set stacato triggering

         flags   &= ~LEGATO;           
         break;
      #endif

      #ifdef CONSOLE_OUTPUT
      case chrBrief:                   // display brief obj info to console
      case chrInfo:                    // display obj info to console

         super::charEv( code );
         console.print( ( flags & LEGATO ) ? '~' : '\'' );
         if ( code == chrInfo ) console.space();
         break;
      #endif

      case '!':                        // reset

         flags &= ~LEGATO;             // "staccato"

         // fall thru to parent 

      default:

         return super::charEv( code );
   }
   return true;
}

#ifdef KEYBRD_MENUS

/*----------------------------------------------------------------------------*
 *
 *  Name:  TControl::menu
 *
 *  Desc:  Given a key, return a character (to be processed via charEv()). 
 *
 *  Args:  k                - key
 *
 *  Rets:  c                - character (0 means "no character")
 *
 *  Note:  If a sketch is compiled with KEYBRD_MENUS defined, then this method 
 *         can be used to map the onboard keys to characters which the system 
 *         will automatically feed to the charEv() method.
 *
 *         This method is only called by the system if the MENU flag in this
 *         object is set (in the ::flags byte inherited from Mode), or if the
 *         keyboard is in a "oneShot menu selection" state.
 *
 *         The key mapping (inclusive of super class) is as follows:
 *
 *           -------------------------------------------------
 *           |   |   |   |   |   |   |   |   |   |   |   |   |
 *           |   |   |   |   |   |   |   |   |   |   |   |   |
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |   |   |   |   |   |   |   |   | ' |   | . |   | 
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |    ___     ___    |    ___     ___     ___    | 
 *           |     |       |     |     |       |       |     |
 *           |     |       |     |     |       |       |     |
 *           |     |       |     |     |   ~   |   <   |  !  |
 *           |     |       |     |     |       |       |     |
 *           |     |       |     |     |       |       |     |
 *           -------------------------------------------------
 *
 *----------------------------------------------------------------------------*/      

char TControl::menu( key k )
{
   switch ( k.position() )
   {
      case  7: return '~';
      case  8: return '\'';
      default: return super::menu( k );
   }
}

#endif

/*----------------------------------------------------------------------------*
 *
 *  Name:  TControl::release
 *
 *  Desc:  Execute a release.
 *
 *  Note:  This method is a wrapper for charEv( chrRelease ), and thus a 
 *         defacto virtual routine. A derived class can modify the behaviour 
 *         of release() by handling the chrRelease character event.
 *
 *----------------------------------------------------------------------------*/      

void TControl::release()
{
   if ( ! muted() ) 
      charEv( chrRelease );
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  TControl::trigger
 *
 *  Desc:  Execute a trigger.
 *
 *  Note:  This method is a wrapper for charEv( chrTrigger ), and thus a 
 *         defacto virtual routine. A derived class can modify the behaviour 
 *         of trigger() by handling the chrTrigger character event.
 *
 *----------------------------------------------------------------------------*/      

void TControl::trigger()
{
   if ( muted() || ( (flags&(LEGATO|DONE)) == LEGATO ) ) 
      return;

   charEv( chrTrigger );
}

/******************************************************************************
 *
 *                               ControlChain
 *
 ******************************************************************************/

/*----------------------------------------------------------------------------*
 *
 *  Name:  ControlChain::add
 *
 *  Desc:  Add a control to the chain.
 *
 *  Args:  control          - ptr to control to append
 *
 *  Memb:  chain            - pts to 1st control in chain
 *
 *  Note:  The control is appended to the end of the chain.
 *
 *----------------------------------------------------------------------------*/      

void ControlChain::add( TControl *x )     
{
   if ( chain )
   {
      TControl *e = chain;
      while ( e->next )
         e = e->next;
      e->next = x;
   }
   else
      chain = x;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  ControlChain::charEv
 *
 *  Desc:  Process a character event.
 *
 *  Args:  code             - character to process
 *
 *  Rets:  status           - if true, event was handled
 *
 *  Memb:  chain            - pts to 1st control in chain
 *
 *----------------------------------------------------------------------------*/      

boolean ControlChain::charEv( char code )
{
   boolean found = false;             // true if code is an ordinal or shortcut
   TControl *e = chain;

   #ifdef INTERN_CONSOLE

   // if code is '0' thru '8' interpret it as a 0-based ordinal and push 
   // the corresponding control in the chain. 

   if ( code >= '0' && code <= '8' )  // code is a 0-based ordinal
   {
      signed char nth = code - '0';
      while ( e && nth > 0 )
      {
         e = e->next;
         --nth;
      }
      if ( e ) 
      {
         found = true;
         console.pushMode( e );
      }
   }

   // check to see if code is a shortcut for a control in the chain, and if
   // so push that control. 

   else                               // code may be a shortcut letter

   {
      while ( e )
      {
         if ( e->shortcut == code )
         {
            found = true;
            console.pushMode( e );
            break;
         }
         e = e->next;
      }
   }

   #endif

   if ( found )                       // return if code already processed
      return true;
   else
   {
      switch ( code )
      {
         case '.':                    // mute all controls
         case '<':                    // unmute all controls
         case '!':                    // reset all controls

         {
            TControl *e = chain;
            while ( e )
            {
               e->charEv( code );
               e = e->next;
            }
            break;
         }

         #ifdef CONSOLE_OUTPUT
         case chrBrief:               // display brief info 
         {
            console.space();
            super::charEv( chrBrief );
            console.space();
            TControl *e = chain;
            while ( e )
            {
               e->brief();
               e = e->next;
            }
            break;
         }
         #endif

         #ifdef CONSOLE_OUTPUT
         case chrInfo:                // display full info
         {
            console.rtab();
            TControl *e = chain;
            while ( e )
            {
               e->brief();
               console.space();
               e = e->next;
            }
            break;
         }
         #endif

         default:

            return super::charEv( code );
      }
      return true;
   }

}

/*----------------------------------------------------------------------------*
 *
 *  Name:  ControlChain::clear
 *
 *  Desc:  Remove all controls from the controls chain.
 *
 *  Memb:  chain            - pts to 1st control in chain
 *
 *----------------------------------------------------------------------------*/      

void ControlChain::clear()
{
   chain = NULL;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  ControlChain::dynamics
 *
 *  Desc:  Perform a dynamic update on all controls in the controls chain.
 *
 *  Memb:  chain            - pts to 1st control in chain
 *
 *----------------------------------------------------------------------------*/      

void ControlChain::dynamics()
{
   TControl *e = chain;
   while ( e )
   {
      e->dynamics();
      e = e->next;
   }
}

#ifdef KEYBRD_MENUS

/*----------------------------------------------------------------------------*
 *
 *  Name:  ControlChain::menu
 *
 *  Desc:  Given a key, return a character (to be processed via charEv()). 
 *
 *  Args:  k                - key
 *
 *  Rets:  c                - character (0 means "no character")
 *
 *  Note:  If a sketch is compiled with KEYBRD_MENUS defined, then this method 
 *         can be used to map the onboard keys to characters which the system 
 *         will automatically feed to the charEv() method.
 *
 *         This method is only called by the system if the MENU flag in this
 *         object is set (in the ::flags byte inherited from Mode), or if the
 *         keyboard is in a "oneShot menu selection" state.
 *
 *         The key mapping is as follows:
 *
 *           -------------------------------------------------
 *           |   |   |   |   |   |   |   |   |   |   |   |   |
 *           |   |   |   |   |   |   |   |   |   |   |   |   |
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |   | 1 |   | 3 |   |   | 6 |   | 8 |   | . |   | 
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |    ___     ___    |    ___     ___     ___    | 
 *           |     |       |     |     |       |       |     |
 *           |     |       |     |     |       |       |     |
 *           |  0  |   2   |  4  |  5  |   7   |   <   |  !  |
 *           |     |       |     |     |       |       |     |
 *           |     |       |     |     |       |       |     |
 *           -------------------------------------------------
 *
 *----------------------------------------------------------------------------*/      

char ControlChain::menu( key k )
{
   switch ( k.position() )
   {
      case 11: return '!';
      case 10: return '.';
      case  9: return '<';
      default: return ( '0' + k.position() );
   }
}
#endif

#ifdef CONSOLE_OUTPUT
const char *ControlChain::prompt() 
{ 
   return CONSTR("controls"); 
}
#endif

/*----------------------------------------------------------------------------*
 *
 *  Name:  ControlChain::reset
 *
 *  Desc:  Reset all controls in the chain.
 *
 *----------------------------------------------------------------------------*/      

void ControlChain::reset()
{
   charEv('!');
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  ControlChain::release
 *
 *  Desc:  Perform a trigger release on all controls in the controls chain.
 *
 *  Memb:  chain            - pts to 1st control in chain
 *
 *----------------------------------------------------------------------------*/      

void ControlChain::release()
{
   TControl *e = chain;
   while ( e )
   {
      e->release();
      e = e->next;
   }
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  ControlChain::trigger
 *
 *  Desc:  Trigger all controls in the controls chain.
 *
 *  Memb:  chain            - pts to 1st control in chain
 *
 *----------------------------------------------------------------------------*/      

void ControlChain::trigger()
{
   TControl *e = chain;
   while ( e )
   {
      e->trigger();
      e = e->next;
   }
}

/******************************************************************************
 *
 *                                Effects 
 *
 ******************************************************************************/

/*----------------------------------------------------------------------------*
 *
 *  Name:  Effects::process
 *
 *  Desc:  Process an audio buffer through the effects chain.
 *
 *  Args:  buf              - ptr to audio buffer
 *
 *  Memb:  chain            - pts to 1st effect in chain
 *
 *----------------------------------------------------------------------------*/      

void Effects::process( char *buf )
{
   Effect *e = (Effect *)chain;
   while ( e )
   {
      if ( ! e->muted() ) 
         e->process( buf );
      e = (Effect *)(e->next);
   }
}

#ifdef CONSOLE_OUTPUT
const char *Effects::prompt() 
{ 
   return CONSTR("Effects"); 
}
#endif

/******************************************************************************
 *
 *                                 Factors
 *
 ******************************************************************************/

/*----------------------------------------------------------------------------*
 *
 *  Name:  Factors::value()
 *
 *  Desc:  Return the value of all factors in the chain multiplied together.
 *
 *  Rets:  value            - aggregate value of all factors
 *
 *  Memb:  chain            - pts to 1st factor in chain
 *
 *----------------------------------------------------------------------------*/      

double Factors::value()
{
   double val = 1.0;
   Factor *f = (Factor *)chain;
   while ( f )
   {
      val *= f->value;
      f = (Factor *)(f->next);
   }
   return val;
}

