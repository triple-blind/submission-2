# $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/haathi-4-oxt/makefile.windows.mk#1 $

DIR = ..\pkg\$(ConfigurationName)
UNDIR = ..\..

..\oxt\$(ConfigurationName)\haathi.oxt: ..\dll\$(ConfigurationName)\haathi.uno.dll
	erase /Q /S $(DIR)
	if exist $(DIR) (rmdir /Q /S $(DIR))
	mkdir $(DIR)
	
	xcopy /Y /E /I ..\src\config $(DIR)\config
	xcopy /Y /E /I ..\src\pkg\description $(DIR)\description
	xcopy /Y /E /I ..\src\pkg\help $(DIR)\help
	xcopy /Y /E /I ..\src\images $(DIR)\images
	xcopy /Y /E /I ..\src\pkg\registration $(DIR)\registration
	
	mkdir $(DIR)\META-INF
	copy /Y ..\src\pkg\META-INF\manifest.windows.xml $(DIR)\META-INF\manifest.xml

	mkdir $(DIR)\Windows_x86
	copy /Y ..\dll\$(ConfigurationName)\haathi.uno.dll $(DIR)\Windows_x86
	
	copy /Y ..\src\pkg\Addons.xcu $(DIR)
	copy /Y ..\src\pkg\description.windows.xml $(DIR)\description.xml
	copy /Y ..\src\pkg\haathi.windows.components $(DIR)\haathi.components
	copy /Y ..\src\pkg\ProtocolHandler.xcu $(DIR)
	copy /Y ..\src\pkg\dictionary.txt $(DIR)

	erase /Q /S ..\oxt\$(ConfigurationName)
	if not exist ..\oxt\$(ConfigurationName) (mkdir ..\oxt\$(ConfigurationName))
	cd $(DIR)
	$(UNDIR)\bin\zip -r -u $(UNDIR)\oxt\$(ConfigurationName)\haathi.oxt *.*
	cd $(UNDIR)\haathi-4-oxt

$(ConfigurationName) : ..\oxt\$(ConfigurationName)\haathi.oxt

rebuild:
	nmake /f makefile.windows.mk ConfigurationName=Debug-StatLink Debug-StatLink
	nmake /f makefile.windows.mk ConfigurationName=Debug-DynLink Debug-DynLink
	nmake /f makefile.windows.mk ConfigurationName=Release-StatLink Release-StatLink
	nmake /f makefile.windows.mk ConfigurationName=Release-DynLink Release-DynLink

cleanup :
	erase /Q /S $(DIR)
	rmdir /Q /S $(DIR)

	erase /Q ..\oxt\$(ConfigurationName)\haathi.oxt
	rmdir /Q ..\oxt\$(ConfigurationName)

clean :
	nmake /I /f makefile.windows.mk ConfigurationName=Debug-StatLink cleanup
	nmake /I /f makefile.windows.mk ConfigurationName=Debug-DynLink cleanup
	nmake /I /f makefile.windows.mk ConfigurationName=Release-StatLink cleanup
	nmake /I /f makefile.windows.mk ConfigurationName=Release-DynLink cleanup
	