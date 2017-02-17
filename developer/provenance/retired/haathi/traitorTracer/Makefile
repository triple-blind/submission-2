CC=g++
CFLAGS=-g -c -Wall -O0
OBJDIR = ./obj
OUTPUTDIR=./bin
LPATH = -L./../../../../external_libs/lib/ -lcyassl -lcryptopp\
	/usr/lib/libboost_serialization.so.1.49.0
LFLAGS=-ltesseract
INCLUDESWITCH = -I
INCLUDE= $(INCLUDESWITCH)./../../../ $(INCLUDESWITCH)./../../../../external_libs/include $(INCLUDESWITCH)/usr/local/include/tesseract/ $(INCLUDESWITCH)/usr/local/include/leptonica/

all: 	json.o network.o crypto.o meta.o helper.o luminance.o spacing.o dual.o main.o
	$(CC) $(OBJDIR)/json.o $(OBJDIR)/crypto.o $(OBJDIR)/network.o $(OBJDIR)/meta.o $(OBJDIR)/helper.o $(OBJDIR)/luminance.o $(OBJDIR)/spacing.o $(OBJDIR)/dual.o $(OBJDIR)/main.o $(LPATH) $(LFLAGS) -o $(OUTPUTDIR)/tracker

json.o:
	$(CC) $(INCLUDE) $(CFLAGS) ./../utils/cJSON.c -o $(OBJDIR)/json.o

main.o:	./TraitorTracer.cxx
	$(CC) $(INCLUDE) $(CFLAGS) ./TraitorTracer.cxx -o $(OBJDIR)/main.o

luminance.o:	./LuminanceWatermarkExtractor.cxx
	$(CC) $(INCLUDE) $(CFLAGS) ./LuminanceWatermarkExtractor.cxx -o $(OBJDIR)/luminance.o

spacing.o:	./WordSpacingWatermarkExtractor.cxx
	$(CC) $(INCLUDE) $(CFLAGS) ./WordSpacingWatermarkExtractor.cxx -o $(OBJDIR)/spacing.o

dual.o:	./DualWatermarkExtractor.cxx
	$(CC) $(INCLUDE) $(CFLAGS) ./DualWatermarkExtractor.cxx -o $(OBJDIR)/dual.o

network.o:
	$(CC) $(INCLUDE) $(CFLAGS) ./../network/network.cxx -o $(OBJDIR)/network.o
crypto.o:
	$(CC) $(INCLUDE) $(CFLAGS) ./../crypto/crypto.cxx -o $(OBJDIR)/crypto.o

meta.o:
	$(CC) $(INCLUDE) $(CFLAGS) ./MetadataProvider.cxx -o $(OBJDIR)/meta.o

helper.o:
	$(CC) $(INCLUDE) $(CFLAGS) ./BitsetComparer.cxx -o $(OBJDIR)/helper.o
clean:
	rm $(OBJDIR)/*.*
	rm $(OUTPUTDIR)/*
run:
	$(OUTPUTDIR)/tracker


