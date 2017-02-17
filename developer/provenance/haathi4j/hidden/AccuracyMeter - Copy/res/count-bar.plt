#terminal = "pngcairo"
if (!exists("input")) input = "acc.dat"
#output = "count.png"

if (exists("terminal")) {
	set terminal terminal mono solid
}
else {
	set terminal windows monochrome solid
}
if (exists("output")) set output output

unset key
set yrange [ 0 : 140 ]
set style fill solid
set boxwidth 0.5
set title "Watermark Counts"
set xlabel "Watermark Scheme"
set ylabel "Watermarks"
plot input using 2:xticlabels(1) with boxes
