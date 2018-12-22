/*
    VirtualPot.cpp  

    Implementation of Quantum Oscillator classes.

    ---------------------------------------------------------------------------
 
    Copyright (C) 2018, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
   ---------------------------------------------------------------------------
*/

#include "Console_.h"
#include "VirtualPot.h"

/******************************************************************************
 *
 *                                VirtualPot 
 *                                                                            
 ******************************************************************************/

/*----------------------------------------------------------------------------*
 *
 *  Name:  VirtualPot::charEv
 *
 *  Desc:  Process a character event.
 *
 *  Args:  code             - character to process
 *
 *  Glob:  NumPots          - number of onboard pots
 *
 *  Memb: +left             - leftmost pot value
 *         pos              - current pot position
 *        +right            - rightmost pot value
 *
 *  Rets:  status           - true if character was handled
 *
 *----------------------------------------------------------------------------*/      

bool VirtualPot::charEv( char code )    
{
   switch ( code )
   {
      #ifdef INTERN_CONSOLE

      case 'l':                  

         if ( console.getByte( CONSTR("left"), &left ) )
            setRange( left, right );
         break;

      case 'p':                  // set current pot position

         if ( console.getByte( CONSTR("pos"), &pos ) )
            setPos( pos );
         break;

      case 'r':                  

         if ( console.getByte( CONSTR("right"), &right ) )
            setRange( left, right );
         break;

      #endif

      #ifdef CONSOLE_OUTPUT

      case chrBrief:

         Mode::charEv( code );
         break;

      case chrInfo:

         if ( code == chrInfo )
            super::charEv( code );

         console.infoByte( CONSTR("pos"), pos );
         console.infoByte( CONSTR("val"), getVal() );
         console.infoByte( CONSTR("left"), left );
         console.infoByte( CONSTR("right"), right );
         break;

      #endif

      case '!':                        // perform a reset

         super::charEv( code );
         setMinMax( 0, 255 );
         setRange( 0, 255 );
         break;

      default:

         return super::charEv( code );  
   }
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  VirtualPot::evHandler
 *
 *  Desc:  Handle an onboard event.
 *
 *  Args:  ev               - onboard event
 *
 *  Rets:  status           - true if the event was handled
 *
 *----------------------------------------------------------------------------*/      

bool VirtualPot::evHandler( obEvent ev )
{
   switch ( ev.type() )
   {
      case POT0:  
         
         setPos( ev.getPotVal() );
         break;

      default:
         return super::evHandler(ev);
   }
   return true;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  VirtualPot::evaluate
 *
 *  Desc:  Return the value at a specified position.
 *
 *  Args:  pos              - pot position
 *
 *  Rets:  val              - value at pos
 *
 *----------------------------------------------------------------------------*/      

byte VirtualPot::evaluate( byte pos )        
{
   double fval = left + pos*unit + 0.5;
   return (byte )fval;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  VirtualPot::setMinMax
 *
 *  Desc:  Set the minimuum and maximum permitted pot values.
 *
 *  Args:  min              - minimum permitted value
 *         max              - maximum permitted value
 *
 *  Memb: +max              - maximum permitted value
 *        +min              - minimum permitted value
 *
 *----------------------------------------------------------------------------*/      

void VirtualPot::setMinMax( byte min, byte max )
{
   if ( min <= max )
   {
      this->min = min;
      this->max = max;
   }
   else
   {
      this->min = max;
      this->max = min;
   }
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  VirtualPot::setRange
 *
 *  Desc:  Set the values for the leftmost and rightmost pot positions.
 *
 *  Args:  left             - desired leftmost pot value
 *         right            - desired rightmost pot value
 *
 *  Memb: +left             - leftmost pot value
 *         max              - maximum permitted value
 *         min              - minimum permitted value
 *        +right            - rightmost pot value
 *
 *  Note: The difference between a pot position and value is that the value
 *        is a function of the pot position (and the settings for left and 
 *        right), and is obtained by a call to evaluate().
 *
 *        The input values for left and right will automatically be clipped
 *        to fit (inclusively) within the min/max range.
 *
 *----------------------------------------------------------------------------*/      

void VirtualPot::setRange( byte left, byte right)
{
   if ( left < min ) left = min;   
   if ( left > max ) left = max;
   if ( right < min ) right = min;  
   if ( right > max ) right = max;

   this->left = left;
   this->right = right;

   int range = right-left;
   unit = range / 255.0;
}

/******************************************************************************
 *
 *                              VirtualPotDemux
 *                                                                            
 ******************************************************************************/

/*----------------------------------------------------------------------------*
 *
 *  Name:  VirtualPotDemux::charEv
 *
 *  Desc:  Process a character event.
 *
 *  Args:  code             - character to process
 *
 *  Memb:  lineOut[]        - each line pts to a VirtualPot or NULL
 *        +mask             - 1 bit per line: if bit set, line is enabled
 *
 *  Rets:  status           - true if character was handled
 *
 *----------------------------------------------------------------------------*/      

bool VirtualPotDemux::charEv( char code )    
{
   switch ( code )
   {
      #ifdef INTERN_CONSOLE

      case 'm':                        // set output mask

         console.getBits( CONSTR( "mask"), MaxLines, &mask );
         break;

      #endif

      #ifdef CONSOLE_OUTPUT

      case chrBrief:

         super::charEv( chrBrief );
         console.space();
         console.infoBits( CONSTR( "mask"), MaxLines, mask );
         break;

      case chrInfo:                    // display object info to console

         super::charEv( chrInfo );
         console.newlntab();
         console.infoBits( CONSTR( "mask"), MaxLines, mask );
         for ( byte i = 0; i < MaxLines; i++ )
         {
            if ( ! lineOut[ i ] ) continue;
            console.newlntab();
            console.print( (char )('0' + i) );   
            console.romprint( CONSTR(": ") );
            console.romprint( lineOut[ i ]->prompt() );
         }
         break;

      #endif

      case '!':                        // perform a reset

         super::charEv( code );
         setMute( false );
         for ( byte i = 0; i < MaxLines; i++ )
            connect(i, NULL);
         mask = 0;
         break;

      default:

         return super::charEv( code );  
   }
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  VirtualPotDemux::connect
 *
 *  Desc:  Connect a line to a VirtualPot.
 *
 *  Args:  lineNumber       - line to set up (0 to MaxLines)
 *         ptr              - pointer to VirtualPot (or NULL)
 *
 *  Memb: +lineOut[]        - each line pts to a VirtualPot or NULL
 *
 *  Note:  This routine can be used to disconnect a line by setting the ptr 
 *         argument to NULL.
 *
 *----------------------------------------------------------------------------*/      

void VirtualPotDemux::connect( byte lineNumber, VirtualPot *ptr )
{
   if ( lineNumber < MaxLines )
      lineOut[ lineNumber ] = ptr;
}

void VirtualPotDemux::lineOn( byte lineNum )
{
   byte lineMask = 1;
   while ( lineNum-- )
      lineMask <<= 1;
   mask |= lineMask;
}

void VirtualPotDemux::lineOff( byte lineNum )
{
   byte lineMask = 1;
   while ( lineNum-- )
      lineMask <<= 1;
   mask &= ~lineMask;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  VirtualPotDemux::setPos
 *
 *  Desc:  Set the pot position and propagate it through all enabled lines.
 *
 *  Args:  pos              - pot position
 *
 *  Memb:  lineOut[]        - each line pts to a VirtualPot or NULL
 *         mask             - 1 bit per line: if bit set, line is enabled
 *
 *----------------------------------------------------------------------------*/      

void VirtualPotDemux::setPos( byte pos )
{
   if ( muted() ) return;           // propagate only if router is ON

   super::setPos( pos );
   byte value = evaluate( pos );

   byte nthOut = 0;
   byte nthBit = 1;
   while ( nthOut < MaxLines )
   {
      if ( ( mask & nthBit ) && lineOut[ nthOut ] )
         lineOut[ nthOut ]->setPos( value );
      ++nthOut;
      nthBit <<= 1;
   }
}

