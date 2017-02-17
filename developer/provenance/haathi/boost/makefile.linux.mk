# $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/boost/makefile.linux.mk#1 $
HOME=../../libcpp/boost/NonWindows
VERSION=boost_1_55_0
DIR=$(HOME)/$(VERSION)
PWD=$(shell pwd)

UNAME := $(shell uname -s)

ifeq ($(UNAME), Darwin)
	CXXFLAGS := cxxflags="-fPIC -fno-common -arch i386 -fvisibility=hidden"
	LINKFLAGS := linkflags="-arch i386"
else
	CXXFLAGS := cxxflags="-fPIC -Wno-unused-local-typedefs -Wno-unused-but-set-variable -Wno-reorder"
	LINKFLAGS := 
endif

$(DIR)/bootstrap.sh: $(DIR).tar.gz
	cd $(HOME) && tar xvzf $(VERSION).tar.gz 1>/dev/null
	touch $(DIR)/bootstrap.sh

$(DIR)/b2: $(DIR)/bootstrap.sh
	cd $(DIR) && ./bootstrap.sh
	touch $(DIR)/b2

$(HOME)/lib/libboost_serialization.a: $(DIR)/b2
	cd $(DIR) && ./b2 $(CXXFLAGS) $(LINKFLAGS) install --with-serialization --prefix=..

Default/libboost_serialization.a: $(HOME)/lib/libboost_serialization.a
	mkdir -p Default
	cp -f $(HOME)/lib/libboost_serialization.a Default/libboost_serialization.a

#Only put the library in project, because otherwise Eclipse crashes analyzing the header files
all: Default/libboost_serialization.a

clean:
	rm -f -r $(HOME)/lib
	rm -f -r $(HOME)/include
	rm -f -r $(DIR)
	rm -f -r Default