set DIR=%CD%
set PATH=.;%PATH%

pushd "D:\ProgramFilesx86\OpenOffice4\program"
"%DIR%\..\Debug-StatLink\autoRnd.exe" W1 50000 "%DIR%\rnd.wm"
popd