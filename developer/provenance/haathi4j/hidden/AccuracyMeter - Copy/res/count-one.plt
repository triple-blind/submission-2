if (!exists("fileCount")) fileCount = 2
if (!exists("schemeCount")) schemeCount = 3
#terminal = "pngcairo"
if (!exists("input")) input = "acc-one.dat"
#output = "acc-one.png"

if (exists("terminal")) {
	set terminal terminal mono solid
}
else {
	set terminal windows monochrome solid
}
if (exists("output")) set output output

set title "Watermark Counts"
set style data linespoints
set key autotitle columnhead

set ylabel "Watermarks"
set yrange [ 90 : 100 ]
set ytics 10
set grid ytics

set xlabel "Watermark Scheme"
set xrange [ -0.5 : schemeCount + 0.5 ]

plot for [ i = 1 : fileCount ] input using 2 : xticlabels(1) index i - 1
