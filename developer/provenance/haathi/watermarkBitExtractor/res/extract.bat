@echo off
rem This is the directory containing all the resource files
set DIR=%CD%

set DOC=%1

IF NOT %DOC%.==. GOTO YesDOC
  set DOC=doc.jpg
:YesDOC

pushd "..\..\..\libcpp\tesseract"
@echo on
"%DIR%\..\Debug-DynLink\watermarkBitExtractor" "%DIR%\%DOC%" "%DIR%\in.wm" "%DIR%\out.wm" "%DIR%\out.txt" "%DIR%\dictionary.txt"
@echo off
popd
