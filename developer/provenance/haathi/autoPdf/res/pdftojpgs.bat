@echo off
rem This is the directory of autotype.exe and the doc.txt file
set DIR=%CD%

set R=%1
set N=%2
set IN=%3
set OUT=%4

IF NOT %R%.==. GOTO YesR
  set R=400
:YesR

IF NOT %N%.==. GOTO YesN
  set N=90
:YesN

IF NOT %IN%.==. GOTO YesIN
  set IN=doc.pdf
:YesIN

rem Must escape the percent sign
IF NOT %OUT%.==. GOTO YesOUT
  set OUT=doc%%d.jpg
:YesOUT

pushd "D:\ProgramFiles\gs\gs9.18"
@echo on
bin\gswin64 -dSAFER -dBATCH -dNOPAUSE -dTextAlphaBits=4 -dGraphicsAlphaBits=4 -r%R% -dJPEGQ=%N% -sDEVICE=jpeg "-sOutputFile=%DIR%\%OUT%" "%DIR%\%IN%"
@echo off
popd


