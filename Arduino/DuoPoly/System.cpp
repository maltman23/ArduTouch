/*
    System.cpp  

    System services.

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
#include "Phonic.h"
#include "ConKeyBrd.h"
#include "pins_arduino.h"              // needed by Arduino pre-1.0 

/* ----------------------     public functions    -------------------------- */

void    device_io();

/* ----------------------    private functions    -------------------------- */


void     audio_setup( Config *c ) __attribute__((always_inline));
boolean  readKey( byte keyNum );
boolean  render_audio() __attribute__((always_inline));
void     scanBut( byte ) __attribute__((always_inline)); 
void     scanKeys(); 

/* ----------------------    private variables    -------------------------- */


word    bufsPerScan;                   // # buffers to render per I/O scan
word    scanDC;                        // downcounter to next I/O scan

#define NumButs        2               // number of onboard buttons 

byte        butPin[] = { 17, 16 };     // associated digital pin # per button
signed char butCount[ NumButs ];       // # scans in present state per button
                                       // (- means up, + means down)
boolean     tapPending[ NumButs ];     // if true, potential tap in progress 

volatile signed char curKey = -1;      // currently pressed key (-1 means none)

/*----------------------------------------------------------------------------*
 *
 *  Name:  system_setup
 *
 *  Desc:  Initialize system resources.
 *
 *  Args:  config           - ptr to program configuration record  
 *
 *  Glob: +bufsPerScan      - # buffers to render per I/O scan
 *        +scanDC           - downcounter (in bufs) to next I/O scan
 *
 *  Note:  This routine should be called from the sketch's setup() routine.
 *
 *----------------------------------------------------------------------------*/      

void system_setup( Config *c )
{
   const double scanRate = 30.0;

   console_setup( c );                 // initialize console module
   audio_setup( c );                   // initialize audio module

   bufsPerScan = audioRate / (scanRate * audio::bufSz);
   scanDC      = bufsPerScan;

   for ( byte i = 0; i < NumButs; i++ )
      pinMode( butPin[i], INPUT );     // set corresponding digital pin as input
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  system_loop
 *
 *  Desc:  Perform ongoing system tasks.
 *
 *  Note:  This routine must be called from the sketch's loop() routine.
 *
 *----------------------------------------------------------------------------*/      

void system_loop()
{
   console.input();
   device_io();
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  device_io
 *
 *  Desc:  Manage device input, posting appropriate events as they happen,
 *         and render audio output.
 *
 *  Glob:  bufsPerScan      - # buffers to render per I/O scan
 *         curKey           - currently pressed key (-1 means none)
 *        +scanDC           - downcounter (in bufs) to next I/O scan
 *
 *----------------------------------------------------------------------------*/      

void device_io()
{
   if ( render_audio() && --scanDC == 0 )
   {
      scanDC = bufsPerScan;               // reload downcounter to next scan

      /* scan keys */

      if ( curKey >= 0 )
      {
         if ( ! readKey( curKey ) )
            scanKeys();
      }
      else
          scanKeys();

      /* scan buttons */

      for ( byte i = 0; i < NumButs; i++ )
         scanBut(i);
   }
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  scanBut
 *
 *  Desc:  Perform a hardware scan of a button, and post a corresponding event 
 *         if a change in its ongoing state warrants it.
 *
 *  Args:  butNum           - button number to read
 *
 *  Glob:  butPin[]         - associated digital pin # 
 *         butCount[]       - # scans in present state (- is up, + is down)
 *         tapPending[]     - if true, potential tap in progress
 *
 *----------------------------------------------------------------------------*/      

void scanBut( byte num ) 
{
   const signed char maxAbsButCount = 125;  // max # scans reflected in butCount  

   boolean butDown = ( digitalRead( butPin[num] ) == LOW );

   if ( butDown )
   {
      if ( butCount[num] < 0 )
         butCount[num] = 0;

      if ( ++butCount[num] > maxAbsButCount )
         butCount[num] = maxAbsButCount;
   }
   else  // button is up
   {
      if ( butCount[num] > 0 )
      {
         if ( tapPending[num] )
         {
            console.postBut( num, butDOUBLE_TAP );
            tapPending[num] = false;
            butCount[num]   = 0;
         }
         else
         {
            if ( butCount[num] >= 7 )
            {
               console.postBut( num, butPRESS );
               butCount[num] = 0;
            }
            else
            {
               tapPending[num] = true;
               butCount[num] = -1;
            }
         }
      }
      else // if ( butCount[num] <= 0 )
      {
         if ( --butCount[num] < -maxAbsButCount )
            butCount[num] = -maxAbsButCount;

         if ( tapPending[num] && butCount[num] <= -7 )
         {
            console.postBut( num, butTAP );
            tapPending[num] = false;
            butCount[num]   = 0;
         }
      }
   }
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  scanKeys
 *
 *  Desc:  Perform a hardware scan of the keys. Post a key event for the lowest
 *         key pressed, if any, and return that key #. 
 *
 *  Args:  -- none --
 *
 *  Glob:  +curKey          - currently pressed key (-1 means none)
 *
 *----------------------------------------------------------------------------*/      

void scanKeys() 
{
   const byte maxKey = 11;

   for ( byte i = 0; i <= maxKey; i++ )
      if ( readKey(i) )
      {
         curKey = i;
         console.postKey( i, keybrd.getOctave() );
         return;
      }
   curKey = -1;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  readKey
 *
 *  Desc:  Measure the capacitance of the pin connected to a key, and return
 *         a boolean indicating whether the key should be considered pressed.
 *
 *  Args:  keyNum           - key position in octave  
 *
 *  Rets:  status           - if true, key is considered pressed.
 *
 *  Glob: +bufsPerScan      - # buffers to render per I/O scan
 *        +scanDC           - downcounter (in bufs) to next I/O scan
 *
 *  Note:  This routine is a modified version of readCapacitivePin() by the 
 *         Capacitive Synth project, Other Machine Co., http://othermachine.co
 *
 *----------------------------------------------------------------------------*/      

const byte keyPins[] PROGMEM = { 13, 12, 10, 8, 7, 6, 5, 4, 3, 2, 19, 18, 17 };

boolean readKey( byte keyNum ) 
{
   int pinToMeasure = (int )pgm_read_byte_near( keyPins + keyNum );

   // Variables used to translate from Arduino to AVR pin naming

   volatile uint8_t* port;
   volatile uint8_t* ddr;
   volatile uint8_t* pin;

   // Here we translate the input pin number from
   //  Arduino pin number to the AVR PORT, PIN, DDR,
   //  and which bit of those registers we care about.

   byte bitmask;

   port    = portOutputRegister(digitalPinToPort(pinToMeasure));
   ddr     = portModeRegister(digitalPinToPort(pinToMeasure));
   bitmask = digitalPinToBitMask(pinToMeasure);
   pin     = portInputRegister(digitalPinToPort(pinToMeasure));

   // Discharge the pin first by setting it low and output

   *port &= ~(bitmask);
   *ddr  |= bitmask;

   asm("nop");           // delay(1);

   // Prevent the timer IRQ from disturbing our measurement

   noInterrupts();

   // Make the pin an input with the internal pull-up on

   *ddr &= ~(bitmask);
   *port |= bitmask;

   // Now see how long the pin to get pulled up. This manual unrolling of the loop
   // decreases the number of hardware cycles between each read of the pin,
   // thus increasing sensitivity.

   uint8_t cycles = 17;

   if      (*pin & bitmask) { cycles =  0;}
   else if (*pin & bitmask) { cycles =  1;}
   else if (*pin & bitmask) { cycles =  2;}
   else if (*pin & bitmask) { cycles =  3;}
   else if (*pin & bitmask) { cycles =  4;}
   else if (*pin & bitmask) { cycles =  5;}
   else if (*pin & bitmask) { cycles =  6;}
   else if (*pin & bitmask) { cycles =  7;}
   else if (*pin & bitmask) { cycles =  8;}
   else if (*pin & bitmask) { cycles =  9;}
   else if (*pin & bitmask) { cycles = 10;}
   else if (*pin & bitmask) { cycles = 11;}
   else if (*pin & bitmask) { cycles = 12;}
   else if (*pin & bitmask) { cycles = 13;}
   else if (*pin & bitmask) { cycles = 14;}
   else if (*pin & bitmask) { cycles = 15;}
   else if (*pin & bitmask) { cycles = 16;}

   // End of timing-critical section

   interrupts();

   // Discharge the pin again by setting it low and output
   //  It's important to leave the pins low if you want to 
   //  be able to touch more than 1 sensor at a time - if
   //  the sensor is left pulled high, when you touch
   //  two sensors, your body will transfer the charge between
   //  sensors.

   *port &= ~(bitmask);
   *ddr  |= bitmask;

   return ( cycles > 3 );
}


/* ----------------------   Audio Output Buffer    ------------------------- */

#define BUFSZ    10                    // # of records per buffer
#define NUMBUFS  4                     // # of buffers
#define NUMRECS  NUMBUFS*BUFSZ         // total # of buffered records 

byte audioL[NUMRECS];                  // records for left audio channel 
byte audioR[NUMRECS];                  // records for right audio channel

volatile boolean lock[NUMBUFS];        // write-lock status per buffer

volatile byte w;                       // idx of record being written to
volatile byte r;                       // idx of record being read from
volatile byte eor;                     // idx to end of current read buffer 

volatile byte wBuf;                    // buffer # being written to
volatile byte rBuf;                    // buffer # being read from

boolean  paused;                       // if true, pause audio output
byte     pauseCnt;                     // fill at least this many buffers 
                                       // with "silence" before resuming 
byte     lastL;                        // last written audioL value
byte     lastR;                        // last written audioR value

namespace audio
{
const byte bufSz = BUFSZ;              // # of records per buffer
}

/* -------------------------------------------------------------------------- */

   // double refclk      = 31372.549;  // ideal ISR rate = 16MHz / 510
const double refclk      = 31376.6;    // real ISR rate as measured

const double audioRate   = refclk*.5;  // PWM regs updated every other tick
const word   ticksPerSec = audioRate;  // for use with integer down counters

double  dynaRate;                      // dynamic update rate
word    bufsPerDyna;                   // # buffers to render per dynamic update
word    dynaDC;                        // downcounter to next dynamic update

StereoPhonic *matrix;                  // ptr to object that generates the audio

/*----------------------------------------------------------------------------*
 *
 *  Name:  audio::setup
 *
 *  Desc:  Initialize the audio module.
 *
 *  ARGS:  config           - ptr to program configuration record  
 *
 *  GLOB: +bufsPerDyna      - # buffers written between dynamic updates
 *        +dynaDC           - downcounter (in bufs) to next dynamic update
 *        +dynaRate         - rate at which to call matrix->dynamics()
 *        +eor              - idx to end of current read buffer 
 *        +lock[]           - write-lock status per buffer
 *        +matrix           - ptr to object which generates the audio
 *        +r                - idx of record being read from
 *        +rBuf             - buffer # being read from
 *        +w                - idx of record being written to
 *        +wBuf             - buffer # being written to
 *
 *----------------------------------------------------------------------------*/      

void audio_setup( Config *c )
{
   const double bufRate = audioRate / BUFSZ;

   /* initialize soft parameters */

   matrix       = (StereoPhonic *)(c->matrix);
   bufsPerDyna  = bufRate / c->dynaRate;
   dynaRate     = bufRate / bufsPerDyna;
   dynaDC       = bufsPerDyna;

   /* initialize audio output buffers */

   wBuf = 0;
   w    = 0;

   rBuf = NUMBUFS-1;
   r    = rBuf * BUFSZ;
   eor  = r + BUFSZ;

   for ( byte i = 0; i < NUMBUFS; i++ )
      lock[i] = false;

   lock[rBuf] = true;

   /* initialize pins */

   pinMode(9, OUTPUT);                 // pin9  = PWM output for Timer1
   pinMode(11, OUTPUT);                // pin11 = PWM output for Timer2 

   /* initialize Timer 1 */

   sbi (TCCR1B, CS20);                 // Clock Prescaler set to : 1
   cbi (TCCR1B, CS21);
   cbi (TCCR1B, CS22);
   cbi (TCCR1A, COM2A0);               // clear Compare Match
   sbi (TCCR1A, COM2A1);
   sbi (TCCR1A, WGM20);                // Mode 1 - Phase Correct PWM
   cbi (TCCR1A, WGM21);
   cbi (TCCR1B, WGM22);

   /* initialize Timer 2 */
   
   sbi (TCCR2B, CS20);                 // Clock Prescaler set to : 1
   cbi (TCCR2B, CS21);
   cbi (TCCR2B, CS22);
   cbi (TCCR2A, COM2A0);               // clear Compare Match
   sbi (TCCR2A, COM2A1);
   sbi (TCCR2A, WGM20);                // Mode 1 - Phase Correct PWM
   cbi (TCCR2A, WGM21);
   cbi (TCCR2B, WGM22);

   /* configure Timer interrupts */

   cbi (TIMSK0,TOIE0);                 // disable Timer0 
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  render_audio
 *
 *  Desc:  Render the next free audio buffer, if available.
 *
 *         When audio has been paused, the buffer is filled with "silence."
 *         Otherwise, the buffer is filled via the matrix object's generate()
 *         method (the matrix object's dynamics are also managed).
 *
 *  Rets:  status           - if true, a buffer was rendered
 *
 *  Glob: +bufsPerDyna      - # buffers to render between dynamic updates
 *        +dynaDC           - downcounter to next dynamic update
 *        +lastL            - last written audioL value
 *        +lastR            - last written audioR value
 *        +lock[]           - write-lock status per buffer
 *         matrix           - ptr to object which generates the audio
 *        +pauseCnt         - fill at least this many buffers with "silence" 
 *                            before resuming 
 *         paused           - if true, audio output is paused
 *        +w                - idx of record being written to
 *        +wBuf             - buffer # being written to
 *
 *----------------------------------------------------------------------------*/      

boolean render_audio()
{
   if ( lock[wBuf] )                   // return if buffer is locked
      return false;

   if ( pauseCnt )                     // was/is paused, so fill buffer with 
   {                                   // last written values to avoid "glitch"
      byte idx  = w;
      byte icnt = BUFSZ;
      while ( icnt-- )
      {
         audioL[idx] = lastL;
         audioR[idx] = lastR;
         ++idx;
      }
      if ( (! --pauseCnt) && paused )  // if pauseCnt has counted down to 0,
         pauseCnt = 1;                 // but audio is still paused, bump it
   }   
   else                                // fill buffer via matrix object
   {
      matrix->generate( (char *)&audioL[w], (char *)&audioR[w] ); 

      if ( --dynaDC == 0 )             // manage dynamics 
      {
         matrix->dynamics();           // perform a dynamic update
         dynaDC = bufsPerDyna;         // reload downcounter
      }
   }

   lock[wBuf] = true;                  // lock buffer from further writes

   lastL = audioL[w + BUFSZ-1];        // update last values
   lastR = audioR[w + BUFSZ-1];

   if ( ++wBuf == NUMBUFS )            // bump wBuf (circular)
   {
      wBuf = 0;
         w = 0;                 
   }
   else
   {
      w += BUFSZ;
   }

   return true;
}

void audio::disable()
{
   cbi (TIMSK1,TOIE1);                 // disable Timer1 Interrupt
}

void audio::enable()
{
   sbi (TIMSK1,TOIE1);                 // enable Timer1 Interrupt
}

void audio::pause()
{
   paused = true;
   pauseCnt = 255;                     // mitigates "glitch" on quick resume
}

void audio::resume()
{
   paused = false;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  ISR(TIMER1_OVF_vect)
 *
 *  Desc:  Timer 1 Interrupt Service Routine
 *         Load buffered audio to PW registers
 *
 *  GLOB: audioL[]          - buffered records for left audio channel 
 *        audioR[]          - buffered records for right audio channel 
 *        +eor              - idx to end of current read buffer 
 *        +lock[]           - write-lock status per buffer
 *        +r                - idx of record being read from
 *        +rBuf             - buffer # being read from
 *
 *  Note: A halving of the effective audio rate is acheived by executing the 
 *        ISR body every other interrupt.
 *
 *----------------------------------------------------------------------------*/      

ISR(TIMER1_OVF_vect) 
{
   static byte toggle;

   if ( (toggle ^= 1 ) == 0 )       // execute ISR body every other interrupt
      return;
         
   OCR1A = audioL[r] + 127;         // normalize and load buffered audio 
   OCR2A = audioR[r] + 127;         // to PWM registers

   if ( ++r == eor )                // bump r, check for end of buffer
   {
      lock[rBuf] = false;           // unlock this buffer for writing
      if ( ++rBuf == NUMBUFS )      // bump rBuf (circular)
      {
         rBuf = 0;
            r = 0;
      }
      eor = r + BUFSZ;              // adjust eor accordingly
   }
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  ISR(TIMER2_OVF_vect)
 *
 *  Desc:  Timer 2 Interrupt Service Routine
 *
 *  GLOB:  -- none --
 *
 *  Note:  This vector must be defined, or program hangs.
 *
 *----------------------------------------------------------------------------*/      

ISR(TIMER2_OVF_vect) 
{
}

