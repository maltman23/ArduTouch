/*
    QuantumOsc.cpp  

    Implementation of Quantum Oscillator classes.

    A quantum oscillator is restricted to only those frequencies with
    wavelengths which are an integral number of audio cycles long.

    ---------------------------------------------------------------------------
 
    Copyright (C) 2018, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
   ---------------------------------------------------------------------------
*/

#include "types.h"
#include "Console_.h"
#include "Audio.h"
#include "QuantumOsc.h"

/******************************************************************************
 *
 *                                QuantumOsc 
 *                                                                            
 ******************************************************************************/

/*----------------------------------------------------------------------------*
 *
 *  Name:  QuantumOsc::charEv
 *
 *  Desc:  Process a character event.
 *
 *  Args:  code             - character to process
 *
 *  Memb: +tracking         - if true, frequency tracking is enabled
 *
 *  Rets:  status           - true if character was handled
 *
 *----------------------------------------------------------------------------*/      

boolean QuantumOsc::charEv( char code )    
{
   switch ( code )
   {
      #ifdef INTERN_CONSOLE

      case 't':                     // set frequency tracking on/off 
      
         console.getBool( CONSTR("track"), &tracking );
         break;

      #endif

      #ifdef CONSOLE_OUTPUT

      case chrInfo:                 // display object info to console

         super::charEv( chrInfo );
         console.infoBool( CONSTR("track"), tracking );
         break;

      #endif

      case '!':                     // perform a reset

         super::charEv( code );  
         tracking = true;
         break;

      default:

         return super::charEv( code ); // pass unhandled events to super class
   }
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  QuantumOsc::periodHz
 *
 *  Desc:  Return the frequency for a period of a given length.
 *
 *  Args:  period           - length of (integral) period
 *
 *  Glob:  audioRate        - audio playback rate
 *
 *  Rets:  freq             - frequency
 *
 *----------------------------------------------------------------------------*/      

double QuantumOsc::periodHz( word period )
{
   if ( period )
      return audioRate / period;
   else
      return 0.0;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  QuantumOsc::onFreq
 *
 *  Desc:  Compute frequency-dependent state vars.
 *
 *  Memb:  effFreq          - effective frequency (includes internal detuning)
 *         extFactor        - external detuning factor
 *         tracking         - if true, frequency tracking is enabled
 *
 *  Note:  onFreq() is a virtual method of Osc, and is automatically called
 *         whenever there is a change in frequency for the oscillator. This change
 *         in frequency can come from one of 3 sources: the oscillator is locally
 *         detuned, the "ideal" frequency of the oscillator is set, or an external
 *         (transient) detuning is applied (such as from an ongoing vibrato).
 *
 *         There are 2 implicit inputs to onFreq(), Osc::effFreq and 
 *         Osc::extFactor. Multiplying these together gives the intended 
 *         frequency of the oscillator.
 * 
 *         A quantum oscillator can only run at frequencies corresponding 
 *         to integral periods. This method sets the frequency as close as 
 *         possible to the intended frequency.
 *
 *----------------------------------------------------------------------------*/

void QuantumOsc::onFreq()
{
   if ( tracking )                  // if frequency tracking is enabled ...
   
      // set frequency as close as possible to the intended frequency

      track( effFreq * extFactor );
}

/******************************************************************************
 *
 *                               PureSquare 
 *                                                                            
 ******************************************************************************/

/*----------------------------------------------------------------------------*
 *
 *  Name:  PureSquare::charEv
 *
 *  Desc:  Process a character event.
 *
 *  Args:  code             - character to process
 *
 *  Memb: +edgLen           - # ticks of output per edge
 *         edgVal           - current output value for edge
 *        +peak             - peak amplitude of waveform
 *
 *  Rets:  status           - true if character was handled
 *
 *----------------------------------------------------------------------------*/      

boolean PureSquare::charEv( char code )    
{
   switch ( code )
   {
      #ifdef INTERN_CONSOLE

      case 'l':                     // set edgLen 
      
         console.getByte( CONSTR("edgLen"), &edgLen );
         break;

      case 'p':                     // set peak amplitude
      {
         byte val;
         if ( console.getByte( CONSTR("peak"), &val ) )
            setPeak( val );
         break;
      }
      #endif

      #ifdef CONSOLE_OUTPUT

      case chrInfo:                 // display object info to console

         super::charEv( chrInfo );
         console.newlntab();
         console.infoByte( CONSTR("peak"), peak );
         console.infoByte( CONSTR("length"), edgLen );
         break;

      #endif

      case '!':                     // perform a reset

         super::charEv( code );  
         edgLen = 0;
         setPeak( 127 );
         break;

      default:

         return super::charEv( code ); // pass unhandled events to super class
   }
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  PureSquare::setPeak
 *
 *  Desc:  Set the peak amplitude.
 *
 *  Args:  peak             - peak amplitude (0-127)
 *
 *  Memb: +edgVal           - current output value per edge
 *        +peak             - peak amplitude of waveform
 *
 *  Note:  A quantum oscillator can only run at frequencies corresponding 
 *         to integral periods. 
 *
 *----------------------------------------------------------------------------*/      

void PureSquare::setPeak( byte peak )
{
   if ( peak > 127 ) peak = 127;

   this->peak = peak;

   edgVal = edgVal >= 0 ? peak : -peak;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  PureSquare::track
 *
 *  Desc:  Set oscillator frequency as close as possible to a target frequency.
 *
 *  Args:  targHz           - target frequency
 *
 *  Glob:  audioRate        - audio playback rate
 *
 *  Memb: +edgLen           - # ticks of output per edge
 *
 *  Note:  A quantum oscillator can only run at frequencies corresponding 
 *         to integral periods. 
 *
 *----------------------------------------------------------------------------*/      

void PureSquare::track( double targHz )
{
   double fPeriod;                  // period of target frequency
   Word   iPeriod;                  // rounded period of target frequency

   fPeriod     = audioRate / targHz;
   iPeriod.val = fPeriod;

   if ( iPeriod._.lsb & 1 )
      ++iPeriod.val;

   if ( iPeriod._.msb > 1 )
      edgLen = 255;
   else
   {
      iPeriod.val >>= 1;
      edgLen = iPeriod._.lsb;
   }   
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  PureSquare::output
 *
 *  Desc:  Write output to an audio buffer.
 *
 *  Args:  buf              - ptr to audio buffer  
 *
 *  Glob:  audioBufSz       - size of system audio buffers
 *
 *  Memb:  edgLen           - # ticks of output per edge
 *        +edgVal           - current output value for edge
 *        +flipDC           - downcounter to next edge flip
 *
 *----------------------------------------------------------------------------*/      

void PureSquare::output( char *buf )
{
   byte icnt = audioBufSz;           // write this many samples

   if ( edgLen )
   {
      while ( icnt-- )
      {
         if ( --flipDC == 0 )
         {
            edgVal = -edgVal;
            flipDC = edgLen;
         }
         *buf++ = edgVal;
      }
   }
   else
   {
      while ( icnt-- )
         *buf++ = 0;
   }
}
    
#ifdef CONSOLE_OUTPUT
const char *PureSquare::prompt() 
{ 
   return CONSTR("pureSq"); 
}
#endif

/*----------------------------------------------------------------------------*
 *
 *  Name:  PureSquare::realFreq
 *
 *  Desc:  Return the real (instantaneous) frequency. 
 *
 *  Rets:  freq             - real frequency
 *
 *  Memb:  edgLen           - # ticks of output per edge
 *
 *----------------------------------------------------------------------------*/      

double PureSquare::realFreq()
{
   return periodHz( edgLen*2 );
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  PureSquare::setEdgLen
 *
 *  Desc:  Set the edge length.
 *
 *  Args:  length           - value for edge length
 *
 *  Memb: +edgLen           - # ticks of output per edge
 *
 *----------------------------------------------------------------------------*/

void PureSquare::setEdgLen( byte length )
{
   edgLen = length;
}

/******************************************************************************
 *
 *                                 SweepOsc 
 *                                                                            
 ******************************************************************************/

#define MIN_STRETCH 1               // minimum stretch factor
#define PWR         4               // binary exponent of max stretch 
#define MAX_STRETCH 1 << PWR        // maximum stretch factor

/*----------------------------------------------------------------------------*
 *
 *  Name:  SweepOsc::calibrate
 *
 *  Desc:  Calculate the step, and insure that the current edgLen lies within
 *         the range of the sweep.
 *
 *  Memb:  begin            - beginning edge length
 *        +edgLen           - # ticks of output per edge
 *         end              - ending edge length
 *        +step             - step edgLen this much per dynamic update
 *
 *----------------------------------------------------------------------------*/

void SweepOsc::calibrate()
{
   if ( end > begin )
   {
      step = 1;
      if ( edgLen >= end )
         edgLen = begin + (end - begin)/2;
   }
   else
   {
      step = 255;
      if ( edgLen != 0 && edgLen <= end )
         edgLen = end + (begin - end)/2;
   }
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  SweepOsc::charEv
 *
 *  Desc:  Process a character event.
 *
 *  Args:  code             - character to process
 *
 *  Memb: +autotrig         - if true, trigger a new sweep upon reaching end
 *         begin            - beginning edge length
 *         edgLen           - # ticks of output per edge
 *         end              - ending edge length
 *        +flipDC           - downcounter to next edge flip
 *        +repeats          - # of times to repeat auto-trigger (0 = forever)
 *        +repsToGo         - # of repeats remaining in ongoing auto-trigger
 *         stretch          - stretch factor (# updates between edge bumps)
 *
 *  Rets:  status           - true if character was handled
 *
 *----------------------------------------------------------------------------*/      

bool SweepOsc::charEv( char code )    
{
   switch ( code )
   {
      case chrTrigger:                    // trigger the sweep 

         flipDC = edgLen;
         startSweep();      

         if ( autotrig )
            repsToGo = repeats;
         break;

     #ifdef INTERN_CONSOLE
      
      case 'a':                           // set auto-trigger 
      
         console.getBool( CONSTR("autotrig"), &autotrig );
         break;

      case 'b':                           // set begin 
      
         if ( console.getByte( CONSTR("begin"), &begin ) )
            setBegin( begin );
         break;
      
      case 'e':                           // set end 
      
         if ( console.getByte( CONSTR("end"), &end ) )
            setEnd( end );
         break;

      case 'r':                           // set repeats 
      
         if ( console.getByte( CONSTR("repeats"), &repeats ) )
            setRepeats( repeats );
         break;

      case 's':                           // set stretch factor 
      
         if ( console.getByte( CONSTR("stretch"), &stretch ) )
            setStretch( stretch );
         break;

     #endif // INTERN_CONSOLE

     #ifdef CONSOLE_OUTPUT

      case chrInfo:                       // display object info to console

         super::charEv( chrInfo );
         console.infoByte( CONSTR("begin"), begin );
         console.infoByte( CONSTR("end"), end );
         console.newlntab();
         console.infoBool( CONSTR("auto"), autotrig );
         console.infoByte( CONSTR("repeats"), repeats );
         console.infoByte( CONSTR("stretch"), stretch );
         break;

     #endif // CONSOLE_OUTPUT

      case '!':                           // perform a reset

         super::charEv( code );  

         setMute( false );                // (enables trigger)
         setBegin( 1 );
         setEnd( 255 );
         setStretch( 1 );
         tracking  = false;
         autotrig  = false;
         repeats   = 0;
         break;

      default:

         return super::charEv( code );    // pass unhandled events to super class
   }
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  SweepOsc::dynamics
 *
 *  Desc:  Perform a dynamic update.
 *
 *  Memb:  autotrig         - if true, trigger a new sweep upon reaching end
 *        +bumpDC           - downcounter to mext bump of edge length
 *        +edgLen           - # ticks of output per edge
 *         repeats          - # of times to repeat auto-trigger (0 = forever)
 *        +repsToGo         - # of repeats remaining in ongoing auto-trigger
 *         step             - step edgLen this much per dynamic update
 *
 *----------------------------------------------------------------------------*/

void SweepOsc::dynamics()
{
   if ( edgLen != 0 )                  // if sweep is on-going
   {
      if ( edgLen == end )
      {
         if ( autotrig )
         {
            if ( repeats == 0 )
               startSweep();
            else
               if ( --repsToGo == 0 )
                  edgLen = 0;
               else
                  startSweep();
         }
         else
            edgLen = 0;
      }
      else if ( --bumpDC == 0 )
         loadEdge( edgLen + step );
   }
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  SweepOsc::evHandler
 *
 *  Desc:  Handle an onboard event.
 *
 *  Args:  ev               - onboard event
 *
 *  Rets:  status           - true if the event was handled
 *
 *----------------------------------------------------------------------------*/      

bool SweepOsc::evHandler( obEvent ev )
{
   switch ( ev.type() )
   {
      case POT0:  
         
         setBegin( ev.getPotVal() );
         break;

      case POT1:  
      
         setEnd( ev.getPotVal() );
         break;

      default:
         return super::evHandler(ev);
   }
   return true;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  SweepOsc::loadEdge
 *
 *  Desc:  Set the edge length to a new value.
 *
 *  Args:  ev               - onboard event
 *
 *  Memb: +bumpDC           - downcounter to mext bump of edge length
 *        +edgLen           - # ticks of output per edge
 *
 *----------------------------------------------------------------------------*/

void SweepOsc::loadEdge( byte val )
{
   edgLen   = val;

   // cap the instantaneous stretch factor (via bumpDC) to avoid audible 
   // quantization effect (instead of a steady sweep)

   byte maxOutAt;

   maxOutAt = 1 + (val >> PWR);
   bumpDC   = maxOutAt < stretch ? maxOutAt : stretch;
}

#ifdef CONSOLE_OUTPUT
const char *SweepOsc::prompt() 
{ 
   return CONSTR("sweep"); 
}
#endif

/*----------------------------------------------------------------------------*
 *
 *  Name:  SweepOsc::setBegin
 *
 *  Desc:  Set the beginning edge length for the sweep.
 *
 *  Args:  length           - value for beginning edge length
 *
 *  Memb: +begin            - beginning edge length
 *
 *----------------------------------------------------------------------------*/

void SweepOsc::setBegin( byte val )
{
   begin = val ? val : 1;           // must be at least 1
   calibrate();
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  SweepOsc::setEnd
 *
 *  Desc:  Set the ending edge length for the sweep.
 *
 *  Args:  length           - value for ending edge length
 *
 *  Memb: +end              - ending edge length
 *
 *----------------------------------------------------------------------------*/

void SweepOsc::setEnd( byte val )
{
   end  = val ? val : 1;            // must be at least 1
   calibrate();
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  SweepOsc::setRepeats
 *
 *  Desc:  Set the number of times to repeat per trigger.
 *
 *  Args:  repeats          - new value for repeats
 *
 *  Memb: +repeats          - # of times to repeat auto-trigger (0 = forever)
 *
 *----------------------------------------------------------------------------*/

void SweepOsc::setRepeats( byte val )
{
   repeats = val;
   // if ( repsToGo > repeats ) repsToGo = repeats;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  SweepOsc::setStretch
 *
 *  Desc:  Set the stretch factor for the sweep.
 *
 *  Args:  val              - new stretch factor
 *
 *  Memb: +stretch          - stretch factor (# of updates between edge bumps)
 *
 *----------------------------------------------------------------------------*/

void SweepOsc::setStretch( byte val )
{
   if ( val < MIN_STRETCH ) val = MIN_STRETCH;
   if ( val > MAX_STRETCH ) val = MAX_STRETCH;
   stretch = val;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  SweepOsc::startSweep
 *
 *  Desc:  Initiate a sweep of the oscillator.
 *
 *----------------------------------------------------------------------------*/

void SweepOsc::startSweep()
{
   loadEdge( begin );
}

