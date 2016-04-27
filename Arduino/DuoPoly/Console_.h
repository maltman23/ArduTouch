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

#define CONSOLE_OUTPUT                 // comment this out for no console output

#ifdef CONSOLE_OUTPUT
   #define CONSTR(x) x
#else
   #define CONSTR(x) ""
#endif

#include "Arduino.h"
#include "Config.h"
#include "Mode.h"
#include "Bank.h"

class MacroBank : public Bank
{
   public:

   char  *prompt();                    // return object's prompt string
   void   onChoice();                  // execute this when member is chosen
   void   select( char ith );          // select a member

} ;

class Console
{
   static const int MAX_MODE = 8;      // max nesting level of console modes
   static const int SEAM     = 10;     // width of console seam

   Mode   *_modeStk[ MAX_MODE+1 ];     // obj ptrs of nested console modes
   signed char modeSP;                 // stackptr for _modeStk[]

   const char *atMacro;                // ptr into macro string being executed

   public:

   void  (*idle)() = NULL;             // ptr to routine to run while waiting 
                                       // for line input to complete

   void    done();
   void    exe( const char * );        // execute a macro str located in ROM
   boolean getByte( char*, byte* );    // prompt for and get a byte (0-255)
   char    getDigit( char*, byte );    // get '0' thru '9', (0 = aborted )
   boolean getDouble( char *prompt );  // prompt for and get a double
   boolean getInt( char *prompt, int *val ); // prompt for and get an int
   boolean getStr( char *prompt );     // prompt for and get a String
   void    infoByte( char*, byte );    // print " name: value" 
   void    infoDouble( char*, double );// print " name: value" 
   void    init( Mode *mode, void (*)() );
   void    input();
   void    newline();
   void    newprompt();
   boolean ongoing( Mode * );
   void    popMode();                  // pop the top mode on the mode stack
   void    postBut( byte, butEvType ); // post a button event  
   void    postKey( byte, byte );      // post a key event  
   void    print( char );              // print a character
   void    print( char * );            // print a string
   void    print( int );               // print an int
   void    prompt();
   void    pushMode( Mode * );         // push a mode onto the mode stack
   char   *result();                   // result of last call to getStr()
   void    romprint( const char * );   // print a string located in ROM
   void    rprint( char * );           // print to right of seam
   void    rtab();                     // move to right of seam
   void    runMode( Mode * );
   void    setIdle( void (*)() );      // specify routine to run when idling
   void    setMode( Mode * );
   void    space();                    // print a space
   void    space( byte );              // print multiple spaces


} ;

/* character codes for selected nonalphanumeric characters */

#define chrCR           13             // <CR>
#define chrESC          27             // <ESC>

/* ------------------------      public vars      -------------------------- */

extern Console console;                // Console object
extern double  lastDouble;             // last gotten double

/* ----------------------     public functions     ------------------------- */

#define isDigit(x) ( x >= '0' && x <= '9' )

extern void  console_setup( Config* );
extern void  exit_sketch();
extern void  toStr( double val, char *str );
extern char *toStr( double val );

#endif   // ifndef CONSOLE_H_INCLUDED