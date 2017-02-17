# $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/haathi-4-oxt/makefile.linux.mk#1 $

PWD := $(shell pwd)
UNAME := $(shell uname -s)
ifeq ($(UNAME), Darwin)
	O_SYS := MacOSX_x86
	oSYS := macintosh
	EXT := dylib
	CONFIGURATION_NAME := $(ConfigurationName)-Mac
else
	O_SYS := Linux_x86_64
	oSYS := linux
	EXT := so
	CONFIGURATION_NAME := $(ConfigurationName)-Linux
endif

DIR := ../pkg/$(CONFIGURATION_NAME)
UNDIR := ../../haathi-4-oxt

../oxt/$(CONFIGURATION_NAME)/haathi.oxt: ../dll/$(CONFIGURATION_NAME)/haathi.uno.$(EXT)
	rm -f -r $(DIR)
	mkdir $(DIR)
	cp -f -r ../src/config $(DIR)
	cp -f -r ../src/pkg/description $(DIR)
	cp -f -r ../src/pkg/help $(DIR)
	cp -f -r ../src/images $(DIR)
	cp -f -r ../src/pkg/registration $(DIR)

	mkdir $(DIR)/META-INF
	cp -f ../src/pkg/META-INF/manifest.$(oSYS).xml $(DIR)/META-INF/manifest.xml
	
	mkdir $(DIR)/$(O_SYS)
	cp -f ../dll/$(CONFIGURATION_NAME)/haathi.uno.$(EXT) $(DIR)/$(O_SYS)/haathi.uno.$(EXT)

	cp -f ../src/pkg/Addons.xcu $(DIR)
	cp -f ../src/pkg/description.$(oSYS).xml $(DIR)/description.xml
	cp -f ../src/pkg/haathi.$(oSYS).components $(DIR)/haathi.components
	cp -f ../src/pkg/ProtocolHandler.xcu $(DIR)
	cp -f ../src/pkg/dictionary.txt $(DIR)
	
	rm -f -r ../oxt/$(CONFIGURATION_NAME)
	mkdir ../oxt/$(CONFIGURATION_NAME)
	cd $(DIR) && zip -r -u "$(UNDIR)/../oxt/$(CONFIGURATION_NAME)/haathi.oxt" *

all: ../oxt/$(CONFIGURATION_NAME)/haathi.oxt
	
test:
	@echo "$(O_SYS)"
	@echo "$(oSYS)"
	@echo "$(EXT)"
	@echo "$(CONFIGURATION_NAME)"
	@echo "$(DIR)"
	@echo "$(UNDIR)"
	
clean:
	rm -f -r $(DIR)
	rm -f -r ../oxt/$(CONFIGURATION_NAME)
