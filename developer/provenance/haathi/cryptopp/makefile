# $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/cryptopp/makefile#1 $
DIR = ../../libcpp/cryptopp
UNDIR = ../../haathi/cryptopp

Default/libcryptopp.a:
	cd $(DIR) && make -f $(UNDIR)/GNUmakefile static
	mkdir -p Default
	cp $(DIR)/libcryptopp.a Default

all: Default/libcryptopp.a
#	cd $(DIR) && make -f $(UNDIR)/GNUmakefile clean

clean:
	cd $(DIR) && make -f $(UNDIR)/GNUmakefile clean
	rm -rf Default


