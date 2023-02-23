set terminal png
set output "TaskA1/PPS/DropRatioPPS.png" 
set title "DropRatio vs Packets Per Second"
set xlabel "Number of Packets Per Second"
set ylabel "DropRatio (%)"
set grid
set yrange [5:10]
set datafile missing "-nan"
plot "TaskA1/statsPPs.txt" using 1:5 with linespoints title "DropRatio"

