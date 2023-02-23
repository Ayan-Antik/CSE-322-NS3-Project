set terminal png
set output "TaskA1/Nodes/ThroughputNode.png" 
set title "Throughput vs Node"
set xlabel "Number of Nodes"
set ylabel "Throughput (Kbps)"
set grid
set yrange [950:1000]
set datafile missing "-nan"
plot "TaskA1/statsNode.txt" using 1:2 with linespoints title "Throughput"

