@echo off
rem This is the directory of autotype.exe and the doc.txt file
set DIR=%CD%
set PATH=.;%PATH%

set P=%1
set W=%2
set DOC=%3

IF NOT %P%.==. GOTO YesP
  set P=P0
:YesP

IF NOT %W%.==. GOTO YesW
  set W=W0
:YesW

IF NOT %DOC%.==. GOTO YesDOC
  set DOC=doc.txt
:YesDOC

rem This is the directory of the OpenOffice programs
pushd "C:\Program Files (x86)\OpenOffice 4\program"
@echo on
"%DIR%\autotype.exe" T S0 %P% %W% "%DIR%\%DOC%" "%DIR%\log.txt"
@echo off
popd
