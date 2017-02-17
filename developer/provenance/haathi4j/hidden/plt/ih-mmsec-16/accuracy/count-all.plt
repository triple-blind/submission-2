#terminal = "pngcairo"
#output = "count-all.png"

if (!exists("input")) input = "acc-all.dat"

set datafile missing "NaN"
stats input using 2 nooutput

if (!exists("fileCount")) {
	fileCount = STATS_blocks
}

if (!exists("schemeCount")) {
	schemeCount = (STATS_records + STATS_invalid) / STATS_blocks
}

if (exists("terminal")) {
	set terminal terminal mono solid
}
else {
	set terminal windows monochrome solid
}

if (exists("output")) set output output

set title "Watermark Counts for Combined Schemes"
set style data linespoints
set key autotitle columnhead left bottom

set ylabel "Watermarks"
set logscale y
set yrange [ 0.1 : 10000 ]
set ytics 10
set grid ytics

set xlabel "Watermark Scheme"
set xrange [ -0.5 : schemeCount - 0.5 ]

plot for [ i = 1 : fileCount ] input using 2 : xticlabels(1) index i - 1
