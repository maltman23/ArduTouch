                        ArduTouch Library 
                           README.TXT

1) Make sure that the PCB REV of your board (printed near the bottom of the
   ArduTouch card) corresponds to the revision declared in System.h 

2) Arduino build 1.6.6 generates smaller executables for large scale synths
   than the current hourly builds (1.8.6 as of this writing). If you are 
   running out of program storage space we recommend you use build 1.6.6,
   (and also uncomment the "//#define BUILD_166" statement in Model.h, if
   you are running synths supplied by Cornfield Electronics.)
   
