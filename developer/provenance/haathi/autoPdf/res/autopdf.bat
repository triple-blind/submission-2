set DIR=%CD%
set PATH=.;%PATH%

pushd "C:\Program Files (x86)\OpenOffice 4\program"
"%DIR%\..\Debug-StatLink\autoPdf.exe" S1 V2 "%DIR%\doc.txt" "%DIR%\doc.pdf" "%DIR%\doc.odt" "%DIR%\in.wm" "%DIR%\in.txt"
popd