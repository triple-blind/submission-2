# $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/haathi-3-dll/postbuild.mk#1 $

# This runs substantially differently in eclipse than it does from the
# command line for some reason and therefore the roundabout...
# We start out in the Debug or Release directory and this verifies
PWD=$(shell pwd)
UNAME := $(shell uname -s)

ifeq ($(UNAME), Darwin)
	EXT := dylib
	POSTLINK :=	install_name_tool \
		-change @__________________________________________________URELIB/libuno_sal.dylib.3 @executable_path/libuno_sal.dylib.3 \
		-change @__________________________________________________URELIB/libuno_cppu.dylib.3 @executable_path/libuno_cppu.dylib.3 \
		-change @__________________________________________________URELIB/libuno_cppuhelpergcc3.dylib.3 @executable_path/libuno_cppuhelpergcc3.dylib.3 \
		-change @__________________________________________________URELIB/libuno_salhelpergcc3.dylib.3 @executable_path/libuno_salhelpergcc3.dylib.3 \
		-change @__________________________________________________URELIB/libreg.dylib.3 @executable_path/libreg.dylib.3 \
		-change @__________________________________________________URELIB/libstore.dylib.3 @executable_path/libstore.dylib.3 \
		../../dll/$(ConfigurationName)/haathi.uno.$(EXT)	
else
	EXT := so
	POSTLINK := echo "No postlink stage"
endif

../../dll/$(ConfigurationName)/haathi.uno.$(EXT): libhaathi.uno.$(EXT)
	rm -f -r ../../dll/$(ConfigurationName)
	mkdir ../../dll/$(ConfigurationName)
	cp -f libhaathi.uno.$(EXT) ../../dll/$(ConfigurationName)/haathi.uno.$(EXT)
	$(POSTLINK)

all: ../../dll/$(ConfigurationName)/haathi.uno.$(EXT)

