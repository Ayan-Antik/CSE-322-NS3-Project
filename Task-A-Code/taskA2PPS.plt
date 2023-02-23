set terminal png
set output "TaskA2/PPS/DropRatioPPS.png" 
set title "DropRatio vs Packets per second"
set xlabel "Packets per second "
set ylabel "DropRatio (%)"
set grid
set datafile missing "-nan"
plot "TaskA2/statsPPS.txt" using 1:5 with linespoints title "DropRatio"

