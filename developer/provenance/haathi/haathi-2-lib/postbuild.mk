# $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/haathi-2-lib/postbuild.mk#1 $

# This runs substantially differently in eclipse than it does from the
# command line for some reason and therefore the roundabout...
# We start out in the Debug or Release directory and this verifies

UNAME := $(shell uname -s)
ifeq ($(UNAME), Darwin)
	CONFIGURATION_NAME := $(ConfigurationName)-Mac
else
	CONFIGURATION_NAME := $(ConfigurationName)-Linux
endif

../$(CONFIGURATION_NAME)/ar/boost/catalog.txt: ../../boost/Default/libboost_serialization.a
	rm -f -r ar/boost
	mkdir -p ar/boost

	cd ar/boost; ar -t ../../../../boost/Default/libboost_serialization.a | sed -e '/^__/d' > catalog.txt
	cd ar/boost; ar -x ../../../../boost/Default/libboost_serialization.a

# Putting this first causes problems, especially on Mac
../$(CONFIGURATION_NAME)/ar/cJSON/catalog.txt: ../../cJSON/$(CONFIGURATION_NAME)/libcJSON.a
	rm -f -r ar/cJSON
	mkdir -p ar/cJSON

	cd ar/cJSON; ar -t ../../../../cJSON/$(CONFIGURATION_NAME)/libcJSON.a | sed -e '/^__/d' > catalog.txt
	cd ar/cJSON; ar -x ../../../../cJSON/$(CONFIGURATION_NAME)/libcJSON.a

../$(CONFIGURATION_NAME)/ar/cyassl/catalog.txt: ../../cyassl/$(CONFIGURATION_NAME)/libcyassl.a
	rm -f -r ar/cyassl
	mkdir -p ar/cyassl

	cd ar/cyassl; ar -t ../../../../cyassl/$(CONFIGURATION_NAME)/libcyassl.a | sed -e '/^__/d' > catalog.txt
	cd ar/cyassl; ar -x ../../../../cyassl/$(CONFIGURATION_NAME)/libcyassl.a

../$(CONFIGURATION_NAME)/ar/cryptopp/catalog.txt: ../../cryptopp/Default/libcryptopp.a
	rm -f -r ar/cryptopp
	mkdir -p ar/cryptopp

	cd ar/cryptopp; ar -t ../../../../cryptopp/Default/libcryptopp.a | sed -e '/^__/d' > catalog.txt
	cd ar/cryptopp; ar -x ../../../../cryptopp/Default/libcryptopp.a

../$(CONFIGURATION_NAME)/ar/haathi/catalog.txt: libhaathi-2-lib.a
	rm -f -r ar/haathi
	mkdir -p ar/haathi

	cd ar/haathi; ar -t ../../libhaathi-2-lib.a | sed -e '/^__/d' > catalog.txt
	cd ar/haathi; ar -x ../../libhaathi-2-lib.a

../$(CONFIGURATION_NAME)/libhaathi.a: ../$(CONFIGURATION_NAME)/libhaathi-2-lib.a ../$(CONFIGURATION_NAME)/ar/cJSON/catalog.txt ../$(CONFIGURATION_NAME)/ar/boost/catalog.txt ../$(CONFIGURATION_NAME)/ar/cyassl/catalog.txt ../$(CONFIGURATION_NAME)/ar/cryptopp/catalog.txt ../$(CONFIGURATION_NAME)/ar/haathi/catalog.txt
	rm -f -r ../../lib/$(CONFIGURATION_NAME)
	mkdir -p ../../lib/$(CONFIGURATION_NAME)
	CATALOG="$(shell cat ar/cJSON/catalog.txt)"; cd ar/cJSON; ar -cq ../../../../lib/$(CONFIGURATION_NAME)/libhaathi.a $$CATALOG
	CATALOG="$(shell cat ar/boost/catalog.txt)"; cd ar/boost; ar -cq ../../../../lib/$(CONFIGURATION_NAME)/libhaathi.a $$CATALOG
	CATALOG="$(shell cat ar/cyassl/catalog.txt)"; cd ar/cyassl; ar -cq ../../../../lib/$(CONFIGURATION_NAME)/libhaathi.a $$CATALOG
	CATALOG="$(shell cat ar/cryptopp/catalog.txt)"; cd ar/cryptopp; ar -cq ../../../../lib/$(CONFIGURATION_NAME)/libhaathi.a $$CATALOG
	CATALOG="$(shell cat ar/haathi/catalog.txt)"; cd ar/haathi; ar -cq ../../../../lib/$(CONFIGURATION_NAME)/libhaathi.a $$CATALOG
	ranlib ../../lib/$(CONFIGURATION_NAME)/libhaathi.a
#	rm -f -r ar

../../lib/$(CONFIGURATION_NAME)/libhaathi.a: ../$(CONFIGURATION_NAME)/libhaathi.a

all: ../../lib/$(CONFIGURATION_NAME)/libhaathi.a

