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

/* ----------------------     public instances    -------------------------- */

Console  console;
double   lastDouble;              // last gotten double


/* ----------------------     public functions    -------------------------- */

void console_setup( Config *c )
{
   Serial.begin( c->baudrate );

   console.rprint( c->title );
   console.print( " [" );
   console.print( c->version );
   console.print( "]\r\n" );

   console.init( c->matrix, &device_io );
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
   static char buf[16];
   toStr( val, &buf[0] );
#else
   static char buf[1];
#endif
   return &buf[0];
}

/* ----------------------     MacroBank class      -------------------------- */

char *MacroBank::prompt()
{
   return CONSTR("macro");
}

void MacroBank::onChoice()
{
   console.exe( (const char *)dataPtr() );
}

/* ----------------------      StrMode class       -------------------------- */

class InputMode : public Mode
{
   public:

   char *_prompt;                      // ptr to prompt str

   InputMode() { flags |= COLON; }

   char menu( key k )
   {
      return ( k.position() <= 9 ? '0' + k.position() : 0 );
   }

   char   *prompt()  { return _prompt; }

} ;

class DigitMode : public InputMode
{
   public:

   DigitMode() { flags &= ~ECHO; }

   char  result;                       // result of input (0 = no input)
   byte  max;                          // max allowable digit

   void charEv( char code )
   {
      if ( isDigit( code ) )
      {
         if ( code > '0' + max )
            code = '0' + max;
         console.print( code );
         result = code;
         console.popMode();
      }
      else
         InputMode::charEv( code );
   }

   void init( char *prompt, byte max )
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

   void butEv( but b )
   {
      if ( b.num() == 1 && ( b.type() & butPRESS+butTAP ) )
         charEv( chrCR );
      else 
         Mode::butEv( b );
   }

   void charEv( char code )
   {
      switch ( code )
      {
         case chrESC:

            buffer[0] = 0;             // fall thru to case chrCR for popMode

         case chrCR:

            console.popMode();
            break;

         default:

            if ( pos <= MaxStrLen )
            {
               buffer[ pos++ ] = code;
               buffer[ pos ]   = 0;
            }
      }
   }
  
   void init( char * x )
   {
      _prompt   = x;
      pos       = 0;
      buffer[0] = 0;
   }

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

   char   *result()  { return &buffer[0]; }

} strMode ;  

/* ----------------------      Console class       -------------------------- */

#define alphaCR         '\\'           // alphanumeric alternative to <CR>
#define alphaESC        '`'            // alphanumeric alternative to <ESC>

Mode nullMode;                         // default mode when none is specified

void Console::done()
{
#ifdef CONSOLE_OUTPUT
   newline();
   rprint( "Done." );
   Serial.flush();
#endif
}

void Console::exe( const char *m )
{
   atMacro = m;
}

char Console::getDigit( char *prompt, byte max )
{
   digitMode.init( prompt, max );
   runMode( &digitMode );
   return digitMode.result;
}

boolean Console::getDouble( char *prompt )
{
   boolean status = getStr( prompt );

   if ( status )
   {
      // lastDouble = (double ) atof( strMode.result() );

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

      lastDouble = (integral + fraction) * sign;
   }

   return status;
}

boolean Console::getByte( char* prompt, byte* val )
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

boolean Console::getInt( char *prompt, int *val )
{
   boolean status = getStr( prompt );

   if ( status )
      *val = (int ) atoi( strMode.result() );

   return status;
}

boolean Console::getStr( char *prompt ) 
{
   strMode.init( prompt );
   runMode( &strMode );
   return ( strMode.result()[0] != 0 );
}

void Console::infoByte( char* name, byte val )
{
   space();
   print( name );
   print( ": " );
   print( int(val) );
}

void Console::infoDouble( char* name, double val )
{
   space();
   print( name );
   print( ": " );
   print( toStr(val) );
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
         return;
      }
   }
   else 
   {
      if ( Serial.available() > 0 )
         chr = Serial.read();
      else
         return;
   }

   /* send character event to current Mode */

   if ( chr == alphaCR )
      chr = chrCR;
   else if ( chr == alphaESC )
      chr = chrESC;

   if ( (_modeStk[ modeSP ]->flags & Mode::ECHO) && chr != chrCR && chr != chrESC )  
      Serial.print( chr );

   _modeStk[ modeSP ]->charEv( chr );
   
}

void Console::newline()
{
#ifdef CONSOLE_OUTPUT
   Serial.print( "\r\n" );
#endif
}

void Console::newprompt()
{
   newline();
   prompt();
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
   _modeStk[ modeSP ]->onFocus( focusPOP ); 
   if ( modeSP > 0 ) 
   {
      newline();
      setMode( _modeStk[ --modeSP ] );
      _modeStk[ modeSP ]->onFocus( focusRESTORE ); 
   }
   else
      exit_sketch();
}

void Console::postBut( byte num, butEvType type )  
{
   but b( num, type );
   _modeStk[ modeSP ]->butEv( b ); 
}

void Console::postKey( byte pos, byte oct )  
{
   key k( pos, oct );

   if ( _modeStk[ modeSP ]->flags & Mode::MENU )
   {
      if ( _modeStk[ modeSP ]->flags & Mode::ONESHOT )
         _modeStk[ modeSP ]->flags &= ~( Mode::ONESHOT | Mode::MENU );

      char keychr = _modeStk[ modeSP ]->menu( k );
      if ( keychr )
      {
         if ( _modeStk[ modeSP ]->flags & Mode::ECHO )
            Serial.print( keychr );
         _modeStk[ modeSP ]->charEv( keychr );
      }
   }
   else
   {
      signed char stkptr = modeSP;
      while ( stkptr >= 0 && ! _modeStk[ stkptr ]->keyEv( k ) ) 
         --stkptr;
   }
}

void Console::print( char c )
{
#ifdef CONSOLE_OUTPUT
   static char buf[2];                 // buffer for null-terminated str
                                       // buf[1] zeroed by compiler :)
   buf[0] = c;
   Serial.print( buf );
#endif
}

void Console::print( char *str )
{
#ifdef CONSOLE_OUTPUT
   Serial.print( str );
#endif
}

void Console::print( int i )
{
#ifdef CONSOLE_OUTPUT
   Serial.print( i, DEC );
#endif
}

void Console::prompt()
{
#ifdef CONSOLE_OUTPUT
   char * p = _modeStk[ modeSP ]->prompt();
   space( SEAM - strlen( p ) - 2 );
   Serial.print( p );
   print( _modeStk[ modeSP ]->flags & Mode::COLON ? ':' : '>' );
   space(1);
#endif
}

void Console::pushMode( Mode *x )
{
   if ( modeSP < MAX_MODE-1 ) 
   {
      newline();
      ++modeSP;
      setMode( x );
      x->onFocus( focusPUSH );
   }
}

char *Console::result()
{  
   return strMode.result();
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

void Console::rprint( char *str )
{
#ifdef CONSOLE_OUTPUT
   rtab();
   Serial.print( str );
#endif
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
   prompt();
}

void Console::space()
{
   space(1);
}

void Console::space( byte repeatCount )
{
#ifdef CONSOLE_OUTPUT
   for ( byte i = 0; i < repeatCount; i++ )
      Serial.print(' ');
#endif
}


