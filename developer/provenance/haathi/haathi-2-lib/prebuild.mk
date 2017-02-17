# $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/haathi-2-lib/prebuild.mk#1 $

UNAME := $(shell uname -s)
ifeq ($(UNAME), Darwin)
	DIR := ../../exe/Macintosh
	LIBS := \
		$(DIR)/libuno_cppu.dylib \
		$(DIR)/libuno_cppuhelpergcc3.dylib \
		$(DIR)/libuno_sal.dylib \
		$(DIR)/libuno_salhelpergcc3.dylib
else
	LIBS :=
endif

$(DIR)/libuno_cppu.dylib: $(DIR)/libuno_cppu.dylib.3
	ln $(DIR)/libuno_cppu.dylib.3 $(DIR)/libuno_cppu.dylib
	
$(DIR)/libuno_cppuhelpergcc3.dylib: $(DIR)/libuno_cppuhelpergcc3.dylib.3
	ln $(DIR)/libuno_cppuhelpergcc3.dylib.3 $(DIR)/libuno_cppuhelpergcc3.dylib

$(DIR)/libuno_sal.dylib: $(DIR)/libuno_sal.dylib.3
	ln $(DIR)/libuno_sal.dylib.3 $(DIR)/libuno_sal.dylib

$(DIR)/libuno_salhelpergcc3.dylib: $(DIR)/libuno_salhelpergcc3.dylib.3
	ln $(DIR)/libuno_salhelpergcc3.dylib.3 $(DIR)/libuno_salhelpergcc3.dylib

all: $(LIBS)

