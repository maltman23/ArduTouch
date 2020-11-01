/*
    Console_.h  

    Declarations for the Console module.

    PLEASE NOTE! The added underscore in the filename is a workaround.
                 If this file is named Console.h, the program will compile 
                 and run just fine -- except that it will inexplicably
                 use 2438 extra bytes of ROM and 84 extra bytes of RAM.

    ---------------------------------------------------------------------------
 
    Copyright (C) 2016, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
   ---------------------------------------------------------------------------
*/

#ifndef CONSOLE_H_INCLUDED
#define CONSOLE_H_INCLUDED

#include "Model.h"

/*
                    IMPORTANT NOTE REGARDING STRING CONSTANTS

   The Arduino environment provides a macro, PSTR(), which places a string
   constant in ROM. The ArduTouch Library extends this feature by providing 2 
   additional macros, CONSTR() and ALLOC_CONSTR(), which are used for placing
   string constants intended only for display to the Ardutouch Console in ROM. 
   
   If a string constant is meant to be executed as an ArduTouch macro (i.e.,
   as an argument to console.exe()), or in any capacity other than being
   displayed to the Console, then YOU MUST use PSTR() -- because CONSTR() and 
   ALLOC_CONSTR() replace their string arguments with the NULL string unless
   the Runtime Model is __FULLHOST__. 

                    -----------------------------------------

   The following 2 macros -- CONSTR(x) and ALLOC_CONSTR(p, x) -- allow for the 
   semi-transparent placement of string contants in ROM (circumventing Arduino's 
   run-time behavior of copying constants into RAM).

   Whenever a string constant is expected as an argument to a routine, the string
   literal should be encapsulated by the CONSTR() macro. For example, given the 
   2 routine prototypes:

      void iExpectAVariableString( char * )
      void iExpectAConstantString( const char * )

   and the string 
   
      "here you go"

   they should be called as follows:

      iExpectAVariableString( "here you go" );
      iExpectAConstantString( CONSTR( "here you go" ) );

   iExpectAConstantString() must read the string from ROM. This may not be an 
   option if it is a pre-compiled routine in a 3rd-party library, in which
   case use a standard Arduino char constant (which ends up in RAM).

   The CONSTR(x) macro cannot be used outside of a function, or within a list. 
   For this, use the ALLOC_CONSTR(p, x) to define a ptr to the constant string x,
   then refer to the string via its ptr in the list.

   For example given the struct:

      typedef struct { const char *a; const char *b; } twoConstantStrs;

   use

      ALLOC_CONSTR( str1, "1st string" );
      ALLOC_CONSTR( str2, "2nd string" );
      twoConstantStrs myInstance = { str1, str2 };
*/

#ifdef CONSOLE_OUTPUT

   #define CONSTR(x) PSTR(x)
   #define ALLOC_CONSTR(p, x) const char p [] PROGMEM = x

   #define PROMPT_STR( s ) const char *prompt() { return CONSTR( #s ); } 

   #define about_program(x, y)                        \
                                                      \
      extern const char __PROGNAME__[];               \
      extern const char __PROGVER__[];                \
      const char __PROGNAME__[] PROGMEM = #x;         \
      const char __PROGVER__[]  PROGMEM = #y;         

#else

   #define CONSTR(x) PSTR("")
   #define ALLOC_CONSTR(p, x) const char p [] PROGMEM = ""

   #define PROMPT_STR( s )

   #define about_program(x, y)

#endif

// As of version 1.08 the baud rate is specified in Model.h of the library
// and not in the sketch via the (now deprecated) set_baud_rate macro

   #define set_baud_rate(x)


#include "Arduino.h"
#include "Mode.h"
#include "Commands.h"

/******************************************************************************
 *
 *                                  Console 
 *
 ******************************************************************************/

class Console
{
   public:

   static const int MAX_MODE = 8;      // max nesting level of console modes

   private:

   Mode   *_modeStk[ MAX_MODE+1 ];     // stack of ptrs to nested console modes
   signed char modeSP;                 // stackptr for _modeStk[]

   const char *atMacro;                // ptr into macro string being executed

   boolean  output;                    // if true, console output is enabled
   boolean  oneShot;                   // if true, interpret next key-down event 
                                       // as a menu selection
   boolean  menuKeyDn;                 // if true, last key down was a menu selection

   boolean getStr( const char* );      // prompt for and get a String

   void    stackEv( obEvent ev );      // post an event to each mode in stack until handled

   public:

   static const int SEAM     = 10;     // width of console seam
   char    delim;                      // character used as prompt delimiter

   Console()
   {
      delim = '>';
   }

   void  (*idle)() = NULL;             // ptr to routine to run while waiting 
                                       // for line input to complete

   void  disable();                    // disable console I/O
   void  dispBits( byte, byte );       // print right-justified bitfld within a byte 
   void  done();
   void  enable();                     // enable console I/O
   void  exe( const char* );           // execute a macro str 
   void  exeIn( const char*, Mode* );  // execute a macro str within a specified mode
   bool  getBits( const char *, byte, byte* );// prompt for and get a bitfield
   bool  getBool( const char*, bool* );       // prompt for and get a boolean
   bool  getByte( const char*, byte* );       // prompt for and get a byte (0:255)
   bool  getChar( const char*, char* );       // prompt for and get a character
   char  getDigit( const char*, byte );       // get '0' thru '9', (0 = aborted)
   bool  getDouble( const char*, double* );   // prompt for and get a double
   bool  getInt( const char*, int* );         // prompt for and get an int
   bool  getSByte( const char*, char* );      // prompt for and get byte (-128:127)
   bool  getStr( const char*, char** );       // prompt for and get a String 
   bool  getULong( const char*, unsigned long* ); // prompt for on unsigned long
   void  infoBits( const char *, byte, byte );// print a bitfield value (with label)
   void  infoBool( const char*, bool );       // print a boolean value (with label)
   void  infoByte( const char*, byte );       // print a byte value (with label)
   void  infoChar( const char*, char );       // print a character (with label)
   void  infoCharBits( const char*, byte );   // print a char as a bit string (with label)
   void  infoDouble( const char*, double );   // print a double value (with label)
   void  infoInt( const char*, int );         // print an int value (with label)
   void  infoIntBits( const char*, int );     // print an int as a bit string (with label)
   void  infoStr( const char*, const char* ); // print a string located in ROM (with label)
   void  infoRAMStr( const char*, char* );    // print a string located in RAM (with label)
   void  infoULong( const char*, unsigned long ); // print an unsigned long (with label)
   void  init( Mode *mode, void (*)() );
   bool  inMode( Mode* );              // returns true if arg is current mode
   void  input();
   void  newline();                    // go to beginning of next line
   void  newlntab();                   // go to beginning of next line and rtab
   void  newprompt();                  // display a new prompt for current mode
   void  oneShotMenu();                // interpret next key as a menu selection
   void  ongoing();                    // continue ongoing tasks for one cycle
   void  popMode();                    // pop the top mode on the mode stack
   void  postBut( byte, butAction );   // post a button event  
   void  postKeyDn( byte, byte );      // post a key-down event  
   void  postKeyUp( byte, byte );      // post a key-up event  
   void  postPot( byte, byte );        // post a pot event  
   void  print( char );                // print a character
   void  print( char * );              // print a string
   void  print( int );                 // print an int
   void  pushMode( Mode * );           // push a mode onto the mode stack
   void  romprint( const char * );     // print a string located in ROM
   byte  romstrlen( const char * );    // returns length of str in ROM
   void  rtab();                       // move to right of seam
   void  runMode( Mode * );            // run a mode until it pops itself
   void  runModeWhile( Mode*, boolean* ); // run a mode until a condition is met
   void  setIdle( void (*)() );        // specify routine to run when idling
   void  setMode( Mode * );            // set the current (top) mode
   void  space();                      // print a space
   void  space( byte );                // print multiple spaces

   private:

   void  begInfo( const char* );       // print beginning of info cell 
   void  endInfo();                    // print end of info cell 

} ;

// character codes for selected nonalphanumeric characters:

#define chrCR           13             // <CR>
#define chrESC          27             // <ESC>

/* ------------------------      public vars      -------------------------- */

extern Console console;                // Console object

/* ----------------------     public functions     ------------------------- */

#define isDigit(x) ( x >= '0' && x <= '9' )

extern void  console_setup( Mode * );
extern void  exit_sketch();
extern void  toStr( double val, char *str );
extern char *toStr( double val );

#endif   // ifndef CONSOLE_H_INCLUDED