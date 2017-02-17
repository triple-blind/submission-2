if (!exists("fileCount")) fileCount = 4
if (!exists("schemeCount")) schemeCount = 6
terminal = "pngcairo"
if (!exists("input")) input = "acc-all-manual.dat"
output = "acc-all.png"

if (exists("terminal")) {
	set terminal terminal mono solid
}
else {
	set terminal windows monochrome solid
}
if (exists("output")) set output output

set title "Watermark Accuracy for Combined Schemes"
set style data linespoints
set key autotitle columnhead

set ylabel "Accuracy (%)"
set yrange [ 80 : 110 ]
set ytics 10
set grid ytics

set xlabel "Watermark Scheme"
set xrange [ -0.5 : schemeCount + 0.5 ]

plot for [ i = 1 : fileCount ] input using 3 : xticlabels(1) index i - 1
