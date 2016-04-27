@echo off
REM
REM  vcb.bat
REM
REM  Compare different backup versions of a project file
REM
REM  Args:  %1    - name of file to compare
REM         %2    - version # of 1st backup
REM         %3    - version # of 2nd backup 
REM
@vcl ..\_backup\DuoPoly\dp_%3\%1 ..\_backup\DuoPoly\dp_%2\%1
