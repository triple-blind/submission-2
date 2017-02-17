# $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/haathi-1-idl/makefile.windows.mk#1 $

# These special suffixes are used in rules that follow
.SUFFIXES : .urd .idl

# The executables make use of DLLs which need to be tracked down at runtime.
# The path is changed locally to isolate the larger development environment.
SETLIB=(set PATH=..\exe\Windows) &
# .idl -> .urd
IDLC=$(SETLIB) ..\sdk\Windows\bin\idlc.exe
# .urd -> .rdb
REGMERGE=$(SETLIB) ..\exe\Windows\regmerge.exe
# .rdb -> .hpp
CPPUMAKER=$(SETLIB) ..\sdk\Windows\bin\cppumaker.exe

# The tmp file is just a flag
..\reg\types.uno.tmp: ..\exe\Windows\types.rdb
#	Here is the types.rdb -> .hpp conversion of OpenOffice
	$(CPPUMAKER) -Gc -BUCR -O..\inc ..\exe\Windows\types.rdb
	echo Made >> ..\reg\types.uno.tmp
	erase /S ..\reg\haathi.uno.rdb

# This tmp file can be erased in order to force recreation of files in ../inc
office: ..\reg\types.uno.tmp

# To make the .urd file, run IDLC on the .idl file
.idl.urd:
	$(IDLC) -I. -I..\sdk\Windows\idl -O..\reg $<

# To make the .rdb file, combine (merge) all the .urd files
merge: ..\reg\*.urd
	$(REGMERGE) ..\reg\haathi.uno.rdb /UCR $(**)
# Note: following does not work with simple -Oinc!
	$(CPPUMAKER) -Gc -BUCR -O..\inc ..\reg\haathi.uno.rdb -X ..\exe\Windows\types.rdb

# The haathi database is dependent upon all the .idl files
..\reg\haathi.uno.rdb: ..\src\idl\*.idl
	erase /S ..\reg\haathi.uno.rdb
#	Each .idl produces a .urd file which needs to be made first	
	nmake /f makefile.windows.mk $(**:.idl=.urd)
#	All of the .urd files need to be merged into a single .rdb file
	nmake /f makefile.windows.mk merge

# This will be the haathi database
haathi: ..\reg\haathi.uno.rdb

# Combine office and haathi components
build: office haathi

rebuild: build

# Remove all the generated files
clean:
	erase /Q /S ..\inc\com
	rmdir /Q /S ..\inc\com
	erase /Q /S ..\inc\haathi
	rmdir /Q /S ..\inc\haathi
	erase /Q ..\reg\*.*
	type nul > ..\reg\.gitempty