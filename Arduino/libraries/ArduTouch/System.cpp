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
#include "Synth.h"
#include "System.h"
#include "Tuning.h"
#include "pins_arduino.h"              // needed by Arduino pre-1.0 

/* ------------------------------------------------------------------------
       The following lines insure that one and only one PCB REV is declared
   ------------------------------------------------------------------------ */

#ifndef PCB_REV_A
   #ifndef PCB_REV_B
      #ifndef PCB_REV_C
            #error No PCB REV has been declared (System.h)
      #endif
   #endif
#endif

#ifdef PCB_REV_A
   #ifdef PCB_REV_B
      #error Multiple PCB REVs have been declared (System.h)
   #endif
   #ifdef PCB_REV_C
      #error Multiple PCB REVs have been declared (System.h)
   #endif
#endif

#ifdef PCB_REV_B
   #ifdef PCB_REV_C
      #error Multiple PCB REVs have been declared (System.h)
   #endif
#endif

/* ------------------------------------------------------------------------- */

#ifdef PCB_REV_C
   #define CLOCKWISE_POTS              // pot values increase going clockwise 
   #define LED_ACTIVE_LOW              // LEDs are active on LOW edge
   const byte numKeys = 12;            // # of accessible onboard keys
#endif

#ifdef PCB_REV_B
   #define LED_ACTIVE_LOW              // LEDs are active on LOW edge
   const byte numKeys = 12;            // # of accessible onboard keys
#endif

#ifdef PCB_REV_A
   const byte numKeys = 12;            // # of accessible onboard keys
#endif

#ifdef LED_ACTIVE_LOW            
   #define LED_ON  LOW
   #define LED_OFF HIGH
#else
   #define LED_ON  HIGH
   #define LED_OFF LOW
#endif

/* ----------------------    private functions    -------------------------- */


void     audio_setup( Synth * ) __attribute__((always_inline));
boolean  readKey( byte keyNum );
void     readPot( byte num );
boolean  render_audio();
void     scanBut( byte ) __attribute__((always_inline)); 
void     scanKeys(); 

/* ----------------------     public variables    -------------------------- */

#ifdef MONITOR_CPU

byte     cpu;                          // %CPU used rendering audio (parts per 256) 

#endif

/* ----------------------    private variables    -------------------------- */

#ifdef MONITOR_CPU

byte    cpuCount;                      // counter for sampling CPU usage
byte    runCpu;                        // running CPU usage (parts per cpuCount)
boolean usingCpu;                      // currently using CPU to render audio 

#endif

#define NUMBUFS  3                     // # of audio buffers

const   double scanRate = 20.0;        // rate at which to scan hardware
byte    bufsPerScan;                   // # buffers to render per I/O scan
byte    scanDC;                        // downcounter to next I/O scan
boolean enabled;                       // audio is enabled

#define NumButs        2               // number of onboard buttons 
#define NumPots        2               // number of onboard pots 
#define NumLEDs        2               // number of onboard LEDs 

byte        butPin[] = { 17, 16 };     // associated digital pin # per button
signed char butCount[ NumButs ];       // # scans in present state per button
                                       // (- means up, + means down)
boolean     tapPending[ NumButs ];     // if true, potential tap in progress 

byte        potPin[] = { 1, 0 };       // associated pin # per pot
word        potVal[ NumPots ];         // last value read per pot
byte        nextPot;                   // next pot # to scan

#ifndef USE_SERIAL_PORT

byte        LEDPin[] = { 1, 0 };       // pin # per LED (left to right)
boolean     blinking[ NumLEDs ];       // true if LED is blinking
byte        blinkRate = 80;            // blink rate (in dynamic updates)
byte        blinkDC = 80;              // down-counter to next blink transition
boolean     blinkEdge;                 // if true turn blinkers on at transition

#endif

volatile signed char curKey = -1;      // currently pressed key (-1 means none)
byte curKeyOct;                        // octave of currently pressed key

byte  octave;                          // current octave # for keyboard

/*----------------------------------------------------------------------------*
 *
 *  Name:  ardutouch_setup
 *
 *  Desc:  Initialize ardutouch resources.
 *         Invoke synthesizer's setup() method.
 *
 *  Args:  syn              - ptr to synthesizer object 
 *
 *  Glob:  audioBufSz       - size of an audio buffer
 *        +bufsPerScan      - # buffers to render per I/O scan
 *         butPin[]         - associated pin # per button (left to right) 
 *         LEDPin[]         - associated pin # per LED (left to right) 
 *         masterTuning     - ptr to master tuning object
 *        +octave           - current octave # for keyboard
 *         potPin[]         - associated pin # per pot (top to bot)
 *        +potVal[]         - last value read per pot
 *        +scanDC           - downcounter (in bufs) to next I/O scan
 *         scanRate         - rate at which to scan hardware
 *
 *  Note:  This routine should be called from the sketch's setup() routine.
 *
 *----------------------------------------------------------------------------*/      

void ardutouch_setup( Synth *syn )
{
   console_setup( (Mode *) syn );      // initialize console module
   audio_setup( syn );                 // initialize audio module

   bufsPerScan = audioRate / (scanRate * audioBufSz);
   scanDC      = bufsPerScan;

   /* initialize buttons */

   for ( byte i = 0; i < NumButs; i++ )
      pinMode( butPin[i], INPUT );     // set corresponding digital pin as input

   /* get initial positions of pots */

   for ( byte i = 0; i < NumPots; i++ )
      #ifdef CLOCKWISE_POTS 
         potVal[i] = analogRead( potPin[i] );
      #else
         potVal[i] = 1023 - analogRead( potPin[i] );
      #endif

   #ifndef USE_SERIAL_PORT

      /* initialize LEDs */
                     
      for ( byte i = 0; i < NumLEDs; i++ )
      {
         pinMode( LEDPin[i],  OUTPUT );
         digitalWrite( LEDPin[i], LED_OFF );
      }

   #endif

   syn->setup();                       // setup the synth
   octave = masterTuning->defOctave;   // set initial octave for keyboard
   audio::enable();                    // enable audio output
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  ardutouch_loop
 *
 *  Desc:  Perform ongoing ardutouch tasks.
 *
 *  Note:  This routine must be called from the sketch's loop() routine.
 *
 *----------------------------------------------------------------------------*/      

void ardutouch_loop()
{
   console.input();
   device_io();
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  ardutouch_info()
 *
 *  Desc:  Display information about the ArduTouch library to the console.
 *
 *  Glob:  audioBufSz       - size of an audio buffer
 *         audioRate        - audio playback rate 
 *         bufsPerDyna      - # audio buffers rendered per dynamic update
 *         bufsPerScan      - # audio buffers rendered per I/O scan
 *         dynaRate         - dynamic udpate rate
 *         scanRate         - rate at which to scan hardware
 *
 *----------------------------------------------------------------------------*/      

void ardutouch_info()
{
   console.newlntab();
   console.infoStr( CONSTR( "Version" ), CONSTR( LIBRARY_VERSION ) );
   console.newlntab();
   console.infoDouble( CONSTR("audioRate"), audioRate );
   console.infoDouble( CONSTR("dynaRate"), dynaRate );
   console.newlntab();
   console.infoByte( CONSTR("bufSz"), audioBufSz );
   console.infoByte( CONSTR("numBufs"), NUMBUFS );
   console.infoByte( CONSTR("bufsPerDyna"), bufsPerDyna );
   console.newlntab();
   console.infoDouble( CONSTR("scanRate"), scanRate );
   console.infoInt( CONSTR("bufsPerScan"), bufsPerScan );
   console.newprompt();
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
 *         curKeyOct        - octave of currently pressed key
 *        +nextPot          - next pot to read
 *        +scanDC           - downcounter (in bufs) to next I/O scan
 *
 *  Note:  One onboard pot is read per scan. 
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
         {
            console.postKeyUp( curKey, curKeyOct );
            scanKeys();
         }
      }
      else
          scanKeys();

      /* scan buttons */

      for ( byte i = 0; i < NumButs; i++ )
         scanBut(i);

      /* read next pot */

      readPot( nextPot );
      if ( ++nextPot >= NumPots )
         nextPot = 0;
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
   const signed char prThresh       = 5;    // threshold (# scans) for a press

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
            console.postBut( num, butCount[num] < prThresh ? butDTAP 
                                                           : butTPRESS );
            tapPending[num] = false;
            butCount[num]   = 0;
         }
         else
         {
            if ( butCount[num] >= prThresh )
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

         if ( tapPending[num] && butCount[num] <= -prThresh )
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
 *  Name:  readPot
 *
 *  Desc:  Perform an analog read of a pot, and post a corresponding event 
 *         if has changed value from the last read.
 *
 *  Args:  num              - pot number to read
 *
 *  Glob:  potPin[]         - associated pin # per pot 
 *        +potVal[]         - last value read per pot
 *
 *  Note:  This routine tries to filter out the "jitter" in succesive raw 
 *         values that are read from the pots.
 *
 *----------------------------------------------------------------------------*/      

void readPot( byte num ) 
{
   #define JITTER_EPSILON 2

   #ifdef CLOCKWISE_POTS 
       word val = analogRead( potPin[num] );
   #else
       word val = 1023 - analogRead( potPin[num] );
   #endif

   if ( val > potVal[num] + JITTER_EPSILON || 
        (int )val < ((int )potVal[num] - JITTER_EPSILON) 
       )
   {
      byte lastPost = potVal[num] >> 2;
      byte newPost  = val >> 2;
      potVal[num]   = val; 
      if ( newPost != lastPost )
         console.postPot( num, newPost );
   }

   #undef JITTER_EPSILON 
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  scanKeys
 *
 *  Desc:  Perform a hardware scan of the keys. Post a key-down event for the 
 *         lowest key pressed, if any. 
 *
 *  Args:  -- none --
 *
 *  Glob:  +curKey          - currently pressed key (-1 means none)
 *         +curKeyOct       - octave of currently pressed key
 *          numKeys         - # of onboard keys
 *
 *----------------------------------------------------------------------------*/      

void scanKeys() 
{
   for ( byte i = 0; i < numKeys; i++ )
      if ( readKey(i) )
      {
         curKey = i;
         curKeyOct = getOctave();
         console.postKeyDn( i, curKeyOct );
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

const byte keyPins[] PROGMEM = { 13, 12, 10, 8, 7, 6, 5, 4, 3, 2, 19, 18, 17, 26 };

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

/*----------------------------------------------------------------------------*
 *
 *  Name:  freeRam
 *
 *  Desc:  Returns the size of the space between the heap and the stack.
 *
 *  Rets:  size             - size of space between heap and stack
 *
 *----------------------------------------------------------------------------*/      

int freeRam ()                            
{
   extern int __heap_start, *__brkval; 
   int v; 
   return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

/* ---------------------           LEDs           -------------------------- */

/*
   Note: LEDs are not operable if the serial port is enabled.
*/

void blinkLED( byte nth )
{
   #ifndef USE_SERIAL_PORT
   if ( nth < NumLEDs )
      blinking[ nth ] = true;
   #endif
}

void onLED( byte nth )
{
   #ifndef USE_SERIAL_PORT
   if ( nth < NumLEDs )
   {
      digitalWrite( LEDPin[nth], LED_ON );
      blinking[ nth ] = false;
   }
   #endif
}

void offLED( byte nth )
{
   #ifndef USE_SERIAL_PORT
   if ( nth < NumLEDs )
   {
      digitalWrite( LEDPin[nth], LED_OFF );
      blinking[ nth ] = false;
   }
   #endif
}

byte getBlinkRate()
{
   #ifndef USE_SERIAL_PORT
      return blinkRate;
   #else
      return 0;
   #endif
}

void setBlinkRate( byte rate )
{
   #ifndef USE_SERIAL_PORT
   if ( rate ) 
      blinkRate = rate;
   #endif
}

/* ---------------------      Onboard Keyboard    -------------------------- */

void downOctave()
{
   if ( octave > 0 ) setOctave( octave-1 );
}

void upOctave()
{
   setOctave( octave+1 );
}

byte getOctave()
{
   return octave;
}

void setOctave( byte o )
{
   if ( o > masterTuning->maxOctave ) o = masterTuning->maxOctave;
   if ( o < masterTuning->minOctave ) o = masterTuning->minOctave;
   octave = o;
}

/* ----------------------   Audio Output Buffer    ------------------------- */

#define NUMRECS  NUMBUFS*audioBufSz         // total # of buffered records 

byte audioL[NUMRECS];                  // records for left audio channel 
byte audioR[NUMRECS];                  // records for right audio channel

volatile boolean lock[NUMBUFS];        // write-lock status per buffer

volatile byte w;                       // idx of record being written to
volatile byte r;                       // idx of record being read from
volatile byte eor;                     // idx to end of current read buffer 

volatile byte wBuf;                    // buffer # being written to
volatile byte rBuf;                    // buffer # being read from

/* -------------------------------------------------------------------------- 

                          SYSTEM AUDIO CONSTANTS

         audioRate          - # audio ticks per second
         bufRate            - # audio buffers rendered per second
         bufsPerDyna        - # audio buffers rendered per dynamic update
         dynaRate           - # dynamic updates per second

  --------------------------------------------------------------------------- */

   // double refclk      = 31372.549;  // ideal ISR rate = 16MHz / 510
const double refclk      = 31376.6;    // real ISR rate as measured

const double audioRate   = refclk*.5;  // PWM regs updated every other tick
const word   ticksPerSec = audioRate;  // for use with integer down counters

#define IDEAL_DYNA_RATE    150.0       // ideal dynamic update rate

const double bufRate     = audioRate / audioBufSz;
const byte   bufsPerDyna = bufRate / IDEAL_DYNA_RATE; 
const double dynaRate    = bufRate / bufsPerDyna;

byte    dynaDC;                        // downcounter to next dynamic update

Synth  *matrix;                        // ptr to synth that generates the audio
boolean stereo;                        // true if synth produces stereo output

/*----------------------------------------------------------------------------*
 *
 *  Name:  audio_setup
 *
 *  Desc:  Initialize the audio module.
 *
 *  ARGS:  synth            - ptr to synth which generates the audio 
 *
 *  GLOB:  bufsPerDyna      - # audio buffers rendered per dynamic update
 *        +dynaDC           - downcounter (in bufs) to next dynamic update
 *        +eor              - idx to end of current read buffer 
 *        +lock[]           - write-lock status per buffer
 *        +matrix           - ptr to synth which generates the audio
 *        +r                - idx of record being read from
 *        +rBuf             - buffer # being read from
 *        +stereo           - if true, synth produces stereo output
 *        +w                - idx of record being written to
 *        +wBuf             - buffer # being written to
 *
 *----------------------------------------------------------------------------*/      

void audio_setup( Synth *x )
{
   /* initialize soft parameters */

   matrix       = x;
   stereo       = x->inStereo();

   dynaDC       = bufsPerDyna;

   /* initialize audio output buffers */

   wBuf = 0;
   w    = 0;

   rBuf = NUMBUFS-1;
   r    = rBuf * audioBufSz;
   eor  = r + audioBufSz;

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

   /* initialize PWM registers to "0" to mitigate glitch when timer enabled */

   OCR1A = 127;                         
   OCR2A = 127;                         

}

/*----------------------------------------------------------------------------*
 *
 *  Name:  render_audio
 *
 *  Desc:  Render the next free audio buffer, if available, via the matrix 
 *         synth's output() method. Manage the matrix synth's dynamics.
 *         Update any blinking LEDs.
 *
 *  Rets:  status           - if true, a buffer was rendered
 *
 *  Glob:  blinking[]       - true if LED is blinking
 *         blinkRate        - blink rate (in dynamic updates)
 *        +blinkDC          - downcounter to next blink transition
 *        +blinkEdge        - if true, turn blinking LEDs on at transition
 *        +dynaDC           - downcounter to next dynamic update
 *        +lock[]           - write-lock status per buffer
 *         matrix           - ptr to synth which generates the audio
 *         stereo           - if true, synth output is in stereo
 *        +usingCpu         - currently using CPU to render audio 
 *        +w                - idx of record being written to
 *        +wBuf             - buffer # being written to
 *
 *----------------------------------------------------------------------------*/      

boolean render_audio()
{
   if ( lock[wBuf] )                   // return if buffer is locked
      return false;

   #ifdef MONITOR_CPU
      usingCpu = true;
   #endif

   if ( stereo )
      ((StereoSynth *)matrix)->output( (char *)&audioL[w], (char *)&audioR[w] ); 
   else
   {
      ((MonoSynth *)matrix)->output( (char *)&audioL[w] );
      byte wCopy = w;
      for ( byte i = audioBufSz; i > 0; i-- )
         audioR[wCopy] = audioL[wCopy++];
   }

   if ( --dynaDC == 0 )                // manage dynamics 
   {
      matrix->dynamics();              // perform a dynamic update

      #ifndef USE_SERIAL_PORT       
      if ( --blinkDC == 0 )            // update any blinking LEDs 
      {
         blinkDC   = blinkRate;        // reload downcounter
         blinkEdge = ! blinkEdge;      // invert edge
         for ( byte i = 0; i < NumLEDs; i++ )
            if ( blinking[i] )
               digitalWrite( LEDPin[i], blinkEdge ? LED_ON : LED_OFF );
      } 
      #endif

      dynaDC = bufsPerDyna;            // reload downcounter
   }

   lock[wBuf] = true;                  // lock buffer from further writes

   if ( ++wBuf == NUMBUFS )            // bump wBuf (circular)
   {
      wBuf = 0;
         w = 0;                 
   }
   else
   {
      w += audioBufSz;
   }

   #ifdef MONITOR_CPU
      usingCpu = false;
   #endif

   return true;
}

void bufStats()
{
   console.romprint( CONSTR(" {locks ") );
   for ( byte i = 0; i < NUMBUFS; i++ )
      console.print( lock[i] ? '1' : '0' );
   console.romprint( CONSTR("} ") );
}

void audio::disable()
{
   enabled = false;
   cbi (TIMSK1,TOIE1);                 // disable Timer1 Interrupt
}

void audio::enable()
{
   sbi (TIMSK1,TOIE1);                 // enable Timer1 Interrupt
   enabled = true;
}

void audio::wait( byte nbufs )
{
   if ( enabled )
   {
      while ( --nbufs )
      {
         byte lastBuf = wBuf;
         while ( lastBuf == wBuf )
            render_audio();
      }
   }
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  ISR(TIMER1_OVF_vect)
 *
 *  Desc:  Timer 1 Interrupt Service Routine
 *         Load buffered audio to PW registers
 *         If MONITOR_CPU, update cpu usage statistics
 *
 *  GLOB: audioL[]          - buffered records for left audio channel 
 *        audioR[]          - buffered records for right audio channel 
 *        +cpu              - % CPU used rendering audio (parts per 256) 
 *        +cpuCount         - counter for sampling CPU usage
 *        +eor              - idx to end of current read buffer 
 *        +lock[]           - write-lock status per buffer
 *        +r                - idx of record being read from
 *        +rBuf             - buffer # being read from
 *        +runCpu           - running CPU usage (parts per cpuCount)
 *        usingCpu          - currently using CPU to render audio 
 *
 *  Note: A halving of the effective audio rate is achieved by executing the 
 *        ISR body every other interrupt.
 *
 *----------------------------------------------------------------------------*/      

ISR(TIMER1_OVF_vect) 
{
   static byte toggle;

   if ( (toggle ^= 1 ) == 0 )       // execute ISR body every other interrupt
      return;
         
   OCR1A = audioL[r] + 128;         // normalize and load buffered audio 
   OCR2A = audioR[r] + 128;         // to PWM registers

   if ( ++r == eor )                // bump r, check for end of buffer
   {
      lock[rBuf] = false;           // unlock this buffer for writing
      if ( ++rBuf == NUMBUFS )      // bump rBuf (circular)
      {
         rBuf = 0;
            r = 0;
      }
      eor = r + audioBufSz;         // adjust eor accordingly
   }

   #ifdef MONITOR_CPU               // monitor % cpu used rendering audio

   if ( ++cpuCount == 0 )           // if CPU sampling counter rolls over
   {
      cpu = runCpu;                 // update cpu%
      runCpu = 0;                   // reset running cpu%  
   }
   if ( usingCpu )                  // if currently rendering audio
      ++runCpu;                     // bump running cpu%

   #endif

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

/******************************************************************************
 *
 *                   Rand16 (Random 16-bit Number Generator)
 *
 ******************************************************************************/

#define LFSR_MASK  0xD295              // xor LFSR with this during feedback

/*----------------------------------------------------------------------------*
 *
 *  Name:  Rand16::next
 *
 *  Desc:  Generate the next random value.
 *
 *  Memb: +output           - random 16-bit value
 *
 *----------------------------------------------------------------------------*/      

void Rand16::next()
{
   byte perform_xor = output._.lsb & 0x01;
   output.val >>= 1;
   if ( perform_xor )
      output.val ^= LFSR_MASK;
}

/*----------------------------------------------------------------------------*
 *
 *  Name:  Rand16::reseed
 *
 *  Desc:  Reseed the generator.
 *
 *  Memb: +output           - random 16-bit value
 *
 *  Note:  This routine does not produce a useful value if called from startup 
 *         code.
 *
 *----------------------------------------------------------------------------*/      

void Rand16::reseed()
{
   output._.msb = TCNT1;
   output._.lsb = TCNT2;
}

