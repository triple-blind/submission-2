all: only

only: density.plt dens.dat
	$(GNUPLOT) -e "terminal='$(TERMINAL)'; input='dens.dat'; output='density.$(EXT)';" density.plt

%.plt: ../../../DensityMeter/res/%.plt
	cp $< $@

%.dat: ../../../DensityMeter/res/%.dat
	cp $< $@
