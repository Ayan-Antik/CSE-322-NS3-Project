set terminal png
set output "TaskA1/Flow/DropRatioFlow.png" 
set title "DropRatio vs Flow"
set xlabel "Number of Flows"
set grid
set ylabel "DropRatio (%)"
set datafile missing "-nan"
plot "TaskA1/statsFlow.txt" using 1:5 with linespoints title "DropRatio"

