
# ./waf --run "scratch/CompNetwork --nPacketsPerSecond=200"
# ./waf --run "scratch/CompNetwork --nPacketsPerSecond=400"
# ./waf --run "scratch/CompNetwork --nPacketsPerSecond=600"
# ./waf --run "scratch/CompNetwork --nPacketsPerSecond=800"
# ./waf --run "scratch/CompNetwork --nPacketsPerSecond=1000"


# touch TaskB/Plt/taskB.plt
# Plotter(){

# echo -e "set terminal png
# set output \"TaskB/Graph/$1.png\" 
# set title \"$1 vs Packets Per Second\"
# set xlabel \"Number of Packets Per Second\"
# set ylabel \"$1 ($2)\"
# set yrange [900:1000]
# set grid
# set datafile missing \"-nan\"
# plot \"TaskB/statsTpt.txt\" using 1:2 with linespoints title \"$3\",\
# \"TaskB/statsTpt.txt\" using 1:3 with linespoints title \"$4\"
# " > TaskB/Plt/taskB.plt

# gnuplot TaskB/Plt/taskB.plt
# }

# Plotter "Throughput" "Kbps" "TcpFrGfr" "TcpNewReno"
# Plotter "Delay" "3" "ns"
# Plotter "DeliveryRatio" "4" "%"
# Plotter "DropRatio" "5" "%"

