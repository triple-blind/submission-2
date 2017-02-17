if (!exists("schemeCount")) schemeCount=2
#terminal = "pngcairo"
if (!exists("input")) input = "acc.dat"
#output = "accuracy.png"

if (exists("terminal")) {
	set terminal terminal mono solid
}
else {
	set terminal windows monochrome solid
}
if (exists("output")) set output output

set ytics 10
set grid ytics

unset key
set yrange [ 0 : 140 ]
set style fill solid
set boxwidth 0.5
set title "Watermark Accuracy"
set xlabel "Watermark Scheme"
set ylabel "Accuracy (%)"
plot input using 3:xticlabels(1) with boxes
