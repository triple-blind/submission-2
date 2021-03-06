#terminal = "pngcairo"
#output = "save-one.png"

if (!exists("input")) input = "spd-one.dat"

set datafile missing "-"
stats input using 4 nooutput

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

set title "Document Save Performance for Single Schemes"
set style data linespoints
set key autotitle columnhead left bottom

set ylabel "Time (sec.)"
set yrange [ 0 : 80 ]

set xlabel "Document"
set xrange [ -0.5 : fileCount - 0.5 ]

plot for [ i = 1 : schemeCount ] input using 4 : xticlabels(1) index i - 1 pointtype i + 2
