set terminal png
set output "TaskB/Graph/Throughput.png" 
set title "Throughput vs Packets Per Second"
set xlabel "Number of Packets Per Second"
set ylabel "Throughput (Kbps)"
set yrange [200:350]
set grid
set datafile missing "-nan"
plot "TaskB/statsDrop.txt" using 1:2 with linespoints title "TcpFrGfr","TaskB/statsDrop.txt" using 1:3 with linespoints title "TcpNewReno"

