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
copy bck.bat             %bckdir% > NUL
copy dis.bat             %bckdir% > NUL
copy vc.bat              %bckdir% > NUL
copy vcb.bat             %bckdir% > NUL
copy grepfile.lst        %bckdir% > NUL
copy versions.txt        %bckdir% > NUL
echo      %bckdir% done.
endlocal