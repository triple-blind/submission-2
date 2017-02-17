# $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/haathi-6-zip/makefile.linux.mk#1 $

# To run this from the command line, use
# make -f makefile.linux.mk zip

.SUFFIXES : .prj .vcproj

UNAME := $(shell uname -s)
ifeq ($(UNAME), Darwin)
	OS := Mac
	DIR := ../exe/Macintosh
	RMLIBS := \
		rm -f \
		$(DIR)/libuno_cppu.dylib \
		$(DIR)/libuno_cppuhelpergcc3.dylib \
		$(DIR)/libuno_sal.dylib \
		$(DIR)/libuno_salhelpergcc3.dylib
else
	OS := Linux
	RMLIBS :=
endif

DEBUG := Debug-$(OS)
RELEASE := Release-$(OS)

PROJDIR = haathi-6-zip
CPPDIR = ..
JAVADIR = ../../servers
UNCPPDIR = $(PROJDIR)
UNJAVADIR = ../haathi/$(PROJDIR)

PROVENANCE = provenance

CPP_PROJECTS = \
	$(CPPDIR)/autoPdf/autoPdf.prj \
	$(CPPDIR)/autotype/autotype.prj \
	$(CPPDIR)/boost/boost.prj \
	$(CPPDIR)/boostTest/boostTest.prj \
	$(CPPDIR)/bugModify/bugModify.prj \
	$(CPPDIR)/bugTest/bugTest.prj \
	$(CPPDIR)/catch/catch.prj \
	$(CPPDIR)/catchTest/catchTest.prj \
	$(CPPDIR)/cJSON/cJSON.prj \
	$(CPPDIR)/cJSONTest/cJSONTest.prj \
	$(CPPDIR)/complextoolbarcontrols/complextoolbarcontrols.prj \
	$(CPPDIR)/cryptopp/cryptopp.prj \
	$(CPPDIR)/cryptoppTest/cryptoppTest.prj \
	$(CPPDIR)/cyassl/cyassl.prj \
	$(CPPDIR)/cyasslClientTest/cyasslClientTest.prj \
	$(CPPDIR)/cyasslServerTest/cyasslServerTest.prj \
	$(CPPDIR)/cyasslSocketTest/cyasslSocketTest.prj \
	$(CPPDIR)/cyasslTest/cyasslTest.prj \
	$(CPPDIR)/defaultCertificates/defaultCertificates.prj \
	$(CPPDIR)/dependencyTest/dependencyTest.prj \
	$(CPPDIR)/haathi-1-idl/haathi-1-idl.prj \
	$(CPPDIR)/haathi-2-lib/haathi-2-lib.prj \
	$(CPPDIR)/haathi-3-dll/haathi-3-dll.prj \
	$(CPPDIR)/haathi-4-oxt/haathi-4-oxt.prj \
	$(CPPDIR)/haathi-5-run/haathi-5-run.prj \
	$(CPPDIR)/haathi-6-zip/haathi-6-zip.prj \
	$(CPPDIR)/helloWorld/helloWorld.prj \
	$(CPPDIR)/leptonica/leptonica.prj \
	$(CPPDIR)/leptonicaTest/leptonicaTest.prj \
	$(CPPDIR)/pingProvenanceTest/pingProvenanceTest.prj \
	$(CPPDIR)/pingTrustedTest/pingTrustedTest.prj \
	$(CPPDIR)/README/README.prj \
	$(CPPDIR)/serializationTest/serializationTest.prj \
	$(CPPDIR)/somewhere/somewhere.prj \
	$(CPPDIR)/tesseract/tesseract.prj \
	$(CPPDIR)/tesseractTest/tesseractTest.prj \
	$(CPPDIR)/traitorTracer/traitorTracer.prj \
	$(CPPDIR)/userCreate/userCreate.prj \
	$(CPPDIR)/watermarkTest/watermarkTest.prj

.vcproj.prj:
	rm -r -f $(@D)/Debug
	rm -r -f $(@D)/Release
	rm -r -f $(@D)/$(DEBUG)
	rm -r -f $(@D)/$(RELEASE)
#	rm -r -f $(@D)/.settings
	rm -f $(@D)/*.vcproj.*.*.user

tidycpp: $(CPP_PROJECTS)
	echo Remember to clean the entire solution first!
	
	rm -r -f $(CPPDIR)/lib/*
	touch $(CPPDIR)/lib/.gitempty
	rm -r -f $(CPPDIR)/dll/*
	touch $(CPPDIR)/dll/.gitempty
	rm -r -f $(CPPDIR)/zip/*
	touch $(CPPDIR)/zip/.gitempty
	rm -r -f $(CPPDIR)/log/*
	touch $(CPPDIR)/log/.gitempty

	rm -r -f $(CPPDIR)/.metadata/.plugins/org.eclipse.core.resources/.history

JAVA_PROJECTS = \
	$(JAVADIR)/CertificateGeneratorTest/.prj \
	$(JAVADIR)/ComboServer/.prj \
	$(JAVADIR)/Configure/.prj \
	$(JAVADIR)/CustomBouncyCastle/.prj \
	$(JAVADIR)/DatabaseGenerator/.prj \
	$(JAVADIR)/DeployComboServer/.prj \
	$(JAVADIR)/DeployList/.prj \
	$(JAVADIR)/DeployNewUser/.prj \
	$(JAVADIR)/JavaDoc/.prj \
	$(JAVADIR)/JSON/.prj \
	$(JAVADIR)/JSONTest/.prj \
	$(JAVADIR)/KeystoreGenerator/.prj \
	$(JAVADIR)/List/.prj \
	$(JAVADIR)/MockClient/.prj \
	$(JAVADIR)/MockServer/.prj \
	$(JAVADIR)/MockServerTest/.prj \
	$(JAVADIR)/NewUser/.prj \
	$(JAVADIR)/PasswordHash/.prj \
	$(JAVADIR)/PasswordHashTest/.prj \
	$(JAVADIR)/Pinger/.prj \
	$(JAVADIR)/PrivateKeyExtractor/.prj \
	$(JAVADIR)/ProvenanceLocalSQLiteDataAccessorTest/.prj \
	$(JAVADIR)/ProvenanceServer/.prj \
	$(JAVADIR)/ProvenanceServerTest/.prj \
	$(JAVADIR)/README/.prj \
	$(JAVADIR)/SharedServer/.prj \
	$(JAVADIR)/SQLite/.prj \
	$(JAVADIR)/SQLiteTest/.prj \
	$(JAVADIR)/SynchronizeTest/.prj \
	$(JAVADIR)/TrustedLocalSQLiteDataAccessorTest/.prj \
	$(JAVADIR)/TrustedServer/.prj \
	$(JAVADIR)/TrustedServerTest/.prj \
	$(JAVADIR)/UserCreate/.prj \
	$(JAVADIR)/WordNet/.prj

.project.prj:
	rm -r -f $(@D)/bin
#	rm -r -f $(@D)/.settings

tidyjava: $(JAVA_PROJECTS)
	rm -r -f $(JAVADIR)/.metadata/.plugins/org.eclipse.jdt.core/*
	rm -r -f $(JAVADIR)/.metadata/.plugins/org.eclipse.core.resources/.history
	
#	$(RMLIBS)

tidy: tidycpp tidyjava
	find ../.. -name "*.log" -delete
	find ../.. -name "*.pdom" -delete
	find ../.. -name ".DS_Store" -delete

zip: tidy
# Zip the Eclipse workspace for C++
	cd $(CPPDIR); rm -f metadata.zip; zip -r -u metadata.zip .metadata
#	cd $(CPPDIR); rm -f launches.zip; zip -r -u launches.zip launches
	
# Zip the Eclipse workspace for Java
	cd $(JAVADIR); rm -f metadata.zip; zip -r -u metadata.zip .metadata
#	cd $(JAVADIR); rm -f launches.zip; zip -r -u launches.zip launches

	cd ../../..; zip -r -u $(PROVENANCE)/haathi/zip/$(PROVENANCE).zip $(PROVENANCE)/*

all: tidy
# zip
	@echo "*************************************************************************"
	@echo "* Add 'zip' to the 'all' target if you really want to clean up and zip. *"
	@echo "*************************************************************************"
	
clean:
	rm -r -f $(CPPDIR)/zip/*
	touch $(CPPDIR)/zip/.gitempty
