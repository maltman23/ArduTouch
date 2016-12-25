@echo off
REM
REM  dis.bat
REM
REM  Disassemble the project .elf file in the Arduino temp build directory
REM  to disassem.txt
REM

setlocal 

set "f=Simple"

for /f %%X in ('dir "%TEMP%\build*.tmp" /a:d /b /o:-d') do set buildPath=%TEMP%\%%X & goto :buildFolderFound
:buildFolderFound

REM there is an extra space at the end of the path so trim it off
call :trim buildPath %buildPath%

set "elf=%buildpath%\sketch\%f%.cpp.elf"
avr-objdump -d -S "%elf%" > disassem.txt

:end 
exit/b

REM trims whitespace - does not work with strings that contains spaces but arduino doesn't allow spaces in filenames so it's ok
:trim
  SetLocal EnableDelayedExpansion
  set Params=%*
  for /f "tokens=1*" %%a in ("!Params!") do EndLocal & set %1=%%b
goto :eof

endlocal
