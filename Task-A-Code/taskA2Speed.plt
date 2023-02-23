set terminal png
set output "TaskA2/Speed/DropRatioSpeed.png" 
set title "DropRatio vs Speed of Nodes"
set xlabel "Speed of Nodes "
set ylabel "DropRatio (%)"
set grid
set datafile missing "-nan"
plot "TaskA2/statsSpeed.txt" using 1:5 with linespoints title "DropRatio"

