set terminal png
set output "TaskA1/Coverage/DropRatioRange.png" 
set title "DropRatio vs Coverage Area"
set xlabel "Coverage Area"
set ylabel "DropRatio (%)"
set grid
set yrange [0:10]
set datafile missing "-nan"
plot "TaskA1/statsPPs.txt" using 1:5 with linespoints title "DropRatio"

