# $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/haathi-1-idl/makefile.linux.mk#1 $

.SUFFIXES : .urd .idl

UNAME := $(shell uname -s)
ifeq ($(UNAME), Darwin)
	OSYS := Macintosh
	SETLIB := export DYLD_LIBRARY_PATH=../exe/$(OSYS)
else
	OSYS := Linux
	SETLIB := export LD_LIBRARY_PATH=../exe/$(OSYS)
endif

CPPUMAKER=../sdk/$(OSYS)/bin/cppumaker
IDLC=../sdk/$(OSYS)/bin/idlc
UCPP=../sdk/$(OSYS)/bin/ucpp
REGMERGE=../exe/$(OSYS)/regmerge

../reg/types.uno.tmp : ../exe/$(OSYS)/types.rdb
	chmod u+x $(CPPUMAKER)
	$(SETLIB); $(CPPUMAKER) -Gc -BUCR -O../inc ../exe/$(OSYS)/types.rdb
	echo Made >> ../reg/types.uno.tmp
	rm -f ../reg/haathi.uno.rdb

office : ../reg/types.uno.tmp

.idl.urd :
	chmod u+x $(IDLC)
	chmod u+x $(UCPP)
	$(SETLIB); $(IDLC) -I . -I ../sdk/$(OSYS)/idl -O ../reg $<

merge : ../reg/*.urd
	chmod u+x $(REGMERGE)
	chmod u+x $(CPPUMAKER)
	$(SETLIB); $(REGMERGE) ../reg/haathi.uno.rdb /UCR $?
	$(SETLIB); $(CPPUMAKER) -Gc -BUCR -O../inc ../reg/haathi.uno.rdb -X ../exe/$(OSYS)/types.rdb	

../reg/haathi.uno.rdb : ../src/idl/*.idl
	rm -f ../reg/haathi.uno.rdb
	make -f makefile.linux.mk $(patsubst %.idl,%.urd,$?)
	make -f makefile.linux.mk merge

haathi : ../reg/haathi.uno.rdb

all : office haathi

test:
	echo $(UNAME)
	echo $(OSYS)

clean :
	rm -f -r ../inc/com
	rm -f -r ../inc/haathi
	rm -f ../reg/*
	touch ../reg/.gitempty
