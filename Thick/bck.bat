@echo off
REM
REM  bck.bat
REM
REM  Perform a version backup of the Wah project
REM
REM  Args:  %1    - backup version name (sans s_ prefix)
REM

setlocal
set "bckdir=..\_backup\thick\th_%1"

if not exist %bckdir% goto bck
rmdir %bckdir% /S /Q

:bck           
mkdir %bckdir%

copy Thick.ino           %bckdir% > NUL
copy bck.bat             %bckdir% > NUL
copy dis.bat             %bckdir% > NUL
copy grepfile.lst        %bckdir% > NUL
copy versions.txt        %bckdir% > NUL
echo      %bckdir% done.
endlocal
