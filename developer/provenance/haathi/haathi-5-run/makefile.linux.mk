# $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/haathi-5-run/makefile.linux.mk#1 $

# If OpenOffice isn't installed in the default directory, then the command
# below must be edited so that OODIR reflects the actual installation directory.

# The full directory is included in $(Dir) because when the $(SolutionDir) is
# used it includes spaces and a trailing \ which messes everything up.

# The CD is used because unopkg requires an incredible number of DLLs which
# aren't distributed with this source.  Don't try unopkg.exe because it won't
# work.  Use the fact that unopkg.com gets priority and does work.

# Unfortunately, nmake can't run an interactive session and Visual Studio can't
# accept input anyway, so the user will have to run the batch file manually.

PWD := $(shell pwd)
UNAME := $(shell uname -s)
ifeq ($(UNAME), Darwin)
	OODIR := /Applications/OpenOffice.app/Contents
	CONFIGURATION_NAME := $(ConfigurationName)-Mac
else
	OODIR := /opt/openoffice4
	CONFIGURATION_NAME := $(ConfigurationName)-Linux
endif

INFILE = ../oxt/$(CONFIGURATION_NAME)/haathi.oxt
OUTFILE = ../run/haathi-$(CONFIGURATION_NAME).sh

$(OUTFILE): $(INFILE)
	@echo "echo \"***\"" > $(OUTFILE)
	@echo "echo \"*** Note that this will only work if OpenOffice is _not_ already running.\"" >> $(OUTFILE)
	@echo "echo \"***\"" >> $(OUTFILE)
	@echo "cd $(OODIR)/program && ./unopkg add -f $(PWD)/$(INFILE)" >> $(OUTFILE)

all: $(OUTFILE)
	chmod u+x $(OUTFILE)
	$(OUTFILE)

clean:
	rm -f $(OUTFILE)
