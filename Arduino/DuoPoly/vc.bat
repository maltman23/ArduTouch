@echo off
REM
REM  vc.bat
REM
REM  Compare a project file against a backup version
REM
REM  Args:  %1    - name of file to compare
REM         %2    - version # of backup
REM
@vcl ..\_backup\DuoPoly\dp_%2\%1 %1
