# $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/complextoolbarcontrols/makefile.windows.mk#1 $

# In order to create this makefile, the OpenOffice SDK complextoolbarcontrols example was built
# under MinGW and the commands captured.  The commands were then edited to fit a new directory
# structure and remove dependencies on MinGW.

SETLIB=(set PATH=..\exe\Windows) &
CPPUMAKER=$(SETLIB) ..\sdk\Windows\bin\cppumaker.exe

# Compile options
C_DEBUG = -MTd -D_DEBUG -Zi -FD -Fd"..\dll\vc90.pdb" -Od
C_RELEASE = -MT -DNDEBUG
C_VERSION = $(C_DEBUG)

# Link options
L_DEBUG = msvcrtd.lib msvcprtd.lib /DEBUG /PDB:"..\dll\vc90.pdb"
L_RELEASE = msvcrt.lib msvcprt.lib
L_VERSION = $(L_DEBUG)

build :
# Generate some include files
# exe\types.rdb comes from the OpenOffice executable distribution
# -O must have a path component (\) and doesn't work without it
# To save time, comment out this next line after build
	$(CPPUMAKER) -Gc -BUCR -O".\inc" "..\exe\Windows\types.rdb"
# Compile the source files
	cd .\src\lib
	cl -c $(C_VERSION) -Zm500 -Zc:forScope,wchar_t- -wd4251 -wd4275 -wd4290 -wd4675 -wd4786 -wd4800 -GR -EHa -I"../../../sdk/Windows/include/stl" -I"." -I"../../inc" -I"../../../sdk/Windows/include" -DWIN32 -DWNT -D_DLL -DCPPU_ENV=msci -Fo"..\..\obj\MyProtocolHandler.obj" "MyProtocolHandler.cxx"
	cl -c $(C_VERSION) -Zm500 -Zc:forScope,wchar_t- -wd4251 -wd4275 -wd4290 -wd4675 -wd4786 -wd4800 -GR -EHa -I"../../../sdk/Windows/include/stl" -I"." -I"../../inc" -I"../../../sdk/Windows/include" -DWIN32 -DWNT -D_DLL -DCPPU_ENV=msci -Fo"..\..\obj\ListenerHelper.obj" "ListenerHelper.cxx"
	cd ..\dll
	cl -c $(C_VERSION) -Zm500 -Zc:forScope,wchar_t- -wd4251 -wd4275 -wd4290 -wd4675 -wd4786 -wd4800 -GR -EHa -I"../../../sdk/Windows/include/stl" -I"../lib" -I"../../inc" -I"../../../sdk/Windows/include" -DWIN32 -DWNT -D_DLL -DCPPU_ENV=msci -Fo"..\..\obj\exports.obj" "exports.cxx"
	cd ..\..
# Link object files and libraries and create manifest, don't think that map is ever used
	link $(L_VERSION) /NODEFAULTLIB /DLL /DEBUGTYPE:cv /DEF:..\sdk\Windows\settings\component.uno.def /OUT:.\dll\complextoolbarcontrols.uno.dll /MAP:.\dll\complextoolbarcontrols.uno.map obj\MyProtocolHandler.obj obj\ListenerHelper.obj obj\exports.obj ..\sdk\Windows\lib\icppuhelper.lib ..\sdk\Windows\lib\icppu.lib ..\sdk\Windows\lib\isal.lib kernel32.lib
	mt -manifest dll\complextoolbarcontrols.uno.dll.manifest "-outputresource:dll\complextoolbarcontrols.uno.dll;2"
# Package it all up
	copy "src\pkg\META-INF\manifest.windows.xml" "pkg\META-INF\manifest.xml"
	copy "dll\complextoolbarcontrols.uno.dll" "pkg\Windows"
	copy "src\pkg\complextoolbarcontrols.windows.components" "pkg\complextoolbarcontrols.components"
	copy "src\pkg\Addons.xcu" "pkg"
	copy "src\pkg\ProtocolHandler.xcu" "pkg"
	copy "src\images\logo_big.png" "pkg"
	copy "src\images\logo_small.png" "pkg"	
	cd "pkg"
	..\..\bin\zip -r -x .gitempty -u "..\oxt\complextoolbarcontrols.oxt" *.*
	cd ..
# This (complextoolbarcontrols.oxt) can now be installed by starting up OpenOffice and using
# the Extension Manager.  After installation, the program must be restarted.
# Check View | Toolbars | Complex Toolbar Controls to show the controls.
# Optionally, the commands below can help if OpenOffice is not running
	cd "C:\Program Files (x86)\OpenOffice 4\program" 
	unopkg add -f "c:\Users\Keith Alcock\Documents\My Data\Projects\provenance\haathi\complextoolbarcontrols\oxt\complextoolbarcontrols.oxt"

rebuild : build

clean :
	erase /Q /S .\inc\com
	rmdir /Q /S .\inc\com
	erase /Q /S .\dll\*.*
	type nul > .\dll\.gitempty	
	erase /Q /S .\log\*.*
	type nul > .\log\.gitempty	
	erase /Q /S .\obj\*.*
	type nul > .\obj\.gitempty
	erase /Q /S .\oxt\*.*
	type nul > .\oxt\.gitempty
	erase /Q /S .\pkg\META-INF\*.*
	type nul > .\pkg\META-INF\.gitempty
	erase /Q /S .\pkg\Windows\*.*
	type nul > .\pkg\Windows\.gitempty
	erase /Q .\pkg\*.*

	erase /Q .\src\dll\*.idb
	erase /Q .\src\dll\*.pdb	