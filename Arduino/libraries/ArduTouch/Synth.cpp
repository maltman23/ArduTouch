/*
    Synth.cpp  

    Implementation of the Synth and auxillary classes.

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
#include "Synth.h"
#include "WaveBank.h"
#include "WaveOsc.h"

PresetBank presets;                 // bank of synth presets

/******************************************************************************
 *
 *                                Synth 
 *                                                                            
 ******************************************************************************/

boolean Synth::charEv( char code )
{
   switch ( code )
   {
      #ifdef INTERN_CONSOLE

      case 'p':                     // choose a preset
      {
         preset_loading = true;
         quiet_reset    = false;
         if ( flags & RSTMUTE )     // if synth is prefigured for mute reset
            presets.choose();
         else                       // synth normally unmuted upon reset
         {      
            presets.choose();
            if ( quiet_reset )
            {
               flags &= ~RSTMUTE;
               setMute( false );
            }
         }
         preset_loading = false;
         quiet_reset    = false;
         break;
      }

      case '\'':                    // perform a quiet reset (for preset)

         if ( preset_loading )      
         {
            flags |= RSTMUTE;
            quiet_reset = true;
            reset();
         }
         break;

      #endif

      case '!':                     // reset

         super::charEv('!');
         keybrd.setMute( false );   // enable keybrd
         break;

      case '.':

         audio::wait( 30 );         // wait for contextual volume to "settle"

      default:

         return super::charEv( code );
   }
   return true;
}

#ifdef KEYBRD_MENUS
/*----------------------------------------------------------------------------*
 *
 *  Name:  Synth::menu
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
 *           |   |   |   |   |   |   |   |   | k |   | . |   | 
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |    ___     ___    |    ___     ___     ___    | 
 *           |     |       |     |     |       |       |     |
 *           |     |       |     |     |       |       |     |
 *           |  p  |       |     |     |   x   |   <   |  !  |
 *           |     |       |     |     |       |       |     |
 *           |     |       |     |     |       |       |     |
 *           -------------------------------------------------
 *
 *----------------------------------------------------------------------------*/      

char Synth::menu( key k )
{
   switch ( k.position() )
   {
      case  0: return 'p';
      default: return super::menu(k);
   }
}
#endif

#ifdef CONSOLE_OUTPUT
const char *Synth::prompt()                     
{
   return CONSTR("synth");
}
#endif

/******************************************************************************
 *
 *                                 VoxSynth 
 *
 ******************************************************************************/

/*----------------------------------------------------------------------------*
 *
 *  Name:  VoxSynth::charEv
 *
 *  Desc:  Process a character event.
 *
 *  Args:  code             - character to process
 *
 *  Memb:  numVox           - number of member voices
 *         vox[]            - array of member voices
 *
 *  Rets:  status           - true if character was handled
 *
 *----------------------------------------------------------------------------*/      

boolean VoxSynth::charEv( char code )              
{                                        
   #ifdef INTERN_CONSOLE                  
   
   // push voice by #

   if ( code >= '0' && code < '0'+numVox )
   {
      console.pushMode( vox[code-'0'] );
      return true;
   }

   #endif

   switch( code )
   {
      #ifdef INTERN_CONSOLE
      case '#':
      {
         char ith = console.getDigit( CONSTR("v#"), numVox-1 );
         if ( ith >= 0 )
            console.pushMode( vox[ith] );
         break;
      }      
      #endif

      case '!':

         super::charEv( code );
         for ( byte i = 0; i < numVox; i++ )
            vox[i]->reset();
         break;

      case '.':                              // mute
      case '<':                              // unmute

      {
         byte chnlVol = ( code == '.' ? 0 : vol );
         for ( byte i = 0; i < numVox; i++ )
            vox[i]->setGlobVol( chnlVol );
      }  
         
      default:

         return super::charEv( code );
   }
   return true;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  VoxSynth::dynamics
 *
 *  Desc:  Perform a dynamic update.
 *
 *  Memb:  numVox           - number of member voices
 *         vox[]            - ptrs to member voices
 *
 *----------------------------------------------------------------------------*/

void VoxSynth::dynamics()
{
   super::dynamics();
   for ( byte i = 0; i < numVox; i++ )
      vox[i]->dynamics();
}

#ifdef KEYBRD_MENUS
/*----------------------------------------------------------------------------*
 *
 *  Name:  VoxSynth::menu
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
 *           |   | # |   |   |   |   |   |   | k |   | . |   | 
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |    ___     ___    |    ___     ___     ___    | 
 *           |     |       |     |     |       |       |     |
 *           |     |       |     |     |       |       |     |
 *           |  p  |       |     |     |   x   |   <   |  !  |
 *           |     |       |     |     |       |       |     |
 *           |     |       |     |     |       |       |     |
 *           -------------------------------------------------
 *
 *----------------------------------------------------------------------------*/      

char VoxSynth::menu( key k )
{
   switch ( k.position() )
   {
      case  1: return '#';
      default: return super::menu(k);
   }
}
#endif

/*----------------------------------------------------------------------------*
 *
 *  Name:  VoxSynth::noteOn
 *
 *  Desc:  Turn a note on.
 *
 *  Args:  note             - note to turn on  
 *
 *  Memb:  numVox           - number of member voices
 *         vox[]            - ptrs to member voices
 *         xpose            - transpose notes by this many intervals
 *
 *  Note:  A noteOn() will be propagated to all voices whose keybrd is under
 *         the synth's control.
 *
 *----------------------------------------------------------------------------*/      

void VoxSynth::noteOn( key note )
{
   note.transpose( xpose ); 
   for ( byte i = 0; i < numVox; i++ )    // propagate to all controlled voices
      if ( vox[i]->keybrd.muted() )
         vox[i]->noteOn( note );
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  VoxSynth::noteOff
 *
 *  Desc:  Turn a note off.
 *
 *  Args:  note             - note to turn off  
 *
 *  Memb:  numVox           - number of member voices
 *         vox[]            - ptrs to member voices
 *         xpose            - transpose notes by this many intervals
 *
 *  Note:  A noteOff() will be propagated to all voices whose keybrd is under
 *         the synth's control.
 *
 *----------------------------------------------------------------------------*/      

void VoxSynth::noteOff( key note )
{
   note.transpose( xpose ); 
   for ( byte i = 0; i < numVox; i++ )    // propagate to all controlled voices
      if ( vox[i]->keybrd.muted() )
         vox[i]->noteOff( note );
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  VoxSynth::newOsc
 *
 *  Desc:  Return a pointer to the oscillator which is to be used by the nth 
 *         voice.
 *
 *  Args:  nth              - voice # (0-based)
 *
 *  Rets:  ptrOsc           - pointer to oscillator object.
 *
 *  Note:  This method is automatically called once per voice by setupVoices(). 
 *         It is not meant to be called from anywhere else!
 *
 *         By overriding this method you can customize which kind of
 *         oscillators are used by which voices.
 *
 *         The return type of this method is Osc*, meaning that it can return
 *         a ptr to an object of any type which has Osc as a base class.
 *
 *         If not overridden, this method creates a WaveOsc using a Sine
 *         wave table.
 *
 *----------------------------------------------------------------------------*/      

Osc *VoxSynth::newOsc( byte nth )
{
   WaveOsc *o = new WaveOsc();
   o->setTable( wavetable_named( Sine ) ); 
   return o;
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

Voice *VoxSynth::newVox( byte nth )
{
   StockVoice *v = new StockVoice();
   return v;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  VoxSynth::setAttack
 *
 *  Desc:  Set the attack time of the amplitude envelope for all voices.
 *
 *  Args:  attack           - attack time
 *
 *  Memb:  vox[]            - component voices
 *
 *----------------------------------------------------------------------------*/

void VoxSynth::setAttack( byte attack )
{
   for ( byte i = 0; i < numVox; i++ )
      vox[i]->envAmp.setAttack( attack );
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  VoxSynth::setDecay
 *
 *  Desc:  Set the decay time of the amplitude envelope for all voices.
 *
 *  Args:  decay            - decay time
 *
 *  Memb:  vox[]            - component voices
 *
 *----------------------------------------------------------------------------*/

void VoxSynth::setDecay( byte decay )
{
   for ( byte i = 0; i < numVox; i++ )
      vox[i]->envAmp.setDecay( decay );
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  VoxSynth::setRelease
 *
 *  Desc:  Set the release time of the amplitude envelope for all voices.
 *
 *  Args:  release          - release time
 *
 *  Memb:  vox[]            - component voices
 *
 *----------------------------------------------------------------------------*/

void VoxSynth::setRelease( byte release )
{
   for ( byte i = 0; i < numVox; i++ )
      vox[i]->envAmp.setRelease( release );
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  VoxSynth::setSustain
 *
 *  Desc:  Set the sustain level of the amplitude envelope for all voices.
 *
 *  Args:  sustain          - sustain level
 *
 *  Memb:  vox[]            - component voices
 *
 *----------------------------------------------------------------------------*/

void VoxSynth::setSustain( byte sustain )
{
   for ( byte i = 0; i < numVox; i++ )
      vox[i]->envAmp.setSustain( sustain );
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  VoxSynth::setupVoices
 *
 *  Desc:  Initialize the voice array by assigning a voice object to each 
 *         element in the array (up to the number used in the synth), and
 *         assigning an oscillator to each voice.
 *
 *  Args:  numVox           - specifies number of voices to use in synth
 *
 *  Memb: +numVox           - # of voices
 *        +vox[]            - ptrs to member voices
 *
 *  Note:  Derived classes which override setup() must call this method from
 *         within their setup() routine. 
 *
 *         This method:
 *
 *             1) establishes the number of voices used in the synth
 *             2) calls newVox() for each voice 
 *             3) stores the object ptr from (2) in the vox[] array
 *             4) calls newOsc() for any voice that hasn't set up an oscillator
 *             5) assigns the object ptr from (4) to its voice
 *
 *         Once this method has been called, voices can be subsequently
 *         referenced using "vox[n]->", where n is the (0-based) voice #.
 *
 *----------------------------------------------------------------------------*/      

void VoxSynth::setupVoices( byte numVox )
{
   if ( numVox > MaxVox )
      numVox = MaxVox;

   this->numVox = numVox;

   for ( byte i = 0; i < numVox; i++ )
   {
      vox[i] = newVox(i);
      if ( ! vox[i]->osc )             // if voice constructor didn't set osciilator 
         vox[i]->useOsc( newOsc(i) );  // create one via newOsc()  
      vox[i]->num = i;
   }
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  VoxSynth::setVoicing
 *
 *  Desc:  Set the transposition interval for each voice.
 *
 *  Args:  voicings         - transposition intervals per voice
 *
 *  Memb:  vox[]            - component voices
 *
 *  Note:  A voicing specifies the transposition interval for each voice.
 *         When a noteOn is received by the synth, it will be transposed
 *         per voice by the number of intervals specified in the voicing.
 *         For example, if the synth has its voicing set to (-12, 0, 7, 12)
 *         and receives a noteOn for the note C4 then the resultant notes 
 *         per voice will be:
 *
 *             vox[0] - C3
 *             vox[1] - C4
 *             vox[2] - G4
 *             vox[3] - C5
 *
 *----------------------------------------------------------------------------*/

void VoxSynth::setVoicing( char* v )
{
   for ( byte i = 0; i < numVox; i++ )   
      vox[i]->xpose = v[i];
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  VoxSynth::setVol
 *
 *  Desc:  Set the volume level of the synth..
 *
 *  Args:  vol              - volume level
 *
 *  Memb:  numVox           - number of member voices
 *        +vol              - volume level (0:255)
 *         vox[]            - ptrs to member voices
 *
 *----------------------------------------------------------------------------*/      

void VoxSynth::setVol( byte vol )
{
   super::setVol( vol );
   if ( ! muted() )
      for ( byte i = 0; i < numVox; i++ )   
         vox[i]->setGlobVol( vol );
}

/******************************************************************************
 *
 *                                OneVoxSynth 
 *
 ******************************************************************************/

/*----------------------------------------------------------------------------*
 *
 *  Name:  OneVoxSynth::setup
 *
 *  Desc:  Initialize synth. 
 *
 *  Note:  This call-back method is automatically executed by ardutouch_setup()
 *         after system resources have been initialized. It is best to put your
 *         synth's initialization code here, and not in the constructor.
 *
 *----------------------------------------------------------------------------*/      

void OneVoxSynth::setup() 
{ 
   setupVoices(1);
}

#ifdef KEYBRD_MENUS
/*----------------------------------------------------------------------------*
 *
 *  Name:  OneVoxSynth::menu
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
 *           |   | 0 |   |   |   |   |   |   | k |   | . |   | 
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |   |   |   |   |   |   |   |   |   |   |   |   | 
 *           |    ___     ___    |    ___     ___     ___    | 
 *           |     |       |     |     |       |       |     |
 *           |     |       |     |     |       |       |     |
 *           |  p  |       |     |     |   x   |   <   |  !  |
 *           |     |       |     |     |       |       |     |
 *           |     |       |     |     |       |       |     |
 *           -------------------------------------------------
 *
 *----------------------------------------------------------------------------*/      

char OneVoxSynth::menu( key k )
{
   switch ( k.position() )
   {
      case  1: return '0';
      default: return super::menu(k);
   }
}
#endif

/*----------------------------------------------------------------------------*
 *
 *  Name:  OneVoxSynth::output
 *
 *  Desc:  Write output to an audio buffer.
 *
 *  Args:  buf              - ptr to audio buffer  
 *
 *  Memb:  vox[0]           - ptr to voice
 *
 *----------------------------------------------------------------------------*/      

void OneVoxSynth::output( char *buf )
{
   vox[0]->output( buf );  
}


/******************************************************************************
 *
 *                                TwoVoxSynth 
 *
 ******************************************************************************/

/*----------------------------------------------------------------------------*
 *
 *  Name:  TwoVoxSynth::setup
 *
 *  Desc:  Initialize synth. 
 *
 *  Note:  This call-back method is automatically executed by ardutouch_setup()
 *         after system resources have been initialized. It is best to put your
 *         synth's initialization code here, and not in the constructor.
 *
 *----------------------------------------------------------------------------*/      

void TwoVoxSynth::setup() 
{ 
   setupVoices(2);
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  TwoVoxSynth::output
 *
 *  Desc:  Write (stereo) output to a pair of audio buffers.
 *
 *  Args:  bufL             - ptr to left audio buffer  
 *         bufR             - ptr to right audio buffer  
 *
 *  Memb:  vox[0]           - ptr to 1st voice 
 *         vox[1]           - ptr to 2nd voice 
 *
 *----------------------------------------------------------------------------*/      

void TwoVoxSynth::output( char *bufL, char *bufR )
{
   vox[0]->output( bufL );  
   vox[1]->output( bufR );  
}

/******************************************************************************
 *
 *                              TwoVoxPanSynth 
 *
 ******************************************************************************/

/*----------------------------------------------------------------------------*
 *
 *  Name:  TwoVoxPanSynth::setup
 *
 *  Desc:  Initialize synth. 
 *
 *  Memb: +panControl       - dynamic controller of panPos
 *         panPos           - static pan position for voices
 *
 *  Note:  This call-back method is automatically executed by ardutouch_setup()
 *         after system resources have been initialized. It is best to put your
 *         synth's initialization code here, and not in the constructor.
 *
 *----------------------------------------------------------------------------*/      

void TwoVoxPanSynth::setup() 
{ 
   setupVoices(2);
   panControl = new PanControl( &this->panPos );
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  TwoVoxPanSynth::charEv
 *
 *  Desc:  Process a character event.
 *
 *  Args:  code             - character to process
 *
 *  Memb:  panControl       - dynamic controller of panPos
 *
 *  Rets:  status           - true if character was handled
 *
 *----------------------------------------------------------------------------*/      

boolean TwoVoxPanSynth::charEv( char code )
{
   switch( code )
   {
      #ifdef INTERN_CONSOLE   // compile cases needed by macros

      case 'P':               // push the panning control
         
         console.pushMode( panControl );
         break;   
      
      #endif

      #ifdef CONSOLE_OUTPUT   // compile cases that display to console 

      case chrInfo:

         super::charEv( chrInfo );
         // console.newlntab();
         panControl->brief();
         break;

      #endif

      case '!':               // reset

         super::charEv('!');
         panControl->reset();
         break;

      default:

         return super::charEv( code );
   }
   return true;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  TwoVoxPanSynth::dynamics
 *
 *  Desc:  Perform a dynamic update.
 *
 *  Memb:  panControl       - dynamic controller of panPos
 *
 *----------------------------------------------------------------------------*/

void TwoVoxPanSynth::dynamics() 
{  
   super::dynamics();
   panControl->dynamics();
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  TwoVoxPanSynth::output
 *
 *  Desc:  Write (stereo) output to a pair of audio buffers.
 *
 *  Args:  bufL             - ptr to left audio buffer  
 *         bufR             - ptr to right audio buffer  
 *
 *  Memb:  panPos           - static pan position for voices 
 *         vox[0]           - ptr to 1st voice 
 *         vox[1]           - ptr to 2nd voice 
 *
 *----------------------------------------------------------------------------*/      

void TwoVoxPanSynth::output( char *bufL, char *bufR )
{
   vox[0]->output( bufL ); 
   vox[1]->output( bufR ); 

   // mix voice 0 & 1 between the two channels based on panPos

   word coPanning = 256 - panPos;         // complement of panPos

   // pan the voices such that:
   //
   //    bufL = (256-panPos)*vox[0] + panPos*vox[1]
   //    bufR = (256-panPos)*vox[1] + panPos*vox[0]
   //

   Int  sum;                              // sum of voice 0 & 1 output
   Int  regL;                             // register for computing left buffer
   Int  regR;                             // register for computing rght buffer

   for ( byte i = 0 ; i < audioBufSz; i++ )
   {
      sum.val    = bufL[i] + bufR[i];

      regR.val   = panPos;
      regR.val  *= bufR[i];

      regL.val   = coPanning;
      regL.val  *= bufL[i];
      regL.val  += regR.val;

      bufL[i]    = regL._.msb;

      sum.val   -= regL._.msb;
      bufR[i]    = sum._.lsb;
   }

}
