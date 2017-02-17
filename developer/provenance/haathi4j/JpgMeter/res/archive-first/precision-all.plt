#terminal = "pngcairo"
#output = "precision-all.png"

if (!exists("input")) input = "prc-all-manual.dat"

set datafile missing "NaN"
stats input using 3 nooutput

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

set title "Watermark Precision Across Visibilities for Combined Schemes"
set style data linespoints
set key autotitle columnhead left bottom

set ylabel "Precision (%)"
set yrange [ 0 : 110 ]
set ytics 10
set grid ytics

set xlabel "Watermark Scheme"
set xrange [ -0.5 : schemeCount - 0.5 ]

plot for [ i = 1 : fileCount ] input using 3 : xticlabels(1) index i - 1
