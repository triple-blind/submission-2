# $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/haathi-6-zip/makefile.windows.mk#1 $

# To run this from the command line, use
# nmake /I /f makefile.windows.mk zip
# from this project directory.

.SUFFIXES : .prj .vcproj .project

PROJDIR = haathi-6-zip
CPPDIR = ..
JAVADIR = ..\..\servers
UNCPPDIR = $(PROJDIR)
UNJAVADIR = ..\haathi\$(PROJDIR)

PROVENANCE = provenance

CPP_PROJECTS = \
	$(CPPDIR)\autoPdf\autoPdf.prj \
	$(CPPDIR)\autotype\autotype.prj \
	$(CPPDIR)\boost\boost.prj \
	$(CPPDIR)\boostTest\boostTest.prj \
	$(CPPDIR)\bugModify\bugModify.prj \
	$(CPPDIR)\bugTest\bugTest.prj \
	$(CPPDIR)\catch\catch.prj \
	$(CPPDIR)\catchTest\catchTest.prj \
	$(CPPDIR)\cJSON\cJSON.prj \
	$(CPPDIR)\cJSONTest\cJSONTest.prj \
	$(CPPDIR)\complextoolbarcontrols\complextoolbarcontrols.prj \
	$(CPPDIR)\cryptopp\cryptopp.prj \
	$(CPPDIR)\cryptoppTest\cryptoppTest.prj \
	$(CPPDIR)\cyassl\cyassl.prj \
	$(CPPDIR)\cyasslClientTest\cyasslClientTest.prj \
	$(CPPDIR)\cyasslServerTest\cyasslServerTest.prj \
	$(CPPDIR)\cyasslSocketTest\cyasslSocketTest.prj \
	$(CPPDIR)\cyasslTest\cyasslTest.prj \
	$(CPPDIR)\defaultCertificates\defaultCertificates.prj \
	$(CPPDIR)\dependencyTest\dependencyTest.prj \
	$(CPPDIR)\haathi-1-idl\haathi-1-idl.prj \
	$(CPPDIR)\haathi-2-lib\haathi-2-lib.prj \
	$(CPPDIR)\haathi-3-dll\haathi-3-dll.prj \
	$(CPPDIR)\haathi-4-oxt\haathi-4-oxt.prj \
	$(CPPDIR)\haathi-5-run\haathi-5-run.prj \
	$(CPPDIR)\haathi-6-zip\haathi-6-zip.prj \
	$(CPPDIR)\helloWorld\helloWorld.prj \
	$(CPPDIR)\leptonica\leptonica.prj \
	$(CPPDIR)\leptonicaTest\leptonicaTest.prj \
	$(CPPDIR)\pingProvenanceTest\pingProvenanceTest.prj \
	$(CPPDIR)\pingTrustedTest\pingTrustedTest.prj \
	$(CPPDIR)\README\README.prj \
	$(CPPDIR)\serializationTest\serializationTest.prj \
	$(CPPDIR)\somewhere\somewhere.prj \
	$(CPPDIR)\tesseract\tesseract.prj \
	$(CPPDIR)\tesseractTest\tesseractTest.prj \
	$(CPPDIR)\traitorTracer\traitorTracer.prj \
	$(CPPDIR)\userCreate\userCreate.prj \
	$(CPPDIR)\watermarkTest\watermarkTest.prj

.vcproj.prj:
	rmdir /Q /S $(@D)\Auto
	rmdir /Q /S $(@D)\Debug-DynLink
	rmdir /Q /S $(@D)\Debug-StatLink
	rmdir /Q /S $(@D)\Release-DynLink
	rmdir /Q /S $(@D)\Release-StatLink
#	rmdir /Q /S $(@D)\.settings
	erase /Q $(@D)\*.vcproj.*.*.user

tidycpp: $(CPP_PROJECTS)
	echo Remember to clean the entire solution first!
	
	erase /Q /S $(CPPDIR)\lib
	type nul > $(CPPDIR)\lib\.gitempty
	rmdir /Q /S $(CPPDIR)\lib\Debug-DynLink
	rmdir /Q /S $(CPPDIR)\lib\Debug-StatLink
	rmdir /Q /S $(CPPDIR)\lib\Release-DynLink
	rmdir /Q /S $(CPPDIR)\lib\Release-StatLink
	
	erase /Q /S $(CPPDIR)\dll
	type nul > $(CPPDIR)\dll\.gitempty
	rmdir /Q /S $(CPPDIR)\dll\Debug-DynLink
	rmdir /Q /S $(CPPDIR)\dll\Debug-StatLink
	rmdir /Q /S $(CPPDIR)\dll\Release-DynLink
	rmdir /Q /S $(CPPDIR)\dll\Release-StatLink
	
	erase /Q /S $(CPPDIR)\zip
	type nul > $(CPPDIR)\zip\.gitempty
	erase /Q /S $(CPPDIR)\KWA-TOSHIBA\Keith Alcock\*.user
#	erase /Q /S $(CPPDIR)\*.user
	erase /Q /S $(CPPDIR)\*.pdb
	erase /Q /S $(CPPDIR)\*.ncb
	erase /Q /AH $(CPPDIR)\*.suo

	erase /Q /S $(CPPDIR)\log
	type nul > $(CPPDIR)\log\.gitempty

JAVA_PROJECTS = \
	$(JAVADIR)\CertificateGeneratorTest\.prj \
	$(JAVADIR)\ComboServer\.prj \
	$(JAVADIR)\Configure\.prj \
	$(JAVADIR)\CustomBouncyCastle\.prj \
	$(JAVADIR)\DatabaseGenerator\.prj \
	$(JAVADIR)\DeployComboServer\.prj \
	$(JAVADIR)\DeployList\.prj \
	$(JAVADIR)\DeployNewUser\.prj \
	$(JAVADIR)\JavaDoc\.prj \
	$(JAVADIR)\JSON\.prj \
	$(JAVADIR)\JSONTest\.prj \
	$(JAVADIR)\KeystoreGenerator\.prj \
	$(JAVADIR)\List\.prj \
	$(JAVADIR)\MockClient\.prj \
	$(JAVADIR)\MockServer\.prj \
	$(JAVADIR)\MockServerTest\.prj \
	$(JAVADIR)\NewUser\.prj \
	$(JAVADIR)\PasswordHash\.prj \
	$(JAVADIR)\PasswordHashTest\.prj \
	$(JAVADIR)\Pinger\.prj \
	$(JAVADIR)\PrivateKeyExtractor\.prj \
	$(JAVADIR)\ProvenanceLocalSQLiteDataAccessorTest\.prj \
	$(JAVADIR)\ProvenanceServer\.prj \
	$(JAVADIR)\ProvenanceServerTest\.prj \
	$(JAVADIR)\README\.prj \
	$(JAVADIR)\SharedServer\.prj \
	$(JAVADIR)\SQLite\.prj \
	$(JAVADIR)\SQLiteTest\.prj \
	$(JAVADIR)\SynchronizeTest\.prj \
	$(JAVADIR)\TrustedLocalSQLiteDataAccessorTest\.prj \
	$(JAVADIR)\TrustedServer\.prj \
	$(JAVADIR)\TrustedServerTest\.prj \
	$(JAVADIR)\UserCreate\.prj \
	$(JAVADIR)\WordNet\.prj

.project.prj:
	rmdir /Q /S $(@D)\bin
#	rmdir /Q /S $(@D)\.settings

tidyjava: $(JAVA_PROJECTS)
	erase /Q /S $(JAVADIR)\.metadata\.plugins\org.eclipse.jdt.core\*.*
	rmdir /Q /S $(JAVADIR)\.metadata\.plugins\org.eclipse.core.resources\.history

tidy: tidycpp tidyjava	

zipcpp-metadata:
# Zip the Eclipse workspace for C++
	cd $(CPPDIR)
	erase /Q metadata.zip
	bin\zip -r -u metadata.zip .metadata\*.*
	cd $(UNCPPDIR)

zipcpp-launches:
# Zip the Eclipse launch configurations for C++
	cd $(CPPDIR)
	erase /Q launches.zip
	bin\zip -r -u launches.zip launches\*.*
	cd $(UNCPPDIR)

zipcpp: zipcpp-metadata
# zipcpp-launches

zipjava-metadata:
# Zip the Eclipse workspace for Java
	cd $(JAVADIR)
	erase /Q metadata.zip
	..\haathi\bin\zip -r -u metadata.zip .metadata\*.*
	cd $(UNJAVADIR)

zipjava-launches:
# Zip the Eclipse launch configurations for Java
	cd $(JAVADIR)
	erase /Q launches.zip
	..\haathi\bin\zip -r -u launches.zip launches\*.*
	cd $(UNJAVADIR)

zipjava: zipjava-metadata
# zipjava-launches

zip: tidy zipcpp zipjava
#	erase /Q /S ..\..\*.log
#	cd ..\..\..
#	$(PROVENANCE)\haathi\bin\zip -r -u $(PROVENANCE)\haathi\zip\$(PROVENANCE).zip $(PROVENANCE)\*.*
#	cd $(PROVENANCE)\haathi

buildcpp: tidycpp zipcpp

buildjava: tidyjava zipjava

build: tidycpp
# zip
	@echo "***************************************************************************"
	@echo "* Add 'zip' to the 'build' target if you really want to clean up and zip. *"
	@echo "***************************************************************************"
	
rebuild: build

clean:
	erase /Q /S $(CPPDIR)\zip
	type nul > $(CPPDIR)\zip\.gitempty
	