/*
    _15_Event_Monitor.ino
 
    This sketch uses the ArduTouch console to provide feedback for onboard 
    I/O events. You can use it to verify that the onboard buttons, keys, 
    and pots are operating properly. When a key is pressed or released, a
    pot turned, or a button tapped, double-tapped, pressed, ot tap-pressed,
    a corrresponding message will be printed to the console.

    Note: If pot values come out backwards -- that is, they decrease when
          a pot is turned clockwise -- then you are probably declaring the 
          wrong PCB_REV in System.h

          To see what PCB_REV the library is compiled for, type '?' at the
          "monitor>" prompt in the console.

    Target:   ArduTouch board
 
    ---------------------------------------------------------------------------
 
    Copyright (C) 2016, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
    ---------------------------------------------------------------------------
 */

#include "ArduTouch.h"                       // use the ArduTouch library 

about_program( Event Monitor, 1.00 )         // specify sketch name & version
set_baud_rate( 115200 )                      // specify serial baud-rate

//  If the Runtime Model is not set to __FULLHOST__ the following statement 
//  will cause the compiler to abort, and to print an error message in the 
//  Arduino output window.

#ifndef __FULLHOST__
   #error This sketch requires a Runtime Model of __FULLHOST__ (Model.h)
#endif

class EvMonitor : public Synth           
{
   boolean charEv( char );                   // process a character event
   boolean evHandler( obEvent );             // handle an onboard event

   const char *prompt() 
   { 
      return CONSTR("monitor"); 
   }

} synth;                                      

void setup()
{
   ardutouch_setup( &synth );                // initialize ardutouch resources
}

void loop()
{
   ardutouch_loop();                         // perform ongoing ardutouch tasks  
}                                             

boolean EvMonitor::charEv( char code )
{
   switch ( code )
   {
      case chrInfo:                          // print the PCB revision code

         console.rtab();
         console.print( "PCB rev: " );
         #ifdef PCB_REV_A
            console.print( "A" );
         #endif
         #ifdef PCB_REV_B
            console.print( "B" );
         #endif
         #ifdef PCB_REV_C
            console.print( "C" );
         #endif
         return true;
         break;

      default:                         
         
         return Mode::charEv( code );
   }
}

boolean EvMonitor::evHandler( obEvent ev )
{
   switch ( ev.genus() )
   {
      case evKEY:
      {
         key k( ev.get() );
         byte pos = k.position();

         console.print( "Key " );
         console.print( (int )pos );
         console.print( " : " );

         switch ( ev.type() )
         {
            case KEY_DOWN:
               console.print("Down");
               break;
            case KEY_UP:
               console.print("Up");
         }

         console.newprompt();
         return true;
         break;
      }
      case evBUT:
      
         switch ( ev.type() )
         {
            case BUT0_PRESS:

               console.print( "Button 0 " );
               console.print( "-PRESS-" );
               break;

            case BUT0_TAP:

               console.print( "Button 0 " );
               console.print( "-TAP-" );
               break;

            case BUT0_DTAP:
               
               console.print( "Button 0 " );
               console.print( "-DOUBLE TAP-" );
               break;

            case BUT0_TPRESS:
               
               console.print( "Button 0 " );
               console.print( "-TAP PRESS-" );
               break;

            case BUT1_PRESS:

               console.print( "Button 1 " );
               console.print( "-PRESS-" );
               break;

            case BUT1_TAP:

               console.print( "Button 1 " );
               console.print( "-TAP-" );
               break;

            case BUT1_DTAP:
               
               console.print( "Button 1 " );
               console.print( "-DOUBLE TAP-" );
               break;

            case BUT1_TPRESS:
               
               console.print( "Button 1 " );
               console.print( "-TAP PRESS-" );
               break;

         }

         console.newprompt();
         return true;

      case evPOT:

         switch ( ev.type() )
         {
            case POT0:

               console.print( "POT 0 : " );
               break;

            case POT1:

               console.print( "POT 1 : " );
               break;
         }

         console.print( (int )ev.getPotVal() );
         console.newprompt();
         return true;
      
      default:

         return false;
   }
}

