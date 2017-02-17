# $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/haathi-5-run/makefile.windows.mk#1 $

# If OpenOffice isn't installed in the default directory, then the command
# below must be edited so that OODIR reflects the actual installation directory.

# The full directory is included in $(Dir) because when the $(SolutionDir) is
# used it includes spaces and a trailing \ which messes everything up.

# The CD is used because unopkg requires an incredible number of DLLs which
# aren't distributed with this source.  Don't try unopkg.exe because it won't
# work.  Use the fact that unopkg.com gets priority and does work.

# Unfortunately, nmake can't run an interactive session and Visual Studio can't
# accept input anyway, so the user will have to run the batch file manually.

OODIR = C:\Program Files (x86)\OpenOffice 4
INFILE = $(ProjectDir)..\oxt\$(ConfigurationName)\haathi.oxt
OUTFILE = $(ProjectDir)..\run\haathi-$(ConfigurationName).bat

"$(OUTFILE)": "$(INFILE)"
	@echo @echo *** > "$(OUTFILE)"
	@echo @echo *** Note that this will only work if OpenOffice is _not_ already running. >> "$(OUTFILE)"
	@echo @echo *** >> "$(OUTFILE)"
	@echo @pause >> "$(OUTFILE)"
	@echo cd "$(OODIR)\program" >> "$(OUTFILE)"
	@echo unopkg add -f "$(INFILE)" >> "$(OUTFILE)"
	@echo @pause >> "$(OUTFILE)"
	@echo ***
	@echo *** Please run $(OUTFILE) from the command line (or by double clicking). ***
	@echo ***
#	"$(OUTFILE)"

build: "$(OUTFILE)"

rebuild: build

clean:
	erase /Q "$(OUTFILE)"
