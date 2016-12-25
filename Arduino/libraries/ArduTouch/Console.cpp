/*
    Console.cpp  

    Implementation of the Console and auxillary classes.

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
#include "System.h"                          
#include "Audio.h"

/* ----------------------     public variables    -------------------------- */

Console  console;

/* ----------------------    private variables    -------------------------- */

#ifdef CONSOLE_OUTPUT
   char buf[16];                  // buffer for conversions to string
#else
   char buf[1];
#endif

/* ----------------------     public functions    -------------------------- */

void console_setup( Mode *iniMode )
{
   #ifdef USE_SERIAL_PORT
      extern const unsigned long __BAUDRATE__;        \
      Serial.begin( __BAUDRATE__ );
   #endif

   console.enable();

   #ifdef CONSOLE_OUTPUT

      extern const char __PROGNAME__[];               
      extern const char __PROGVER__[];                

      console.newlntab();
      console.romprint( __PROGNAME__ );
      console.romprint( CONSTR( " [" ) );
      console.romprint( __PROGVER__ );
      console.print( ']');

   #endif

   console.init( iniMode, &device_io );
}

void exit_sketch()
{
   console.done();
   exit( 0 );
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  toStr
 *
 *  Desc:  Convert a double to a string with 2 decimal places.
 *
 *  Args:  val              - a double value
 *         str              - address of buffer to store string  
 *
 *  Note:  Instead of calling the library routine dtostrf(), some inline logic
 *         using ltoa() saves 1138 bytes of ROM.
 *
 *         The integral portion is assumed to not exceed max value for a long.
 *
 *         The hundreths place is rounded up, unless it would cause the
 *         integral portion to be bumped.
 *
 *----------------------------------------------------------------------------*/      

void toStr( double val, char *str )
{
#ifdef CONSOLE_OUTPUT

   // dtostrf( val, 2, 2, str ); 

   /* convert integral portion */

   long   integral   = val;
   double fractional = val - integral;

   ltoa( integral, str, 10 );

   while ( *str != 0 ) str++;   // move to end of str
   *str++ = '.';                // add decimal pt

   /* convert 1st 2 places of fractional portion */

   if ( fractional < 0 )
      fractional = -fractional;

   fractional *= 100;
   integral = fractional;
   fractional -= integral;
   if ( fractional >= .5 && integral < 99 )
      integral++;

   if ( integral < 10 )
     *str++ = '0';

   ltoa( integral, str, 10 );

#endif
}

char *toStr( double val )
{
#ifdef CONSOLE_OUTPUT
   toStr( val, &buf[0] );
#endif
   return &buf[0];
}

/* ----------------------    InputMode classes      -------------------------- */

#ifdef INTERN_CONSOLE

class InputMode : public Mode
{
   static char priorDelim;             // console delimiter prior to push

   public:

   const char *_prompt;                // ptr to prompt str

   InputMode() {}

   boolean charEv( char code )
   {
      switch ( code )
      {
         case focusPUSH:

            priorDelim = console.delim;
            console.delim = ':';
            break;

         case focusPOP:

            console.delim = priorDelim;
            break;

         default:

            return Mode::charEv( code );
      }
      return true;
   }

   #ifdef KEYBRD_MENUS
   char menu( key k )
   {
      return ( k.position() <= 9 ? '0' + k.position() : 0 );
   }
   #endif

   #ifdef CONSOLE_OUTPUT
      const char *prompt()  { return _prompt; }
   #endif

} ;

char InputMode::priorDelim;

class DigitMode : public InputMode
{
   public:

   DigitMode() { flags &= ~ECHO; }

   char  result;                       // result of input (0 = no input)
   byte  max;                          // max allowable digit

   boolean charEv( char code )
   {
      if ( isDigit( code ) )
      {
         if ( code > '0' + max )
            code = '0' + max;
         console.print( code );
         result = code;
         console.popMode();
         return true;
      }
      else
         return InputMode::charEv( code );
   }

   void init( const char *prompt, byte max )
   {
      _prompt   = prompt;
      this->max = max;     
      result    = 0;
   }

} digitMode;


class StrMode : public InputMode
{
   static const byte MaxStrLen = 20;   // sans null terminator byte 

   char buffer[ MaxStrLen+1 ];         // character buffer
   byte pos;                           // current (insert) position in buffer

   public:

   boolean charEv( char code )
   {
      switch ( code )
      {
         case chrESC:

            buffer[0] = 0;             // fall thru to case chrCR for popMode

         case chrCR:

            console.popMode();
            break;

         case focusPUSH:
         case focusPOP:

            InputMode::charEv( code );
            break;

         default:

            if ( pos <= MaxStrLen )
            {
               buffer[ pos++ ] = code;
               buffer[ pos ]   = 0;
            }
      }
      return true;
   }
  
   boolean evHandler( obEvent ev ) 
   {
      // map the pressing or tapping of button 1 to a carriage return

      switch ( ev.type() )
      {
         case BUT1_PRESS:
         case BUT1_TAP:

            charEv( chrCR );
            return true;

         default:

            return Mode::evHandler( ev );
       }
   }

   void init( const char * x )
   {
      _prompt   = x;
      pos       = 0;
      buffer[0] = 0;
   }

   #ifdef KEYBRD_MENUS
   char menu( key k )
   {
      byte keypos = k.position();
      char keychr;

      if ( keypos <= 9 )
         keychr = '0' + keypos;
      else if ( keypos == 10 )
         keychr = '-';
      else if ( keypos == 11 )
         keychr = '.';

      return keychr;
   }
   #endif

   char   *result()  { return &buffer[0]; }

} strMode ;  

#endif  // ifdef INTERN_CONSOLE

/* ----------------------      Console class       -------------------------- */

#define alphaCR         '\\'           // alphanumeric alternative to <CR>
#define alphaESC        '`'            // alphanumeric alternative to <ESC>

Mode nullMode;                         // default mode when none is specified

/*----------------------------------------------------------------------------*
 *
 *  Name:  Console::disable
 *
 *  Desc:  Disable output to the console.
 *
 *  Memb: +output           - if set, console output is enabled
 *
 *----------------------------------------------------------------------------*/ 
      
void Console::disable()
{
   output = false;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Console::done
 *
 *  Desc:  Flush console output at sketch end.
 *
 *----------------------------------------------------------------------------*/ 
      
void Console::done()
{
#ifdef CONSOLE_OUTPUT
   newlntab();
   romprint( CONSTR("Done.") );
   Serial.flush();
#endif
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Console::enable
 *
 *  Desc:  Enable output to the console.
 *
 *  Memb: +output           - if set, console output is enabled
 *
 *  Note:  This routine has no effect if console output has been permanently
 *         disabled at compile time by not defining the symbolic constant
 *         CONSOLE_OUTPUT.
 *
 *----------------------------------------------------------------------------*/      

void Console::enable()
{
   output = true;
}

void Console::exe( const char *m )
{
   atMacro = m;
   disable();
}

char Console::getDigit( const char *prompt, byte max )
{
   #ifdef INTERN_CONSOLE
      digitMode.init( prompt, max );
      runMode( &digitMode );
      return digitMode.result;
   #else
      return false;
   #endif
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Console::getDouble
 *
 *  Desc:  Display an input prompt, and wait for a string to be input, then 
 *         interpret this string as a floating pt number and write it to
 *         the address supplied by the calling routine.
 *
 *  Args:  prompt           - string to display to left of input field
 *         ptrDouble        - address of double to be written to 
 *
 *  Rets:  status           - if true, input was successfully completed
 *
 *  Note:  If this routine returns a false status (i.e., the user hit escape
 *         before completing input) then no value is written to the address
 *         supplied in the 2nd argument.
 *
 *----------------------------------------------------------------------------*/
       
boolean Console::getDouble( const char *prompt, double *val )
{
  #ifdef INTERN_CONSOLE

   boolean status = getStr( prompt );

   if ( status )
   {
      // *val = (double ) atof( strMode.result() );

      char *str = strMode.result();

      // skip leading spaces

      while ( *str == ' ' ) str++;     

      // process leading '-' 

      int sign = 1;
      while ( *str == '-' )
      {
         sign = -sign;
         str++;
      }

      // process integral portion

      long integral = 0;
      char *intstr  = str;
      while ( *str != '.' && *str != 0 )
         str++;
      char terminus = *str;
      *str = 0;
      integral = atol( intstr );

      // process fractional portion

      double fraction = 0;
      long   divisor  = 1;
      if ( terminus == '.' )
      {
         char *fracstr = str+1;
         while ( *++str >= '0' && *str <= '9' )
            divisor *= 10;
         *str = 0;
         fraction = (double )atol( fracstr )/divisor;
      }

      *val = (integral + fraction) * sign;
   }

   return status;
  #else              // INTERN_CONSOLE not defined
   return false;
  #endif
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Console::getByte
 *
 *  Desc:  Display an input prompt, and wait for a string to be input, then 
 *         interpret this string as an unsigned byte value and write it to
 *         the address supplied by the calling routine.
 *
 *  Args:  prompt           - string to display to left of input field
 *         ptrByte          - address of byte to be written to 
 *
 *  Rets:  status           - if true, input was successfully completed
 *
 *  Note:  If this routine returns a false status (i.e., the user hit escape
 *         before completing input) then no value is written to the address
 *         supplied in the 2nd argument.
 *
 *----------------------------------------------------------------------------*/
       
boolean Console::getByte( const char* prompt, byte* val )
{
   int temp;
   boolean status = getInt( prompt, &temp );
   if ( status )
   {
      if ( temp < 0 || temp > 255 )
         status = false;
      else
         *val = (byte )temp; 
   }
   return status;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Console::getSByte
 *
 *  Desc:  Display an input prompt, and wait for a string to be input, then 
 *         interpret this string as a signed byte value and write it to
 *         the address supplied by the calling routine.
 *
 *  Args:  prompt           - string to display to left of input field
 *         ptrByte          - address of byte to be written to 
 *
 *  Rets:  status           - if true, input was successfully completed
 *
 *  Note:  If this routine returns a false status (i.e., the user hit escape
 *         before completing input) then no value is written to the address
 *         supplied in the 2nd argument.
 *
 *----------------------------------------------------------------------------*/
       
boolean Console::getSByte( const char* prompt, char* val )
{
   int temp;
   boolean status = getInt( prompt, &temp );
   if ( status )
   {
      if ( temp < -128 || temp > 127 )
         status = false;
      else
         *val = temp; 
   }
   return status;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Console::getInt
 *
 *  Desc:  Display an input prompt, and wait for a string to be input, then 
 *         interpret this string as an int value and write it to the address 
 *         supplied by the calling routine.
 *
 *  Args:  prompt           - string to display to left of input field
 *         ptrInt           - address of int to be written to 
 *
 *  Rets:  status           - if true, input was successfully completed
 *
 *  Note:  If this routine returns a false status (i.e., the user hit escape
 *         before completing input) then no value is written to the address
 *         supplied in the 2nd argument.
 *
 *----------------------------------------------------------------------------*/
       
boolean Console::getInt( const char *prompt, int *val )
{
   boolean status = getStr( prompt );

   #ifdef INTERN_CONSOLE
      if ( status )
         *val = (int ) atoi( strMode.result() );
      return status;
   #else
      return false;
   #endif
}

boolean Console::getStr( const char *prompt ) 
{
   #ifdef INTERN_CONSOLE
      strMode.init( prompt );
      runMode( &strMode );
      return ( strMode.result()[0] != 0 );
   #else
      return false;
   #endif
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Console::getULong
 *
 *  Desc:  Display an input prompt, and wait for a string to be input, then 
 *         interpret this string as an unsigned long value and write it to 
 *         the address supplied by the calling routine.
 *
 *  Args:  prompt           - string to display to left of input field
 *         ptrULong         - address of unsigned long to be written to 
 *
 *  Rets:  status           - if true, input was successfully completed
 *
 *  Note:  If this routine returns a false status (i.e., the user hit escape
 *         before completing input) then no value is written to the address
 *         supplied in the 2nd argument.
 *
 *         This routine saves 550 bytes of ROM by not using the standard C
 *         library function strtoul(), which is meant to be more general 
 *         purpose (it supports different radixes).
 *
 *----------------------------------------------------------------------------*/
       
boolean Console::getULong( const char *prompt, unsigned long *val )
{
   boolean status = getStr( prompt );

   #ifdef INTERN_CONSOLE
      if ( status )
      {
         // *val = strtoul( strMode.result(), NULL, 10 ) uses mucho ROM

         unsigned long result = 0;
         char *buf            = strMode.result();
         while ( *buf != '\0')
            result = result*10 + *buf++ - '0';
         *val = result;
      }
      return status;
   #else
      return false;
   #endif
}

void Console::begInfo( const char* name )
{
   print('{');
   romprint( name );
   space();
}

void Console::endInfo()
{
   romprint( CONSTR("} ") );
}

void Console::infoByte( const char* name, byte val )
{
   infoInt( name, val );
}

void Console::infoInt( const char* name, int val )
{
   begInfo( name );
   print( val );
   endInfo();
}

void Console::infoDouble( const char* name, double val )
{
   begInfo( name );
   print( toStr(val) );
   endInfo();
}

void Console::infoStr( const char* name, const char* val )
{
   begInfo( name );
   romprint( val );
   endInfo();
}


void Console::infoULong( const char* name, unsigned long val )
{
   begInfo( name );
   Serial.print( val );
   endInfo();
}

void Console::init( Mode *mode, void (*idle_routine)() )
{
   modeSP = 0;
   setMode( mode ? mode : &nullMode );
   setIdle( idle_routine );
}

void Console::input()
{
   char chr;

   /* get next keystroke; if none available, return. */

   if ( atMacro )
   {
      chr = pgm_read_byte_near( atMacro++ ); 
      if ( ! chr )
      {
         atMacro = NULL;
         enable();
         return;
      }
   }
   else 
   {
      #ifdef USE_SERIAL_PORT
      if ( Serial.available() > 0 )
         chr = Serial.read();
      else
      #endif
         return;
   }

   /* send character event to current Mode */

   if ( chr == alphaCR )
      chr = chrCR;
   else if ( chr == alphaESC )
      chr = chrESC;

   #ifdef CONSOLE_OUTPUT
      if ( (_modeStk[ modeSP ]->flags & Mode::ECHO) && chr != chrCR 
                                                    && chr != chrESC 
                                                    && output 
                                                    )  
         Serial.print( chr );
   #endif

   _modeStk[ modeSP ]->charEv( chr );
   
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Console::newline
 *
 *  Desc:  Send a carriage-return and new-line character to the console.
 *
 *  Memb:  output           - if set, console output is enabled
 *
 *  Note:  Inorder to avoid a timing conflict between the serial port and audio
 *         interrupt which can arise when too many characters are sent to the
 *         serial port in too short a time, this routine incorporates an 
 *         implicit "wait" period during which the audio engine continues to 
 *         run. A wait of 25 output buffers has proven effective at a baud rate 
 *         of 115200. If the baud rate is reduced the number of buffers to wait
 *         will probably have to be increased.
 *
 *----------------------------------------------------------------------------*/      

void Console::newline()
{
#ifdef CONSOLE_OUTPUT
   romprint( CONSTR("\r\n") );
   if ( output )
      audio::wait( 25 );
#endif
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Console::newlntab
 *
 *  Desc:  Move to the next line on the console, and tab to the right of the 
 *         console seam.
 *
 *----------------------------------------------------------------------------*/      

void Console::newlntab()
{
   newline();
   rtab();
}

void Console::newprompt()
{
#ifdef CONSOLE_OUTPUT
   newline();
   const char * p = _modeStk[ modeSP ]->prompt();
   space( SEAM - romstrlen( p ) - 2 );
   romprint( p );
   print( delim );
   space(1);
#endif
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Console::oneShotMenu
 *
 *  Desc:  Interpret next posted key event as a one-shot menu selection. 
 *
 *  Memb:  +oneShot         - interpret next key event as a menu selection.
 *
 *----------------------------------------------------------------------------*/      

void Console::oneShotMenu()                     
{
   oneShot = true;
}

boolean Console::ongoing( Mode *x )
{
   if ( _modeStk[ modeSP ] == x )
   {
      input();
      if ( idle ) idle();
      return true;
   }
   else
      return false;
}

void Console::popMode()
{
   oneShot = false;
   _modeStk[ modeSP ]->charEv( focusPOP ); 
   if ( modeSP > 0 ) 
   {
      --modeSP;
      _modeStk[ modeSP ]->charEv( focusRESTORE ); 
      setMode( _modeStk[ modeSP ] );
   }
   else
      exit_sketch();
}

void Console::postBut( byte num, butAction action )  
{
   obEvent o;

   oneShot = false;
   o.setType( ( num ? BUT1_PRESS : BUT0_PRESS ) + action );   
   if ( _modeStk[ modeSP ]->flags&Mode::PLAYTHRU )
   {
      if ( o.type() == BUT0_TAP )
         o.setType( META_OCTDN );
      else if ( o.type() == BUT1_TAP )
         o.setType( META_OCTUP );
      else if ( o.type() == BUT1_DTAP )
         o.setType( META_ONESHOT );
   }
   _modeStk[ modeSP ]->evHandler( o ); 
}

void Console::postKeyDn( byte pos, byte oct )  
{
   key k( pos, oct );

   #ifdef KEYBRD_MENUS
   byte menuFlags = _modeStk[ modeSP ]->flags&( Mode::MENU | Mode::PLAYTHRU );
   if ( oneShot || ( menuFlags == Mode::MENU ) )
   {
      menuKeyDn = true;
      oneShot   = false;

      char keychr = _modeStk[ modeSP ]->menu( k );
      if ( keychr )
      {
         #ifdef CONSOLE_OUTPUT
            if ( _modeStk[ modeSP ]->flags & Mode::ECHO )
               Serial.print( keychr );
         #endif
         _modeStk[ modeSP ]->charEv( keychr );
      }
   }
   else
   #endif
   {
      obEvent o;
      o.setKeyDn( k );
      signed char stkptr = modeSP;
      while ( stkptr >= 0 && ! _modeStk[ stkptr ]->evHandler( o ) ) 
         --stkptr;
   }
}

void Console::postKeyUp( byte pos, byte oct )  
{
   key k( pos, oct );

   #ifdef KEYBRD_MENUS
   if ( menuKeyDn )
   {
      menuKeyDn = false;
   }
   else
   #endif
   {
      obEvent o;
      o.setKeyUp( k );
      signed char stkptr = modeSP;
      while ( stkptr >= 0 && ! _modeStk[ stkptr ]->evHandler( o ) ) 
         --stkptr;
   }
}

void Console::postPot( byte num, byte val )  
{
   obEvent o;

   oneShot = false;
   o.setPotVal( val );
   o.setType( POT0 + num );   
   _modeStk[ modeSP ]->evHandler( o ); 
}

void Console::print( char c )
{
#ifdef CONSOLE_OUTPUT
   if ( output )
      Serial.print( c );
#endif
}

void Console::print( char *str )
{
#ifdef CONSOLE_OUTPUT
   if ( output )
      Serial.print( str );
#endif
}

void Console::print( int i )
{
#ifdef CONSOLE_OUTPUT
   if ( output )
   {
      itoa( i, buf, 10 );
      Serial.print( buf );
   }
#endif
}

void Console::pushMode( Mode *x )
{
   if ( modeSP < MAX_MODE-1 ) 
   {
      oneShot = false;
      ++modeSP;
      x->charEv( focusPUSH );
      setMode( x );
   }
}

void Console::romprint( const char *str )
{
#ifdef CONSOLE_OUTPUT
   char romChar = pgm_read_byte_near( str++ );
   while ( romChar )
   {
      print( romChar );
      romChar = pgm_read_byte_near( str++ );
   }
#endif
}

byte Console::romstrlen( const char *str )
{
   byte len = 0;
#ifdef CONSOLE_OUTPUT
   char romChar = pgm_read_byte_near( str++ );
   while ( romChar )
   {
      ++len;
      romChar = pgm_read_byte_near( str++ );
   }
#endif
   return len;
}

void Console::rtab()
{
   space( SEAM );
}

void Console::runMode( Mode *x )
{
   pushMode( x );

   int strDepth = modeSP;
   while ( modeSP == strDepth )
   {
      input();
      if ( idle ) idle();
   }
}

void Console::setIdle( void (*x)() )
{
   idle = x;
}

void Console::setMode( Mode *x )
{
   _modeStk[ modeSP ] = x;
   newprompt();
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Console::space
 *
 *  Desc:  Print a space.
 *
 *  Args:  -- none --
 *
 *----------------------------------------------------------------------------*/      

void Console::space()
{
   space(1);
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Console::space
 *
 *  Desc:  Print a specified number of spaces.
 *
 *  Args:  count            - # of spaces to print
 *
 *----------------------------------------------------------------------------*/      

void Console::space( byte repeatCount )
{
#ifdef CONSOLE_OUTPUT
   for ( byte i = 0; i < repeatCount; i++ )
      if ( output )
         Serial.print(' ');
#endif
}


