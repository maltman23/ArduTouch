/*
    QuadSynth.cpp  

    Implementation of the QuadSynth and related classes.

    ---------------------------------------------------------------------------
 
    Copyright (C) 2016, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
   ---------------------------------------------------------------------------
*/

#include "Audio.h"
#include "QuadSynth.h"

/******************************************************************************
 *
 *                                QuadSynth 
 *
 ******************************************************************************/

/*----------------------------------------------------------------------------*
 *
 *  Name:  QuadSynth::setup
 *
 *  Desc:  Initialize synth. 
 *
 *  Memb:  vox[]            - component voices
 *
 *  Note:  This call-back method is automatically executed by ardutouch_setup()
 *         after system resources have been initialized. It is best to put your
 *         synth's initialization code here, and not in the constructor.
 *
 *----------------------------------------------------------------------------*/      

void QuadSynth::setup() 
{ 
   setupVoices(4);
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  QuadSynth::dynamics
 *
 *  Desc:  Perform a dynamic update:
 *
 *         1) execute a noteOn for the next defered voice, if any.
 *         2) decrement the index for next defered voice
 *         3) call each voice's dynamic update method.
 *
 *  Memb: +deferVoc         - if nonzero, execute noteOn for vox[deferVoc-1] 
 *         lastNote         - last note played
 *         vox[]            - component voices
 *
 *----------------------------------------------------------------------------*/

void QuadSynth::dynamics()  
{  
   if ( deferVoc )         // execute noteOn for next defered voice
   {
      --deferVoc;
      vox[ deferVoc ]->noteOn( lastNote );
   }
   super::dynamics();    
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  VoxSynth::newVox
 *
 *  Desc:  Return a pointer to the nth voice to be used by the synth.
 *
 *  Args:  nth              - voice # (0-based)
 *
 *  Rets:  ptrVox           - pointer to voice object.
 *
 *  Note:  This method is automatically called once per voice by setupVoices(). 
 *         It is not meant to be called from anywhere else!
 *
 *         By overriding this method you can customize which kind of
 *         voices are used by the synth.
 *
 *         The return type of this method is Voice*, meaning that it can 
 *         return a ptr to an object of any type which has Voice as a base 
 *         class.
 *
 *         If not overridden, this method creates a standard Voice object.
 *
 *----------------------------------------------------------------------------*/      

Voice *QuadSynth::newVox( byte nth )
{
   // this empty definition necessary to avoid audio glitch at startup -- why?

   class QuadVoice : public StockVoice          
   {
   } ;

   return new QuadVoice();
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  QuadSynth::noteOn
 *
 *  Desc:  Propagate a noteOn to each of the synth's voices. 
 *
 *  Memb: +deferVoc         - next dynamics() will call vox[deferVoc-1]->noteOn()
 *                            (if deferVoc != 0)
 *        +lastNote         - last note played
 *         xpose            - transpose notes by this many intervals
 *
 *  Note:  The actual note played by each voice depends on the input note plus
 *         the synth's transposition value plus the voice's transposition value.
 *
 *         QuadSynth uses an incremental algorithm for performing noteOns by
 *         distributing their execution across the next 4 dynamic updates.
 *
 *----------------------------------------------------------------------------*/

void QuadSynth::noteOn( key newNote )
{
   deferVoc = numVox;                        // set for defered noteOns
   lastNote = newNote;
   lastNote.transpose( xpose ); 
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  QuadSynth::output
 *
 *  Desc:  Write (stereo) output to a pair of audio buffers.
 *
 *  Args:  bufL             - ptr to left audio buffer  
 *         bufR             - ptr to right audio buffer  
 *
 *  Glob:  audioBufSz       - size of system audio buffers
 *
 *  Memb:  vox[]            - component voices
 *
 *  Note:  The outputs from voice 0 and voice 1 are combined in left buffer.
 *         The outputs from voice 2 and voice 3 are combined in right buffer.
 *
 *----------------------------------------------------------------------------*/      

void QuadSynth::output( char *bufL, char *bufR )
{  
   int sum;

   char  buf0[ audioBufSz ];     // temp buffer for for holding vox[0] output
   char  buf3[ audioBufSz ];     // temp buffer for for holding vox[3] output

   // combine vox[0] and vox[1] output in bufL
   // combine vox[2] and vox[3] output in bufR

   vox[0]->output( &buf0[0] ); 
   vox[1]->output( bufL ); 
   vox[2]->output( bufR ); 
   vox[3]->output( &buf3[0] ); 

   for ( byte i = 0 ; i < audioBufSz; i++ )
   {
      sum = bufL[ i ] + buf0[ i ];
      bufL[i] = sum >> 1;
      sum = bufR[ i ] + buf3[ i ];
      bufR[i] = sum >> 1;
   }
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  QuadSynth::reTrigger
 *
 *  Desc:  Retrigger all voices. 
 *
 *  Memb: +deferVoc         - next dynamics() will call vox[deferVoc-1]->noteOn()
 *                            (if deferVoc != 0)
 *         vox[]            - component voices
 *
 *----------------------------------------------------------------------------*/

void QuadSynth::reTrigger()                          
{
   noteOff(0);

   // set deferVoc so that noteOn()s will be called in subsequent dynamics() 

   deferVoc = numVox;                        
}  

/******************************************************************************
 *
 *                               QuadPanSynth 
 *
 ******************************************************************************/

/*----------------------------------------------------------------------------*
 *
 *  Name:  QuadPanSynth::setup
 *
 *  Desc:  Initialize synth. 
 *
 *  Memb: +panControl       - dynamic controller of panPos
 *
 *  Note:  This call-back method is automatically executed by ardutouch_setup()
 *         after system resources have been initialized. It is best to put your
 *         synth's initialization code here, and not in the constructor.
 *
 *----------------------------------------------------------------------------*/      

void QuadPanSynth::setup() 
{ 
   super::setup();
   panControl = new PanControl( &this->panPos );
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  QuadPanSynth::charEv
 *
 *  Desc:  Process a character event.
 *
 *  Args:  code             - character to process
 *
 *  Memb: +pan[]            - component voices in pan order
 *         panControl       - dynamic controller of panPos
 *         vox[]            - component voices
 *
 *  Rets:  status           - true if character was handled
 *
 *----------------------------------------------------------------------------*/      

boolean QuadPanSynth::charEv( char code )
{
   switch( code )
   {
      #ifdef INTERN_CONSOLE   // compile cases needed by macros

      case 'M':
      {
         char *newMap;
         if ( console.getStr( CONSTR("panMap"), &newMap ) )
            setMap( newMap );
         break;
      }
      case 'P':
         
         console.pushMode( panControl );
         break;   
      
      #endif

      #ifdef CONSOLE_OUTPUT   // compile cases that display to console 

      case chrInfo:

         super::charEv( chrInfo );
         console.newlntab();

         console.romprint( CONSTR("{panMap ") );
         for ( byte i = 0; i < numVox; i++ )
            console.print( (char )('0' + pan[i]->num) );
         console.romprint( CONSTR("} ") );

         panControl->brief();
         break;

      #endif

      case '!':

         super::charEv('!');
         for ( byte i = 0; i < numVox; i++ )
            pan[i] = vox[i];
         panControl->reset();
         break;

      default:

         return super::charEv( code );
   }
   return true;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  QuadPanSynth::dynamics
 *
 *  Desc:  Perform a dynamic update.
 *
 *  Memb:  panControl       - dynamic controller of panPos
 *
 *----------------------------------------------------------------------------*/

void QuadPanSynth::dynamics() 
{  
   super::dynamics();
   panControl->dynamics();
}

#ifdef KEYBRD_MENUS

/*----------------------------------------------------------------------------*
 *
 *  Name:  QuadPanSynth::menu
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

char QuadPanSynth::menu( key k )   
{
   switch ( k.position() )
   {
      case  5: return 'P';                // push Pan control
      default: return super::menu(k);
   }
}

#endif

/*----------------------------------------------------------------------------*
 *
 *  Name:  QuadPanSynth::output
 *
 *  Desc:  Write (stereo) output to a pair of audio buffers.
 *
 *  Args:  bufL             - ptr to left audio buffer  
 *         bufR             - ptr to right audio buffer  
 *
 *  Glob:  audioBufSz       - size of system audio buffers
 *
 *  Memb:  pan[]            - component voices in pan order
 *         panPos           - pan position for voice pan[1] 
 *
 *  Note:  Output from voice pan[0] will be written to the left buffer.
 *         Output from voice pan[3] will be written to the right buffer.
 *
 *         Output from voices pan[1] & pan[2] will be divided between the
 *         left and right buffers depending on the value of panPos. 
 *
 *         panPos is a scalar which determines how much of voice pan[1] is 
 *         written to the left buffer (the remainder is written to the right 
 *         buffer). Voice pan[2] is panned as the stereo complement of voice
 *         pan[1] -- i.e., if voice pan[1] is 30% in the left buffer and 70%
 *         in the right buffer, then voice pan[2] is 70% in the left buffer
 *         and 30% in the right buffer.
 *
 *----------------------------------------------------------------------------*/      

void QuadPanSynth::output( char *bufL, char *bufR )
{  
   // voice 0 output is always all in left channel
   // voice 3 output is always all in right channel
   //
   // voice 1 & 2 are mixed between the two channels based on panPos

   // pan the four voices so that:
   //
   //    bufL = ( pan[0] + (256-panPos)*pan[1] + panPos*pan[2] ) / 2
   //    bufR = ( pan[3] + (256-panPos)*pan[2] + panPos*pan[1] ) / 2
   //

   char  buf0[ audioBufSz ];    // temp buffer for for holding pan[0] output
   char  buf3[ audioBufSz ];    // temp buffer for for holding pan[3] output

   int  sum;                              // temp sum register
   int  sum1_2;                           // sum of voice 1 & 2 output
   int  sum1L_2L;                         // left component of pan[1+2]
   Int  pan1L;                            // left component of pan[1]
   Int  pan2L;                            // left component of pan[2]
   word coPanning;                        // complement of panPos

   coPanning = 256 - panPos;

   pan[0]->output( &buf0[0] ); 
   pan[1]->output( bufL ); 
   pan[2]->output( bufR ); 
   pan[3]->output( &buf3[0] ); 

   for ( byte i = 0 ; i < audioBufSz; i++ )
   {
      pan1L.val  = coPanning;
      pan1L.val *= bufL[i];
      pan2L.val  = panPos;
      pan2L.val *= bufR[i];

      sum1_2     = bufL[i] + bufR[i];
      sum1L_2L   = pan1L._.msb + pan2L._.msb;

      sum        = buf0[i] + sum1L_2L;
      bufL[i]    = sum >> 1;

      sum        = buf3[ i ] + sum1_2  - sum1L_2L;
      bufR[i]    = sum >> 1;
   }

}

/*----------------------------------------------------------------------------*
 *
 *  Name:  QuadPanSynth::setMap
 *
 *  Desc:  Set the panning map for the component voices.
 *
 *  Args:  mapStr           - string specifying pan order for voices
 *
 *  Memb: +pan[]            - component voices in pan order
 *         vox[]            - component voices
 *
 *  Note:  A valid mapStr is 4 characters long (+ null terminal char), and
 *         consists of the digits "0", "1", "2", and "3" appearing once in 
 *         any order. 
 *
 *----------------------------------------------------------------------------*/      

void QuadPanSynth::setMap( char *mapStr )
{
   byte i;

   // reject map if length of string != numVox

   if ( mapStr[ numVox ] != 0 )           
      return;   

   // initialize all members of transfer array to NOT_PRESENT

   const byte NOT_PRESENT = 255;
   byte xfer[ numVox ];
   for ( i = 0; i < numVox; i++ )
      xfer[i] = NOT_PRESENT;

   // iterate through characters of mapStr and populate transfer array

   i = 0;
   while ( mapStr[i] != 0 )
   {
      byte digit = mapStr[i]-'0';
      if ( digit >= numVox ) return;      // reject on bad digit
      xfer[digit] = i;
      i++;
   }
      
   // reject map if any element of xfer[] is not present

   for ( i = 0; i < numVox; i++ )
      if ( xfer[i] == NOT_PRESENT ) 
         return;

   // remap pan[]

   i = 0;
   while ( mapStr[i] != 0 )
   {
      byte digit = mapStr[i]-'0';
      pan[i] = vox[digit];
      i++;
   }
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  QuadPanSynth::setPanPos
 *
 *  Desc:  Set the panning position for voices pan[1/2].
 *
 *  Args:  panPos           - pan position for voices pan[1/2].
 *
 *  Memb:  panControl       - dynamic controller of panPos
 *
 *  Note:  panPos is a scalar which determines how much of voice pan[1] is 
 *         written to the left buffer (the remainder is written to the right 
 *         buffer). Voice pan[2] is panned as the stereo complement of voice
 *         pan[1] -- i.e., if voice pan[1] is 30% in the left buffer and 70%
 *         in the right buffer, then voice pan[2] is 70% in the left buffer
 *         and 30% in the right buffer.
 *
 *----------------------------------------------------------------------------*/      

void QuadPanSynth::setPanPos( byte panPos )
{
   panControl->setRestPos( panPos );
}


/******************************************************************************
 *
 *                          QuadDualPanSynth 
 *
 ******************************************************************************/

/*----------------------------------------------------------------------------*
 *
 *  Name:  QuadDualPanSynth::setup
 *
 *  Desc:  Initialize synth. 
 *
 *  Memb: +xpanControl      - dynamic controller of xpanPos
 *
 *  Note:  This call-back method is automatically executed by ardutouch_setup()
 *         after system resources have been initialized. It is best to put your
 *         synth's initialization code here, and not in the constructor.
 *
 *----------------------------------------------------------------------------*/      

void QuadDualPanSynth::setup() 
{ 
   super::setup();
   xpanControl = new XPanControl( &this->xpanPos );
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  QuadDualPanSynth::charEv
 *
 *  Desc:  Process a character event.
 *
 *  Args:  code             - character to process
 *
 *  Memb:  xpanControl      - dynamic controller of xpanPos
 *
 *  Rets:  status           - true if character was handled
 *
 *----------------------------------------------------------------------------*/      

boolean QuadDualPanSynth::charEv( char code )    
{
   switch( code )
   {
      #ifdef INTERN_CONSOLE   // compile cases needed by macros

      case 'X':
         
         console.pushMode( xpanControl );
         break;   
      
      #endif

      #ifdef CONSOLE_OUTPUT   // compile cases that display to console 

      case chrInfo:

         super::charEv( chrInfo );
         xpanControl->brief();
         break;

      #endif

      case '!':

         super::charEv('!');
         xpanControl->reset();
         break;

      default:

         return super::charEv( code );
   }
   return true;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  QuadDualPanSynth::dynamics
 *
 *  Desc:  Perform a dynamic update.
 *
 *  Memb:  xpanControl      - dynamic controller of xpanPos
 *
 *----------------------------------------------------------------------------*/

void QuadDualPanSynth::dynamics()     
{  
   super::dynamics();
   xpanControl->dynamics();
}

#ifdef KEYBRD_MENUS

/*----------------------------------------------------------------------------*
 *
 *  Name:  QuadDualPanSynth::menu
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

char QuadDualPanSynth::menu( key k )   
{
   switch ( k.position() )
   {
      case  4: return 'X';                // push XPan control
      default: return super::menu(k);
   }
}

#endif

/*----------------------------------------------------------------------------*
 *
 *  Name:  QuadDualPanSynth::output
 *
 *  Desc:  Write (stereo) output to a pair of audio buffers.
 *
 *  Args:  bufL             - ptr to left audio buffer  
 *         bufR             - ptr to right audio buffer  
 *
 *  Glob:  audioBufSz       - size of system audio buffers
 *
 *  Memb:  pan[]            - component voices in pan order
 *         panPos           - pan position for voices pan[1/2] 
 *         xpanPos          - pan position for voices pan[0/3] 
 *
 *  Note:  Output from voices pan[1] & pan[2] will be divided between the
 *         left and right buffers depending on the value of panPos. 
 *
 *         Output from voices pan[0] & pan[3] will be divided between the
 *         left and right buffers depending on the value of xpanPos. 
 *
 *         panPos is a scalar which determines how much of voice pan[1] is 
 *         written to the left buffer (the remainder is written to the right 
 *         buffer). Voice pan[2] is panned as the stereo complement of voice
 *         pan[1] -- i.e., if voice pan[1] is 30% in the left buffer and 70%
 *         in the right buffer, then voice pan[2] is 70% in the left buffer
 *         and 30% in the right buffer.
 *
 *         xpanPos is operable on voices pan[0] and pan[3], in the same way 
 *         that panPos is operable on voices pan[1] and pan[2]
 *
 *----------------------------------------------------------------------------*/      

void QuadDualPanSynth::output( char *bufL, char *bufR ) 
{  
   // voice 0 & 3 are mixed between the two channels based on xpanPos
   // voice 1 & 2 are mixed between the two channels based on panPos

   // pan the four voices so that:
   //
   //    bufL = ( (256-panPos) *pan[1] +  panPos*pan[2] 
   //             (256-xpanPos)*pan[0] + xpanPos*pan[3] ) / 2
   //
   //    bufR = ( (256-panPos) *pan[2] +  panPos*pan[1] 
   //             (256-xpanPos)*pan[3] + xpanPos*pan[0] ) / 2

   char  buf0[ audioBufSz ];    // temp buffer for for holding pan[0] output
   char  buf3[ audioBufSz ];    // temp buffer for for holding pan[3] output

   int  sum;                              // temp sum register
   int  sum1_2;                           // sum of voice 1 & 2 output
   int  sum1L_2L;                         // left component of pan[1+2]
   Int  pan1L;                            // left component of pan[1]
   Int  pan2L;                            // left component of pan[2]
   word coPanning;                        // complement of panPos

   int  sum0_3;                           // sum of voice 0 & 3 output
   int  sum0L_3L;                         // left component of pan[0+3]
   Int  pan0L;                            // left component of pan[0]
   Int  pan3L;                            // left component of pan[3]
   word coXPanning;                       // complement of xpanPos

   coPanning  = 256 - panPos;
   coXPanning = 256 - xpanPos;

   pan[0]->output( &buf0[0] ); 
   pan[1]->output( bufL ); 
   pan[2]->output( bufR ); 
   pan[3]->output( &buf3[0] ); 

   for ( byte i = 0 ; i < audioBufSz; i++ )
   {
      pan1L.val  = coPanning;
      pan1L.val *= bufL[i];
      pan2L.val  = panPos;
      pan2L.val *= bufR[i];

      sum1_2     = bufL[i] + bufR[i];
      sum1L_2L   = pan1L._.msb + pan2L._.msb;

      pan0L.val  = coXPanning;
      pan0L.val *= buf0[i];
      pan3L.val  = xpanPos;
      pan3L.val *= buf3[i];

      sum0_3     = buf0[i] + buf3[i];
      sum0L_3L   = pan0L._.msb + pan3L._.msb;

      sum        = sum0L_3L + sum1L_2L;
      bufL[i]    = sum >> 1;

      sum        = sum0_3 + sum1_2 - sum0L_3L - sum1L_2L;
      bufR[i]    = sum >> 1;
   }

}

/*----------------------------------------------------------------------------*
 *
 *  Name:  QuadDualPanSynth::setXPanPos
 *
 *  Desc:  Set the panning position for voices pan[0/3].
 *
 *  Args:  xpanPos          - pan position for voices pan[0/3].
 *
 *  Memb:  xpanControl      - dynamic controller of xpanPos
 *
 *  Note:  xpanPos is a scalar which determines how much of voice pan[0] is 
 *         written to the left buffer (the remainder is written to the right 
 *         buffer). Voice pan[3] is panned as the stereo complement of voice
 *         pan[1] -- i.e., if voice pan[0] is 30% in the left buffer and 70%
 *         in the right buffer, then voice pan[3] is 70% in the left buffer
 *         and 30% in the right buffer.
 *
 *----------------------------------------------------------------------------*/      

void QuadDualPanSynth::setXPanPos( byte xpanPos )
{
   xpanControl->setRestPos( xpanPos );
}

/******************************************************************************
 *       
 *                                XPanControl
 *
 ******************************************************************************/

   
#ifdef CONSOLE_OUTPUT
const char *XPanControl::prompt()        
{
   return CONSTR("XPan");
}
#endif

