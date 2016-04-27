@echo off
REM
REM  bck.bat
REM
REM  Perform a version backup of the TwoTone project
REM
REM  Args:  %1    - backup version number
REM

setlocal
set "bckdir=..\_backup\DuoPoly\dp_%1"

if not exist %bckdir% goto bck
rmdir %bckdir% /S /Q

:bck           
mkdir %bckdir%

copy DuoPoly.ino         %bckdir% > NUL
copy Audio.h             %bckdir% > NUL
copy Bank.h              %bckdir% > NUL
copy Bank.cpp            %bckdir% > NUL
copy but.h               %bckdir% > NUL
copy Chnl.h              %bckdir% > NUL
copy Chnl.cpp            %bckdir% > NUL
copy Config.h            %bckdir% > NUL
copy ConKeyBrd.h         %bckdir% > NUL
copy ConKeyBrd.cpp       %bckdir% > NUL
copy Console_.h          %bckdir% > NUL
copy Console.cpp         %bckdir% > NUL
copy Control.h           %bckdir% > NUL
copy Control.cpp         %bckdir% > NUL
copy DuoSynth.h          %bckdir% > NUL
copy DuoSynth.cpp        %bckdir% > NUL
copy Effect.h            %bckdir% > NUL
copy Effect.cpp          %bckdir% > NUL
copy Filter.h            %bckdir% > NUL
copy Filter.cpp          %bckdir% > NUL
copy key.h               %bckdir% > NUL
copy LFO.h               %bckdir% > NUL
copy LFO.cpp             %bckdir% > NUL
copy Mode.h              %bckdir% > NUL
copy Mode.cpp            %bckdir% > NUL
copy Phonic.h            %bckdir% > NUL
copy StepSqnc.h          %bckdir% > NUL
copy StepSqnc.cpp        %bckdir% > NUL
copy Synth.h             %bckdir% > NUL
copy Synth.cpp           %bckdir% > NUL
copy System.h            %bckdir% > NUL
copy System.cpp          %bckdir% > NUL
copy Tuning.h            %bckdir% > NUL
copy Tuning.cpp          %bckdir% > NUL
copy types.h             %bckdir% > NUL
copy Voice.h             %bckdir% > NUL
copy WaveBank.h          %bckdir% > NUL
copy WaveBank.cpp        %bckdir% > NUL
copy WaveOsc.h           %bckdir% > NUL
copy WaveOsc.cpp         %bckdir% > NUL
copy bck.bat             %bckdir% > NUL
copy dis.bat             %bckdir% > NUL
copy vc.bat              %bckdir% > NUL
copy vcb.bat             %bckdir% > NUL
copy grepfile.lst        %bckdir% > NUL
copy versions.txt        %bckdir% > NUL
echo      %bckdir% done.
endlocal