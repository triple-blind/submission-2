terminal = "pngcairo"
output = "type-all.png"

if (!exists("input")) input = "spd-all.dat"

set datafile missing "-"
stats input using 3 nooutput

if (!exists("schemeCount")) {
	schemeCount = STATS_blocks
}

if (!exists("fileCount")) {
	fileCount = (STATS_records + STATS_invalid) / STATS_blocks
}

if (exists("terminal")) {
	set terminal terminal mono solid
}
else {
	set terminal windows monochrome solid
}

if (exists("output")) set output output

#set title "Document Type Performance for Multiple Schemes"
set style data linespoints
set key autotitle columnhead left top

set ylabel "Rate (words per minute, WPM)"
set yrange [ 0 : 300 ]
set ytics 50
set grid ytics

set xlabel "Document"
set xrange [ -0.5 : fileCount - 0.5 ]

plot for [ i = 1 : schemeCount ] input using 3 : xticlabels(1) index i - 1 pointtype i + 2
