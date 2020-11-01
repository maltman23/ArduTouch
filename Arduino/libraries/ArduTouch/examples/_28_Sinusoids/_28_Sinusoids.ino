//
//  _28_Sinusoids.ino
//
//  This sketch allows the user to browse through all the Sinusoid wave tables
//  in the ArduTouch library.
//
//               --------     What is a Sinusoid?    --------
//
//  A Sinusoid is a sine wave combined with none, one or more of its harmonics 
//  (which are also sine waves). Each sine wave component (harmonic) of the 
//  sinusoid is present at a specific relative amplitude. The Ardutouch library 
//  has 48 different sinusoids. 
//
//
//              --------    How To Use This Sketch   --------
//
//
//  1) Compile this sketch using the __FULLHOST__ Runtime Model.
//  2) Upload it to the ArduTouch card.
//  3) From the Arduino IDE select Tools > Serial Monitor
//  4) The Arduino Serial Monitor will open.
//  5) You should see the following in the monitor's output window:
//
//                 Sinusoids [1.00]
//         synth> 
//      sinusoid>
//
//  6) If you don't see the above output make sure the baud rate for the monitor
//     is set to 115200 in the dropbox in the lower right-hand corner of the
//     monitor.
//
//  7) Play the ArduTouch keyboard. You can lower or raise the octave setting
//     of the keyboard by tapping the left or right buttons respectively.
//
//  8) Use the top pot on the ArduTouch card to select different sinusoids.
//     Each time a new sinusoid is chosen it's name will be printed to the
//     Serial Monitor's output window:
//
//                 Sinusoids [1.00]
//         synth> 
//      sinusoid> {waveform Sn2}
//      sinusoid> {waveform Sn2b}
//      sinusoid> {waveform Sn4b}
//
//  9) When you find a sinusoid you like you can incorporate it into your synth
//     by loading it's table into your oscillator:
//
//      osc->setTable( wavetable( Sn4b ) );   // use "Sn4b" wavetable from library
//
//
//     --------   Some Technical Info on the Sinusoid Wave Tables   --------
//
//
//  All of the sinusoid wave tables in the Ardutouch library contain exactly
//  256 samples which define exactly one wave length of the sinusoid. Any
//  harmonics which are present in the sinusoid are phase-locked with the
//  fundamental.
//
//  The general purpose WaveOsc and FastWaveOsc oscillator classes can be used
//  with any kind of wave table: complex waveforms as well as sinusoids. However, 
//  if you know in advance that your oscillator will only be using sinusoids then
//  you should use a class TabOsc256 oscillator (or a class derived from it),
//  because the TabOsc256 osciilator class is tailored specifically for wave 
//  tables that contain 256 samples defining exactly one wave length. The
//  TabOsc256 oscillator runs faster and produces a slightly higher precision 
//  output than the general purpose oscillators.
//
//  Target:   ArduTouch board
// 
//  ---------------------------------------------------------------------------
// 
//  Copyright (C) 2020, Cornfield Electronics, Inc.
// 
//  This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 
//  Unported License.
// 
//  To view a copy of this license, visit
//  http://creativecommons.org/licenses/by-sa/3.0/
// 
//  Created by Bill Alessi & Mitch Altman.
// 
//  ---------------------------------------------------------------------------
                                          
#include "ArduTouch.h"                    // use the ArduTouch library 

about_program( Sinusoids, 1.00 )          // specify sketch name & version

/*----------------------------------------------------------------------------*
 *                              WaveBank
 *----------------------------------------------------------------------------*/      

// Out WaveBank consists of all 48 sinusoid wave tables in the ArduTouch library

begin_bank( sinusoids )             // with relative strengths of harmonics:
                         
   _wavetable( Sine )               // 1 - 100%
   _wavetable( Sn2  )               // 1 - 100%  2 - 40%  
   _wavetable( Sn2b )               // 1 - 100%  2 - 20%  
   _wavetable( Sn3  )               // 1 - 100%  3 - 40%  
   _wavetable( Sn3b )               // 1 - 100%  3 - 20%  
   _wavetable( Sn4  )               // 1 - 100%  4 - 40%  
   _wavetable( Sn4b )               // 1 - 100%  4 - 20%  
   _wavetable( Sn5  )               // 1 - 100%  5 - 40%  
   _wavetable( Sn5b )               // 1 - 100%  5 - 20%  
   _wavetable( Sn6  )               // 1 - 100%  6 - 40%  
   _wavetable( Sn6b )               // 1 - 100%  6 - 20%  
   _wavetable( Sn8  )               // 1 - 100%  8 - 40%  
   _wavetable( Sn8b )               // 1 - 100%  8 - 20%  
   _wavetable( Sn23 )               // 1 - 100%  2 - 30%  3 - 15%
   _wavetable( Sn24 )               // 1 - 100%  2 - 25%  4 - 12% 
   _wavetable( Sn24b )              // 1 - 100%  2 - 50%  4 - 25% 
   _wavetable( Sn25 )               // 1 - 100%  2 - 25%  5 - 16%
   _wavetable( Sn26 )               // 1 - 100%  2 - 20%  6 - 40% 
   _wavetable( Sn34 )               // 1 - 100%  3 - 25%  4 - 16% 
   _wavetable( Sn35 )               // 1 - 100%  3 - 25%  5 - 12% 
   _wavetable( Sn35b )              // 1 - 100%  3 - 50%  5 - 25% 
   _wavetable( Sn36 )               // 1 - 100%  3 - 18%  6 - 24% 
   _wavetable( Sn38 )               // 1 - 100%  3 - 12%  8 - 24% 
   _wavetable( Sn39 )               // 1 - 100%  3 - 12%  9 - 24% 
   _wavetable( Sn45 )               // 1 - 100%  4 - 25%  5 - 16% 
   _wavetable( Sn46 )               // 1 - 100%  4 - 25%  6 - 12% 
   _wavetable( Sn56 )               // 1 - 100%  5 - 25%  6 - 16% 
   _wavetable( Sn234 )              // 1 - 100%  2 - 25%  3 - 40%  4 - 08% 
   _wavetable( Sn235 )              // 1 - 100%  2 - 40%  3 - 20%  5 - 10% 
   _wavetable( Sn245 )              // 1 - 100%  2 - 25%  4 - 40%  5 - 08% 
   _wavetable( Sn258 )              // 1 - 100%  2 - 20%  5 - 30%  8 - 20% 
   _wavetable( Sn267 )              // 1 - 100%  2 - 25%  6 - 40%  7 - 08% 
   _wavetable( Sn268 )              // 1 -  50%  2 - 80%  6 - 20%  8 - 12% 
   _wavetable( Sn345 )              // 1 - 100%  3 - 25%  4 - 40%  5 - 08% 
   _wavetable( Sn346 )              // 1 - 100%  3 - 60%  4 - 40%  6 - 20% 
   _wavetable( Sn456 )              // 1 - 100%  4 - 25%  5 - 40%  6 - 08% 
   _wavetable( Sn468 )              // 1 - 100%  4 - 25%  6 - 40%  8 - 12% 
   _wavetable( Sn357 )              // 1 - 100%  2 - 60%  5 - 40%  7 - 20% 
   _wavetable( Sn368 )              // 1 -  50%  3 - 80%  6 - 20%  8 - 12% 
   _wavetable( Sn2345 )             // 1 - 100%  2 - 40%  3 - 60%  4 - 20%  5 - 30% 
   _wavetable( Sn2346 )             // 1 - 100%  2 - 40%  3 - 24%  4 - 20%  6 - 12% 
   _wavetable( Sn2468 )             // 1 - 100%  2 - 80%  3 - 60%  4 - 40%  5 - 20% 
   _wavetable( Sn2479 )             // 1 - 100%  2 - 33%  4 - 20%  7 - 40%  9 - 12% 
   _wavetable( Sn2568 )             // 1 - 100%  2 - 24%  5 - 12%  6 - 16%  8 - 08% 
   _wavetable( Sn3456 )             // 1 - 100%  3 - 33%  4 - 25%  5 - 33%  6 - 50% 
   _wavetable( Sn3579 )             // 1 - 100%  3 - 80%  5 - 60%  7 - 40%  9 - 20% 
   _wavetable( Sn4567 )             // 1 - 100%  4 - 40%  5 - 60%  6 - 20%  7 - 30% 
   _wavetable( Sn5678 )             // 1 - 100%  5 - 40%  6 - 60%  7 - 20%  8 - 30% 

end_bank()

/******************************************************************************
 *
 *                             SinusoidOsc 
 *
 ******************************************************************************/

//  We derive a "browser-of-sinusoids" oscillator from the BankOsc256 class in 
//  the library.
//
//  BankOsc256 is derived from the TabOsc256 class. 
//
//  BankOsc256 has a method for loading the nth wave table from the wavebank 
//  into the oscillator.
//
//   void   BankOsc256::setTableFromBank( byte nth );  // set table to nth member of wavebank
//

class SinusoidOsc : public BankOsc256
{
   typedef BankOsc256 super;              // superclass is BankOsc256

   public:

   bool evHandler( obEvent ev )           // handle an onboard event
   {
      switch ( ev.type() )
      {
         // top pot selects a sinusoid, and prints waveform name to console

         case POT0:                       // select a sinusoid
         {
            byte prior = wavebank.choice();  // indice of last selected sinusoid

            word nth   = ev.getPotVal() * wavebank.numMembers() / 256; 
            if ( nth != prior )           
            {
               wavebank.select( nth );
               setTableFromBank( nth );
               console.infoStr( CONSTR("waveform"), name );
               console.newprompt();
            }
            break;
         }
         default:
            return super::evHandler(ev);
      }
   }

   PROMPT_STR( sinusoid )

} ;

/******************************************************************************
 *
 *                            SinusoidSynth 
 *
 ******************************************************************************/

class SinusoidSynth : public OneVoxSynth
{
   typedef OneVoxSynth super;             // superclass is OneVoxSynth

   public:

   void config()
   {
      wavebank.load( sinusoids );         // load sinusoid wave tables before configuring synth
      super::config();                    // this will callback newOsc() and newVox()
   }

   // We will use our own special "browser-of-sinusoids" oscillator

   Osc *newOsc( byte nth )                // create nth osc 
   {
      SinusoidOsc *o = new SinusoidOsc();
      o->setTableFromBank( 0 );           // set waveform to member 0 of bank
      return o;
   }

   // We will use a bare Voice. Otherwise the library will create a StockVoice for our
   // synth by default, and a StockVoice contains an ADSR envelope and a vibrato -- which
   // will add a bunch of code and cause the binary to be too large!
      
   Voice *newVox( byte nth )              
   {                                      
      return new Voice();
   }

   void welcome()
   {
      console.pushMode( vox[0]->osc );    // operate synth from within our "browser" osc
   }

} mySynth;

void setup()
{
   ardutouch_setup( &mySynth );                   
}

void loop()
{
   ardutouch_loop();                            
}                                             

                                             
