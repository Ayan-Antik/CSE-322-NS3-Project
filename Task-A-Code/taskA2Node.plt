set terminal png
set output "TaskA2/Nodes/ThroughputNode.png" 
set title "Throughput vs Node"
set xlabel "Number of Nodes"
set ylabel "Throughput (Kbps)"
set grid
set yrange [0:3]
set datafile missing "-nan"
plot "TaskA2/statsNode.txt" using 1:2 with linespoints title "Throughput"

