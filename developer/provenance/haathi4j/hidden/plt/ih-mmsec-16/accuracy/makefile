all: solo combo

solo: accuracy-one.plt count-one.plt acc-one.dat
	$(GNUPLOT) -e "terminal='$(TERMINAL)'; input='acc-one.dat'; output='accuracy-one.$(EXT)';" accuracy-one.plt
	$(GNUPLOT) -e "terminal='$(TERMINAL)'; input='acc-one.dat'; output='count-one.$(EXT)';" count-one.plt

combo: accuracy-all.plt count-all.plt acc-all.dat
	$(GNUPLOT) -e "terminal='$(TERMINAL)'; input='acc-all.dat'; output='accuracy-all.$(EXT)';" accuracy-all.plt
	$(GNUPLOT) -e "terminal='$(TERMINAL)'; input='acc-all.dat'; output='count-all.$(EXT)';" count-all.plt

%.plt: ../../../AccuracyMeter/res/%.plt
	cp $< $@

%.dat: ../../../AccuracyMeter/res/%.dat
	cp $< $@
