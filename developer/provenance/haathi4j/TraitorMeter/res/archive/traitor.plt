terminal = "pngcairo"
output = "traitor.png"

if (!exists("input")) input = "tt.dat"

if (exists("terminal")) {
	set terminal terminal mono solid
}
else {
	set terminal windows monochrome solid
}

if (exists("output")) set output output

#set title "Traitor Tracing Performance"
set style data linespoints
set grid
set key autotitle columnhead left top

set logscale xy

set ylabel "Time (sec.)"
set yrange [ 0.01 : 1000 ]

set xlabel "Plausible (user, document) Pairs"
set xrange [ 0.5 : 1000 ]

plot for [ i = 1 : 7 ] input using 4 : 5 index i - 1 pointtype i + 1