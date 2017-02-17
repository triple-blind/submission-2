#terminal = "png"
if (!exists("input")) input = "test.dat"
#output = "test.png"

if (exists("terminal")) set terminal terminal
if (exists("output")) set output output

unset key
set yrange [0:5]
set style fill solid
set boxwidth 0.5
set title "Keith was here"
set xlabel "Watermark Scheme"
set ylabel "Time to Load (seconds)"
plot input using 2:xticlabels(1) with boxes
