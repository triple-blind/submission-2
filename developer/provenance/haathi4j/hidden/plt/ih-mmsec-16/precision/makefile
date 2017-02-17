all: solo combo

solo: precision-one.plt recall-one.plt count-one.plt prc-one.dat
	$(GNUPLOT) -e "terminal='$(TERMINAL)'; input='prc-one.dat'; output='precision-one.$(EXT)';" precision-one.plt
	$(GNUPLOT) -e "terminal='$(TERMINAL)'; input='prc-one.dat'; output='recall-one.$(EXT)';" recall-one.plt
	$(GNUPLOT) -e "terminal='$(TERMINAL)'; input='prc-one.dat'; output='count-one.$(EXT)';" count-one.plt

combo: precision-all.plt recall-all.plt count-all.plt prc-all.dat
	$(GNUPLOT) -e "terminal='$(TERMINAL)'; input='prc-all.dat'; output='precision-all.$(EXT)';" precision-all.plt
	$(GNUPLOT) -e "terminal='$(TERMINAL)'; input='prc-all.dat'; output='recall-all.$(EXT)';" recall-all.plt
	$(GNUPLOT) -e "terminal='$(TERMINAL)'; input='prc-all.dat'; output='count-all.$(EXT)';" count-all.plt

%.plt: ../../../PrecisionMeter/res/%.plt
	cp $< $@

%.dat: ../../../PrecisionMeter/res/%.dat
	cp $< $@
