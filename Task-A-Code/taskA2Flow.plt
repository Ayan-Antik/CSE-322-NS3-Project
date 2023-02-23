set terminal png
set output "TaskA2/Flow/DropRatioFlow.png" 
set title "DropRatio vs Flow"
set xlabel "Number of Flow"
set ylabel "DropRatio (%)"
set grid
set yrange[0:60]
set datafile missing "-nan"
plot "TaskA2/statsFlow.txt" using 1:5 with linespoints title "DropRatio"

