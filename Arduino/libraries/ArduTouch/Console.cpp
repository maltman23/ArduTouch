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

/* ----------------------    external functions   -------------------------- */

extern void pushDisplay();             // push display state
extern void popDisplay();              // pop display state
extern void resetDisplay( byte );      // reset display stackptr and state

/* ----------------------     public functions    -------------------------- */

void console_setup( Mode *iniMode )
{
   #ifdef USE_SERIAL_PORT
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

class BitMode : public InputMode
{
   typedef InputMode super;            // superclass is InputMode

   public:

   bool  status;                       // status of input
   byte  numBits;                      // # of bits in field
   byte  bitsToGo;                     // # of bits remaining to input
   byte  bitVal;                       // value for bit currently being input
   byte  value;                        // accumulated value for all bits input

   BitMode() 
   { 
      flags &= ~ECHO; 
   }

   boolean charEv( char code )
   {
      switch ( code )
      {
         case '0':                        // input a 0 at current bit position 
         case '1':                        // input a 1 at current bit position 

            console.print( code );
            if ( code == '1' )
               value += bitVal;
            bitVal >>= 1;
            if ( --bitsToGo == 0 )
            {
               status = true;
               console.popMode();
            }
            break;

         case chrCR:                      // complete input early

            value >>= bitsToGo;           // right justify bits already input
            status = true;
            console.popMode();
            break;

         default:

            return super::charEv( code );
      }
      return true;
   }

   void init( const char *prompt, byte numbits )
   {
      _prompt  = prompt;
      numBits  = numbits;
      bitsToGo = numbits;     
      value    = 0;
      bitVal   = 1 << (numBits-1);
      status   = false;
   }

} bitMode;

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
 *  Name:  Console::dispBits
 *
 *  Desc:  Display a byte (or sub-byte) as a string of '0's and '1's.
 *
 *  Args:  value            - byte value
 *         numBits          - # of bits to display (up to 8 bits)
 *
 *  Note:  If numBits is less than 8 than the least significant bits up to
 *         numBits are displayed.
 *
 *----------------------------------------------------------------------------*/
       
void Console::dispBits( byte val, byte numBits )
{
   if ( numBits > 0 && numBits <= 8 )
   {
      byte bitPos = 1 << (numBits-1);
      while ( bitPos )
      {
         print( val & bitPos ? '1' : '0' );
         bitPos >>= 1;
      }
   }
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
   resetDisplay( modeSP );
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Console::getBits
 *
 *  Desc:  Display an input prompt, and wait for a bit string to be input, 
 *         then write it (as a byte) to the address supplied by the calling 
 *         routine.
 *
 *  Args:  prompt           - string to display to left of input field
 *         numBits          - number of bits in string (up to 8)
 *         ptrByte          - address of byte to be written to 
 *
 *  Rets:  status           - if true, input was successfully completed
 *
 *----------------------------------------------------------------------------*/

bool Console::getBits( const char *prompt, byte numBits, byte *val )
{
   #ifdef INTERN_CONSOLE

      if ( numBits == 0 || numBits > 8 )
         return false;

      bitMode.init( prompt, numBits );
      runMode( &bitMode );

      if ( bitMode.status ) 
         *val = bitMode.value;

      return bitMode.status;
      
   #else             // INTERN_CONSOLE not defined

      return false;

   #endif
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
 *  Name:  Console::getBool
 *
 *  Desc:  Display an input prompt, and wait for a string to be input, then 
 *         interpret this string as a boolean and write it to the address 
 *         supplied by the calling routine.
 *
 *  Args:  prompt           - string to display to left of input field
 *         ptrBool          - address of boolean to be written to 
 *
 *  Rets:  status           - if true, input was successfully completed
 *
 *  Note:  If this routine returns a false status (i.e., the user hit escape
 *         before completing input) then no value is written to the address
 *         supplied in the 2nd argument.
 *
 *         Only the 1st character of the string is examined.
 *         If the 1st character if a 't' or 'T' a true value is written.
 *         If the 1st character if a 'f' or 'F' a false value is written.
 *         Any other 1st character results in no value being written. 
 *
 *----------------------------------------------------------------------------*/
       
boolean Console::getBool( const char* prompt, bool* val )
{
   boolean status = getStr( prompt );
   if ( status )
   {
      switch ( strMode.result()[0] )
      {
         case 't':
         case 'T':

            *val = true; 
            break;

         case 'f':
         case 'F':

            *val = false; 
            break;

         default:

            status = false;
      }
   }
   return status;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Console::getChar
 *
 *  Desc:  Display an input prompt, and wait for a string to be input, then 
 *         write the 1st character in the string to the address supplied by 
 *         the calling routine.
 *
 *  Args:  prompt           - string to display to left of input field
 *         ptrChar          - address of boolean to be written to 
 *
 *  Rets:  status           - if true, input was successfully completed
 *
 *  Note:  If this routine returns a false status (i.e., the user hit escape
 *         before completing input) then no value is written to the address
 *         supplied in the 2nd argument.
 *
 *         Only the 1st character of the string is examined.
 *         If the 1st character if a 't' or 'T' a true value is written.
 *         If the 1st character if a 'f' or 'F' a false value is written.
 *         Any other 1st character results in no value being written. 
 *
 *----------------------------------------------------------------------------*/
       
bool Console::getChar( const char* prompt, char* val )
{
   bool status = getStr( prompt );

   if ( status )
      *val = strMode.result()[0]; 

   return status;
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

void Console::begInfo( const char* label )
{
   print('{');
   romprint( label );
   space();
}

void Console::endInfo()
{
   romprint( CONSTR("} ") );
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Console::infoBits
 *
 *  Desc:  Display a bitfield value (preceded by a label).
 *
 *  Args:  label            - ptr to label str
 *         numBits          - # of bits in field (up to 8 bits)
 *         value            - bitfield value
 *
 *  Note:  This method is one of a group of "info" methods which display  
 *         a label/value pair to the console as follows:
 *
 *                           {label value}
 *
 *----------------------------------------------------------------------------*/
       
void Console::infoBits( const char* label, byte numBits, byte val )
{
   begInfo( label );
   dispBits( val, numBits );
   endInfo();
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Console::infoBool
 *
 *  Desc:  Display a boolean value (preceded by a label).
 *
 *  Args:  label            - ptr to label str
 *         value            - boolean value
 *
 *  Note:  This method is one of a group of "info" methods which display  
 *         a label/value pair to the console as follows:
 *
 *                           {label value}
 *
 *----------------------------------------------------------------------------*/
       
void Console::infoBool( const char* label, bool val )
{
   begInfo( label );
   romprint( val ? CONSTR("true") : CONSTR("false") );
   endInfo();
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Console::infoByte
 *
 *  Desc:  Display a byte value (preceded by a label).
 *
 *  Args:  label            - ptr to label str
 *         value            - byte value
 *
 *  Note:  This method is one of a group of "info" methods which display  
 *         a label/value pair to the console as follows:
 *
 *                           {label value}
 *
 *----------------------------------------------------------------------------*/
       
void Console::infoByte( const char* label, byte val )
{
   infoInt( label, val );
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Console::infoByteBits
 *
 *  Desc:  Display a byte value as a bit string (preceded by a label).
 *
 *  Args:  label            - ptr to label str
 *         value            - byte value
 *
 *  Note:  This method is one of a group of "info" methods which display  
 *         a label/value pair to the console as follows:
 *
 *                           {label value}
 *
 *----------------------------------------------------------------------------*/
       
void Console::infoCharBits( const char* label, byte val )
{
   begInfo( label );
   dispBits( val, 8 );
   endInfo();
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Console::infoChar
 *
 *  Desc:  Display a character (preceded by a label).
 *
 *  Args:  label            - ptr to label str
 *         value            - character
 *
 *  Note:  This method is one of a group of "info" methods which display  
 *         a label/value pair to the console as follows:
 *
 *                           {label value}
 *
 *----------------------------------------------------------------------------*/
       
void Console::infoChar( const char* label, char val )
{
   begInfo( label );
   print( val );
   endInfo();
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Console::infoInt
 *
 *  Desc:  Display an integer value (preceded by a label).
 *
 *  Args:  label            - ptr to label str
 *         value            - byte value
 *
 *  Note:  This method is one of a group of "info" methods which display  
 *         a label/value pair to the console as follows:
 *
 *                           {label value}
 *
 *----------------------------------------------------------------------------*/
       
void Console::infoInt( const char* label, int val )
{
   begInfo( label );
   print( val );
   endInfo();
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Console::infoIntBits
 *
 *  Desc:  Display an int value as a bit string (preceded by a label).
 *
 *  Args:  label            - ptr to label str
 *         value            - byte value
 *
 *  Note:  This method is one of a group of "info" methods which display  
 *         a label/value pair to the console as follows:
 *
 *                           {label value}
 *
 *----------------------------------------------------------------------------*/
       
void Console::infoIntBits( const char* label, int val )
{
   begInfo( label );
   Int intBits;
   intBits.val = val;
   dispBits( intBits._.msb, 8 );
   space();
   dispBits( intBits._.lsb, 8 );
   endInfo();
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Console::infoDouble
 *
 *  Desc:  Display an double value (preceded by a label).
 *
 *  Args:  label            - ptr to label str
 *         value            - double value
 *
 *  Note:  This method is one of a group of "info" methods which display  
 *         a label/value pair to the console as follows:
 *
 *                           {label value}
 *
 *----------------------------------------------------------------------------*/

void Console::infoDouble( const char* label, double val )
{
   begInfo( label );
   print( toStr(val) );
   endInfo();
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Console::infoStr
 *
 *  Desc:  Display a string value located in ROM (preceded by a label).
 *
 *  Args:  label            - ptr to label str
 *         value            - ptr to string in ROM
 *
 *  Note:  This method is one of a group of "info" methods which display  
 *         a label/value pair to the console as follows:
 *
 *                           {label value}
 *
 *----------------------------------------------------------------------------*/

void Console::infoStr( const char* label, const char* val )
{
   begInfo( label );
   romprint( val );
   endInfo();
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Console::infoRAMStr
 *
 *  Desc:  Display a string value located in RAM (preceded by a label).
 *
 *  Args:  label            - ptr to label str
 *         value            - strinf value
 *
 *  Note:  This method is one of a group of "info" methods which display  
 *         a label/value pair to the console as follows:
 *
 *                           {label value}
 *
 *----------------------------------------------------------------------------*/

void Console::infoRAMStr( const char* label, char* val )
{
   begInfo( label );
   print( val );
   endInfo();
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Console::infoULong
 *
 *  Desc:  Display an unsigned long value (preceded by a label).
 *
 *  Args:  label            - ptr to label str
 *         value            - unsigned long value
 *
 *  Note:  This method is one of a group of "info" methods which display  
 *         a label/value pair to the console as follows:
 *
 *                           {label value}
 *
 *----------------------------------------------------------------------------*/

void Console::infoULong( const char* label, unsigned long val )
{
   begInfo( label );
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

/*----------------------------------------------------------------------------*
 *
 *  Name:  Console::newprompt
 *
 *  Desc:  Move to the next line on the console and print the prompt string 
 *         for the top mode on the mode stack.
 *
 *  Memb:  modeSP           - stackptr for _modeStk[]
 *         _modeStk[]       - stack of ptrs to nested Mode objects
 *
 *----------------------------------------------------------------------------*/      

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
/*----------------------------------------------------------------------------*
 *
 *  Name:  Console::pushMode
 *
 *  Desc:  Push a mode onto the mode stack.  
 *
 *  Memb:  _ledsStk[]       - stack of nested led states
 *         _modeStk[]       - stack of ptrs to nested Mode objects
 *        +modeSP           - stackptr for _modeStk[]
 *        +oneShot          - interpret next key event as a menu selection
 *
 *----------------------------------------------------------------------------*/      

void Console::pushMode( Mode *x )
{
   if ( modeSP < MAX_MODE-1 ) 
   {
      oneShot = false;
      pushDisplay();
      ++modeSP;
      x->charEv( focusPUSH );
      setMode( x );
   }
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Console::popMode
 *
 *  Desc:  Pop the top mode off the mode stack, restoring the prior mode and
 *         display state of LEDs.  
 *
 *  Memb:  _ledsStk[]       - stack of nested led states
 *         _modeStk[]       - stack of ptrs to nested Mode objects
 *        +modeSP           - stackptr for _modeStk[]
 *        +oneShot          - interpret next key event as a menu selection
 *
 *----------------------------------------------------------------------------*/      

void Console::popMode()
{
   oneShot = false;
   _modeStk[ modeSP ]->charEv( focusPOP ); 
   if ( modeSP > 0 ) 
   {
      --modeSP;
      _modeStk[ modeSP ]->charEv( focusRESTORE ); 
      popDisplay();
      setMode( _modeStk[ modeSP ] );
   }
   else
      exit_sketch();
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Console::postBut
 *
 *  Desc:  Post a pot event to the top mode on the mode stack.  
 *
 *  Args:  num              - button number 
 *         action           - button action
 *
 *  Memb: +oneShot          - interpret next key event as a menu selection
 *         modeSP           - stackptr for _modeStk[]
 *         _modeStk[]       - stack of ptrs to nested Mode objects
 *
 *----------------------------------------------------------------------------*/      

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

/*----------------------------------------------------------------------------*
 *
 *  Name:  Console::postKeyDn
 *
 *  Desc:  Post a key-down event to the mode stack.  
 *
 *  Args:  pos              - key position within the octave 
 *         oct              - octave
 *
 *  Memb: +oneShot          - interpret next key event as a menu selection
 *        +menuKeyDn        - last key down was a menu selection
 *         modeSP           - stackptr for _modeStk[]
 *         _modeStk[]       - stack of ptrs to nested Mode objects
 *
 *----------------------------------------------------------------------------*/      

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

/*----------------------------------------------------------------------------*
 *
 *  Name:  Console::postKeyUp
 *
 *  Desc:  Post a key-up event to the mode stack.  
 *
 *  Args:  pos              - key position within the octave 
 *         oct              - octave
 *
 *  Memb: +menuKeyDn        - last key down was a menu selection
 *
 *----------------------------------------------------------------------------*/      

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

/*----------------------------------------------------------------------------*
 *
 *  Name:  Console::postPot
 *
 *  Desc:  Post a pot event to the top mode on the mode stack.  
 *
 *  Args:  num              - pot number 
 *         val              - pot value
 *
 *  Memb: +oneShot          - interpret next key event as a menu selection.
 *         modeSP           - stackptr for _modeStk[]
 *         _modeStk[]       - stack of ptrs to nested Mode objects
 *
 *----------------------------------------------------------------------------*/      

void Console::postPot( byte num, byte val )  
{
   obEvent o;
   byte    type;

   oneShot = false;

   type    = POT0 + num;

   // If top mode has an enabled frame then add its current frame number 
   // to the basic pot event type to create an extended pot event.

   if ( _modeStk[ modeSP ]->flags&Mode::UIFRAME )     
   {
      type += ((Control *)_modeStk[ modeSP ])->frame.Num();
   }

   o.setType( type );
   o.setPotVal( val );

   _modeStk[ modeSP ]->evHandler( o ); 
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Console::print
 *
 *  Desc:  Print a character.  
 *
 *  Args:  c                - character 
 *
 *  Memb:  output           - if true, console output is enabled
 *
 *----------------------------------------------------------------------------*/      

void Console::print( char c )
{
#ifdef CONSOLE_OUTPUT
   if ( output )
      Serial.print( c );
#endif
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Console::print
 *
 *  Desc:  Print a string.  
 *
 *  Args:  s                - string 
 *
 *  Memb:  output           - if true, console output is enabled
 *
 *----------------------------------------------------------------------------*/      

void Console::print( char *str )
{
#ifdef CONSOLE_OUTPUT
   if ( output )
      Serial.print( str );
#endif
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Console::print
 *
 *  Desc:  Print an integer.  
 *
 *  Args:  i                - integer 
 *
 *  Memb:  output           - if true, console output is enabled
 *
 *----------------------------------------------------------------------------*/      

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


