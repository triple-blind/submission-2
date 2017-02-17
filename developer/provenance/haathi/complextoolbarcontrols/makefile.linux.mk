# $Header: //MyDataDepot/Projects/provenance/haathi/complextoolbarcontrols/complextoolbarcontrols.mak#4 $

# In order to create this makefile, the OpenOffice SDK complextoolbarcontrols example was built
# and the commands captured.  The commands were then edited to fit a new directory structure.

UNAME := $(shell uname -s)
PWD := $(shell pwd)

ifeq ($(UNAME), Darwin)
#		-malign-natural
	OSYS := Macintosh
	oSYS := macintosh
	COPTIONS := \
		-c \
		-fPIC \
		-fno-common \
		-arch i386 \
		-fvisibility=hidden \
		-O
	OSDEF := MACOSX
	# change to static
	LOPTIONS := \
		-dynamiclib \
		-single_module \
		-Wl,-multiply_defined,suppress \
		-arch i386
	DYLIBDIR := /Applications/OpenOffice.app/Contents/MacOS
	INSTALL := /Applications/OpenOffice.app/Contents
	LIBRARIES := \
		-luno_cppuhelpergcc3 \
		-luno_cppu \
		-luno_sal \
		-Wl,-dylib_file,@__________________________________________________URELIB/libuno_cppuhelper'gcc3.dylib.3:$(DYLIBDIR)/libuno_cppuhelpergcc3.dylib' \
		-Wl,-dylib_file,@__________________________________________________URELIB/libuno_cppu.dylib.3:'$(DYLIBDIR)/libuno_cppu.dylib' \
		-Wl,-dylib_file,@__________________________________________________URELIB/libuno_sal.dylib.3:'$(DYLIBDIR)/libuno_sal.dylib'
	LIBRARY := complextoolbarcontrols.uno.dylib
	OSYSDIR := MacOSX_x86
	POSTLINK :=	install_name_tool \
		-change @__________________________________________________URELIB/libuno_sal.dylib.3 @executable_path/libuno_sal.dylib.3 \
		-change @__________________________________________________URELIB/libuno_cppu.dylib.3 @executable_path/libuno_cppu.dylib.3 \
		-change @__________________________________________________URELIB/libuno_cppuhelpergcc3.dylib.3 @executable_path/libuno_cppuhelpergcc3.dylib.3 \
		-change @__________________________________________________URELIB/libuno_salhelpergcc3.dylib.3 @executable_path/libuno_salhelpergcc3.dylib.3 \
		-change @__________________________________________________URELIB/libreg.dylib.3 @executable_path/libreg.dylib.3 \
		-change @__________________________________________________URELIB/libstore.dylib.3 @executable_path/libstore.dylib.3 \
		dll/$(LIBRARY)	
	SETLIB=export DYLD_LIBRARY_PATH=../exe/$(OSYS)
else
	OSYS := Linux
	oSYS := linux
	COPTIONS := \
		-c \
		-fpic \
		-fvisibility=hidden \
		-O
	OSDEF := LINUX
	LOPTIONS := \
		-shared '-Wl,-rpath,$ORIGIN'
	INSTALL := /opt/openoffice4
	LIBRARIES := \
		-l:libuno_cppuhelpergcc3.so.3 \
		-l:libuno_cppu.so.3 \
		-l:libuno_sal.so.3
	LIBRARY := complextoolbarcontrols.uno.so
	OSYSDIR := Linux_x86_64
	POSTLINK := echo "No postlink stage"
	SETLIB=export LD_LIBRARY_PATH=../exe/$(OSYS)	
endif

CPPUMAKER=../sdk/$(OSYS)/bin/cppumaker

# Compile options, include directories, and defines

INCLUDES = \
	-I. \
	-I../lib \
	-I../../inc \
	-I../../../sdk/$(OSYS)/include \
	-I../../../sdk/$(OSYS)/include/stl \
	-I../../../../libcpp/boost/NonWindows/include
DEFINES = \
	-DUNX \
	-DGCC \
	-D$(OSDEF) \
	-DCPPU_ENV=gcc3 \
	-DHAVE_GCC_VISIBILITY_FEATURE
	
all :
# Generate some include files
# exe/types.rdb comes from the OpenOffice executable distribution
	chmod u+x $(CPPUMAKER)
	$(SETLIB); $(CPPUMAKER) -Gc -BUCR -O./inc ../exe/$(OSYS)/types.rdb
# Compile the source files
	cd ./src/lib && gcc $(COPTIONS) $(INCLUDES) $(DEFINES) -o../../obj/MyProtocolHandler.o MyProtocolHandler.cxx
	cd ./src/lib && gcc $(COPTIONS) $(INCLUDES) $(DEFINES) -o../../obj/ListenerHelper.o ListenerHelper.cxx
	cd ./src/dll && gcc $(COPTIONS) $(INCLUDES) $(DEFINES) -o../../obj/exports.o exports.cxx
# Link object files and libraries
	g++ $(LOPTIONS) -L"../exe/$(OSYS)" $(LIBRARIES) -o dll/$(LIBRARY) obj/MyProtocolHandler.o obj/ListenerHelper.o obj/exports.o
	$(POSTLINK)
# Package it all up
	cp src/pkg/META-INF/manifest.$(oSYS).xml pkg/META-INF/manifest.xml
	cp dll/$(LIBRARY) pkg/$(OSYSDIR)
	cp src/pkg/complextoolbarcontrols.$(oSYS).components pkg/complextoolbarcontrols.components
	cp src/pkg/Addons.xcu pkg
	cp src/pkg/ProtocolHandler.xcu pkg	
	cp src/images/logo_big.png pkg
	cp src/images/logo_small.png pkg	
	cd pkg && zip -r -x .gitempty -u "../oxt/complextoolbarcontrols.oxt" *
# This (complextoolbarcontrols.oxt) can now be installed by starting up OpenOffice and using
# the Extension Manager.  After installation, the program must be restarted.
# Check View | Toolbars | Complex Toolbar Controls to show the controls.
# Optionally, the commands below can help if OpenOffice is not running
	cd $(INSTALL)/program && ./unopkg add -f "$(PWD)/oxt/complextoolbarcontrols.oxt"


rebuild : build

clean :
	rm -f -r ./inc/com
	rm -f ./dll/*
	touch ./dll/.gitempty
	rm -f ./log/*
	touch ./log/.gitempty
	rm -f ./obj/*
	touch ./obj/.gitempty
	rm -f ./oxt/*
	touch ./oxt/.gitempty
	rm -f ./pkg/META-INF/*
	touch ./pkg/META-INF/.gitempty
	rm -f ./pkg/$(OSYSDIR)/*
	touch ./pkg/$(OSYSDIR)/.gitempty
	find ./pkg -maxdepth 1 -type f -exec rm -v \{\} \;
