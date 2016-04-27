/*
    Config.h  

    Declaration of the Config struct.

    ---------------------------------------------------------------------------
 
    Copyright (C) 2016, Cornfield Electronics, Inc.
 
    This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
    Unported License.
 
    To view a copy of this license, visit
    http://creativecommons.org/licenses/by-sa/3.0/
 
    Created by Bill Alessi & Mitch Altman.
 
   ---------------------------------------------------------------------------
*/

class Mode;

#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

typedef struct                         // program configuration record 
{
   char          *title;               // program name
   char          *version;             // program version
   Mode          *matrix;              // matrix object  
   double        dynaRate;             // dynamic update rate 
   unsigned long baudrate;             // serial baud rate

} Config;


#endif   // ifndef CONFIG_H_INCLUDED
