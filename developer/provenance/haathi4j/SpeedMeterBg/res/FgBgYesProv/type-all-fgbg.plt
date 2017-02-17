terminal = "pngcairo"
output = "type-all-fgbg.png"

set datafile missing "-"

schemeCount = 6
fileCount = 4

if (exists("terminal")) {
	set terminal terminal enhanced mono solid
}
else {
	set terminal windows enhanced monochrome solid
}

if (exists("output")) set output output

#set title "Document Type Performance for Multiple Schemes"
set style data linespoints
set key autotitle columnhead

set ylabel "Rate (words per minute, WPM)"
set yrange [ 0 : 500 ]
set ytics 100
set grid ytics

set xlabel "Document"
set xrange [ -2.5 : fileCount - 0.5 ]

set multiplot

set key at -0.2, 400 right
plot for [ i = 1 : schemeCount ] "spd-all-fg.dat" using 3 : xticlabels(1) index i - 1 pointtype i + 2

set key at -0.2, 150 right
plot for [ i = 1 : schemeCount ] "spd-all-bg.dat" using 3 : xticlabels(1) index i - 1 pointtype i + 2

unset multiplot