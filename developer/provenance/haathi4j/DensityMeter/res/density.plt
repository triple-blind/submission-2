#terminal = "pngcairo"
#output = "density.png"

if (!exists("input")) input = "dens.dat"

set datafile missing "NaN"
#stats does not work with log scales :(
#stats input using 2 nooutput

if (!exists("fileCount")) {
	fileCount = 4 # STATS_blocks
}

if (!exists("schemeCount")) {
	schemeCount = 6 + 1 # (STATS_records + STATS_invalid) / STATS_blocks
}

if (exists("terminal")) {
	set terminal terminal mono solid
}
else {
	set terminal windows monochrome solid
}

if (exists("output")) set output output

set title "Watermark Density"
set style data linespoints

set key autotitle columnhead left bottom

set ylabel "Average Watermarks per Full Page"
set yrange [ 0.1 : 1000 ]
set logscale y

set xlabel "Document"
set xrange [ -1.5 : fileCount - 0.5 ]

plot for [ i = 1 : schemeCount ] input using i + 4 : xticlabels(1) pointtype i + 1
