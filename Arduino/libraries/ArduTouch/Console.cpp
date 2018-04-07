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
   #define STR_BUFSZ 16
#else
   #define STR_BUFSZ 1
#endif

char buf[ STR_BUFSZ ];                 // buffer for conversions to string

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
 *  Desc:  Convert a double to a string.
 *
 *  Args:  val              - a double value
 *         str              - address of buffer to store string  
 *
 *  Note:  Instead of calling the library routine dtostrf(), some inline logic
 *         using ltoa() saves around 1100 bytes of ROM.
 *
 *         The value to be converted must not exceed the max value for a long
 *         when place-shifted by NUM_DEC_PLACES. 
 *
 *----------------------------------------------------------------------------*/      

#define NUM_DEC_PLACES  3           // # of digits to display after decimal pt
//#define USE_DTOSTRF               // uncomment to use Arduino library function

void toStr( double val, char *str )
{
#ifdef CONSOLE_OUTPUT

 #ifdef USE_DTOSTRF

   dtostrf( val, STR_BUFSZ, NUM_DEC_PLACES, str ); 

 #else   // override library routine (to save space)

   if ( val < 0 )
   {
      *str++ = '-';
      val = -val;
   }

   // pre-pend '0's; place-shift val

   for ( byte i = NUM_DEC_PLACES; i > 0; i-- )
   {
      if ( val < 1 )
         *str++ = '0';
      val *= 10;
   }

   long integral = val;
   val -= integral;

   if ( val > .99 ) ++integral; // round within .99 epsilon

   ltoa( integral, str, 10 );   // convert place-shifted integral value to str

   // prepend 0s of necessary 

   // shift digits and insert decimal pt

   while ( *str != 0 ) str++;   // move to end of str
   str -= NUM_DEC_PLACES;       // move back to decimal insertion pt

   for ( char i = NUM_DEC_PLACES; i >= 0; i-- )
      str[ i + 1 ] = str[i];

   *str = '.';                  // insert decimal pt

 #endif // USE_DTOSTRF

#endif  // CONSOLE_OUTPUT
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  toStr
 *
 *  Desc:  Convert a double to a string.
 *
 *  Args:  val              - a double value
 *
 *  Rets   str              - address of buffer to store string  
 *
 *----------------------------------------------------------------------------*/      

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

   InputMode() 
   { 
      flags |= MENU;                   // persistent keybrd menu
   }

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

   char  result;                       // numeric result of input (-1 = no input)
   byte  max;                          // max allowable digit

   DigitMode() 
   { 
      flags &= ~ECHO; 
   }

   boolean charEv( char code )
   {
      if ( isDigit( code ) && code <= '0' + max )
      {
         result = code - '0';
         console.print( code );
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
      result    = -1;
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

/******************************************************************************
 *
 *                                  Console 
 *
 ******************************************************************************/

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

/*----------------------------------------------------------------------------*
 *
 *  Name:  Console::exe
 *
 *  Desc:  Execute a macro string. 
 *
 *  Args:  macStr           - ptr to macro string in ROM.
 *
 *  Memb: +atMacro          - ptr into macro string currently being executed
 *
 *----------------------------------------------------------------------------*/
       
void Console::exe( const char *m )
{
   const char *nestMacro = atMacro;

   atMacro = m;

   if ( ! nestMacro ) 
      disable();

   while ( atMacro )
   {
      input();
      if ( idle ) idle();
   }

   atMacro = nestMacro;
   if ( ! nestMacro ) 
      enable();
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Console::exeIn
 *
 *  Desc:  Execute a macro string within a specified mode. 
 *
 *  Args:  macStr           - ptr to macro string in ROM.
 *         exeMode          - mode within which to execute macro
 *
 *  Memb:  _modeStk[]       - obj ptrs of nested console modes
 *         modeSP           - stack pointer for _modeStk[]
 *
 *  Note:  If exeMode is not the top mode on the stack, it will be pushed for
 *         the duration of the macro.
 *
 *         This method restores the modeSP at the end of macro execution, so
 *         the macro does not need to pop any modes that may have been pushed
 *         during it's execution. 
 *
 *----------------------------------------------------------------------------*/
       
void Console::exeIn( const char *m, Mode *exeMode )
{
   byte saveSP = modeSP;

   if ( _modeStk[ modeSP ] != exeMode ) 
      _modeStk[ ++modeSP ] = exeMode;

   exe(m);

   modeSP = saveSP;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Console::getDigit
 *
 *  Desc:  Display an input prompt, and wait for a digit to be input. 
 *
 *  Args:  prompt           - string to display to left of input field
 *         maxDigit         - numeric value of max allowable digit 
 *
 *  Rets:  result           - numeric value of digit (-1 = "aborted input")
 *
 *----------------------------------------------------------------------------*/

char Console::getDigit( const char *prompt, byte max )
{
   #ifdef INTERN_CONSOLE
      digitMode.init( prompt, max );
      runMode( &digitMode );
      return digitMode.result;
   #else
      return -1;
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
 *  Name:  Console::getStr
 *
 *  Desc:  Display an input prompt, and wait for a string to be input, then 
 *         write the address of the input string to a handle supplied by the
 *         calling routine.
 *
 *  Args:  prompt           - string to display to left of input field
 *         handleStr        - address of handle to be written to 
 *
 *  Rets:  status           - if true, input was successfully completed
 *
 *  Note:  If this routine returns a false status (i.e., the user hit escape
 *         before completing input) then no value is written to the address
 *         supplied in the 2nd argument.
 *
 *         The input string will only be preserved until the next call to
 *         a get*() routine.
 *
 *----------------------------------------------------------------------------*/
       
boolean Console::getStr( const char *prompt, char **val ) 
{
   #ifdef INTERN_CONSOLE
      if ( getStr( prompt ) )
      {
         *val = strMode.result();
         return true;
      }
      else
         return false;
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
   #ifdef CONSOLE_OUTPUT
      Serial.print( val );
   #endif
   endInfo();
}

void Console::init( Mode *mode, void (*idle_routine)() )
{
   modeSP = 0;
   setMode( mode ? mode : &nullMode );
   setIdle( idle_routine );
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Console::inMode
 *
 *  Desc:  Return a boolean indicating whether a given mode is the current 
 *         mode (i.e. is the top mode on the mode stack ).
 *
 *  Args:  tryMode          - ptr to mode to test as current mode
 *
 *  Rets:  status           - if true, tryMode is the current mode
 *
 *  Memb:  _modeStk[]       - obj ptrs of nested console modes
 *         modeSP           - stack pointer for _modeStk[]
 *
 *----------------------------------------------------------------------------*/
       
boolean Console::inMode( Mode *tryMode )
{
   return ( _modeStk[ modeSP ] == tryMode );
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Console::input
 *
 *  Desc:  Process the next character, if any, from the input stream.
 *
 *  Args:  macStr           - ptr to macro string in ROM.
 *
 *  Memb: +atMacro          - ptr into macro string currently being executed
 *
 *----------------------------------------------------------------------------*/

void Console::input()
{
   char chr;

   /* get next character; if none available, return. */

   if ( atMacro )    // input stream is a string in ROM
   {
      chr = pgm_read_byte_near( atMacro++ ); 
      if ( ! chr )
      {
         atMacro = NULL;
         return;
      }
   }
   else              // input stream is the serial port
   {
      #ifdef USE_SERIAL_PORT
      if ( Serial.available() > 0 )
         chr = Serial.read();
      else
      #endif
         return;
   }

   /* send character to current Mode */

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

   if ( o.type() == BUT0_TAP || o.type() == BUT1_TAP )
      stackEv( o );
   else
      _modeStk[ modeSP ]->evHandler( o ); 
}

void Console::postKeyDn( byte pos, byte oct )  
{
   key k( pos, oct );

   #ifdef KEYBRD_MENUS

   // if the top mode has its menu enabled, pass the key to the mode's menu() 
   // method, which will return a character. If this character is non-null, 
   // call the mode's charEv() method with it.

   byte menuFlags = _modeStk[ modeSP ]->flags&( Mode::MENU );
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
      obEvent ev;
      ev.clean();
      ev.setKeyDn( k );
      stackEv( ev );
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
      obEvent ev;
      ev.clean();
      ev.setKeyUp( k );
      stackEv( ev );
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

/*----------------------------------------------------------------------------*
 *
 *  Name:  Console::ongoing
 *
 *  Desc:  Continue the console's ongoing tasks for one cycle.
 *
 *  Memb:  idle             - ptr to idling routine
 *
 *  Note:  Call this method when idling for a condition to be satisfied,
 *         inorder to keep the system's console and audio I/O running.
 *
 *----------------------------------------------------------------------------*/
       
void Console::ongoing()                     
{
   input();
   if ( idle ) idle();
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

void Console::runModeWhile( Mode *x, boolean *condition )
{
   pushMode( x );
   while ( *condition )
   {
      input();
      if ( idle ) idle();
   }
   popMode();
}

void Console::setIdle( void (*x)() )
{
   idle = x;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Console::setMode
 *
 *  Desc:  Set the current (top) mode.
 *
 *  Args:  ptrMode          - ptr to Mode to set as the current mode
 *
 *  Memb:  _modeStk[]       - obj ptrs of nested console modes
 *         modeSP           - stack pointer for _modeStk[]
 *
 *----------------------------------------------------------------------------*/      

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

/*----------------------------------------------------------------------------*
 *
 *  Name:  Console::stackEv
 *
 *  Desc:  Post an event iteratively to each mode in the stack until the event 
 *         is handled, or the bottom of the mode stack is reached.
 *
 *  Args:  event            - event to be posted
 *
 *----------------------------------------------------------------------------*/      

void Console::stackEv( obEvent ev ) 
{
   signed char stkptr = modeSP;
   while ( stkptr >= 0 && ! _modeStk[ stkptr ]->evHandler( ev ) ) 
      --stkptr;
}


