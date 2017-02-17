MAKE = make
GNUPLOT = /cygdrive/d/ProgramFilesx86/gnuplot/bin/gnuplot.exe # gnuplot

PROJECTS = \
	density.prj
#	accuracy.prj \
#	precision.prj
#	speed.prj

all: png tex

tex: cairolatex # epslatex pslatex

%.prj:
	cd $*; $(MAKE) GNUPLOT=$(GNUPLOT) TERMINAL=$(TERMINAL) EXT=$(EXT)

png:
	$(MAKE) TERMINAL=pngcairo EXT=png $(PROJECTS)

epslatex:
	$(MAKE) TERMINAL=epslatex EXT=tex $(PROJECTS)

pslatex:
	$(MAKE) TERMINAL=pslatex EXT=tex $(PROJECTS)
	
cairolatex:
	$(MAKE) TERMINAL="cairolatex" EXT=tex $(PROJECTS)

clean:
	find . -type f -name '*.png' -delete
	find . -type f -name '*.tex' -delete
	find . -type f -name '*.eps' -delete
	find . -type f -name '*.pdf' -delete

help:
	@echo Try make all, make png, make tex, or make clean.
	@echo Check the subdirectories for the resulting image files.
