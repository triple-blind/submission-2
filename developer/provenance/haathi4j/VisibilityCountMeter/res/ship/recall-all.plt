terminal = "pngcairo"
output = "recall-all.png"

if (!exists("input")) input = "prc-all.dat"

set datafile missing "0"
stats input using 4 nooutput

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

#set title "Watermark Recall Across Visibilities for Combined Schemes"
set style data linespoints
set key autotitle columnhead left bottom

set ylabel "Average Recalled Watermarks per Full Page"
set yrange [ 0.1 : 1000 ]
#set ytics 10
set grid ytics
set logscale y

set xlabel "Watermark Scheme"
set xrange [ -1.0 : schemeCount - 0.5 ]

plot for [ i = 1 : fileCount ] input using ($4 / $2) : xticlabels(1) index i - 1 pointtype i + 2
