# ./waf --run "scratch/taskA --nFlow=10 --filename=TaskA1/statsFlow.txt"
# ./waf --run "scratch/taskA --nFlow=20 --filename=TaskA1/statsFlow.txt"
# ./waf --run "scratch/taskA --nFlow=30 --filename=TaskA1/statsFlow.txt"
# ./waf --run "scratch/taskA --nFlow=40 --filename=TaskA1/statsFlow.txt"
# ./waf --run "scratch/taskA --nFlow=50 --filename=TaskA1/statsFlow.txt"


# touch TaskA1/Plt/taskAFlow.plt
# Plotter(){

# echo -e "set terminal png
# set output \"TaskA1/Flow/$1Flow.png\" 
# set title \"$1 vs Flow\"
# set xlabel \"Number of Flows\"
# set grid
# set ylabel \"$1 ($3)\"
# set datafile missing \"-nan\"
# plot \"TaskA1/statsFlow.txt\" using 1:$2 with linespoints title \"$1\"
# " > TaskA1/Plt/taskAFlow.plt

# gnuplot TaskA1/Plt/taskAFlow.plt
# }

# Plotter "Throughput" "2" "Kbps"
# Plotter "Delay" "3" "ns"
# Plotter "DeliveryRatio" "4" "%"
# Plotter "DropRatio" "5" "%"



# ./waf --run "scratch/taskA --nPacketsPerSecond=100 --filename=TaskA1/statsPPs.txt"
# ./waf --run "scratch/taskA --nPacketsPerSecond=200 --filename=TaskA1/statsPPs.txt"
# ./waf --run "scratch/taskA --nPacketsPerSecond=300 --filename=TaskA1/statsPPs.txt"
# ./waf --run "scratch/taskA --nPacketsPerSecond=400 --filename=TaskA1/statsPPs.txt"
# ./waf --run "scratch/taskA --nPacketsPerSecond=500 --filename=TaskA1/statsPPs.txt"


# touch TaskA1/Plt/taskAPPS.plt
# Plotter(){

# echo -e "set terminal png
# set output \"TaskA1/PPS/$1PPS.png\" 
# set title \"$1 vs Packets Per Second\"
# set xlabel \"Number of Packets Per Second\"
# set ylabel \"$1 ($3)\"
# set grid
# set yrange [5:10]
# set datafile missing \"-nan\"
# plot \"TaskA1/statsPPs.txt\" using 1:$2 with linespoints title \"$1\"
# " > TaskA1/Plt/taskAPPS.plt

# gnuplot TaskA1/Plt/taskAPPS.plt
# }

# Plotter "Throughput" "2" "Kbps"
# Plotter "Delay" "3" "ns"
# Plotter "DeliveryRatio" "4" "%"
# Plotter "DropRatio" "5" "%"


# ./waf --run "scratch/taskA --range=1 --filename=TaskA1/statsCoverage.txt"
# ./waf --run "scratch/taskA --range=2 --filename=TaskA1/statsCoverage.txt"
# ./waf --run "scratch/taskA --range=3 --filename=TaskA1/statsCoverage.txt"
# ./waf --run "scratch/taskA --range=4 --filename=TaskA1/statsCoverage.txt"
# ./waf --run "scratch/taskA --range=5 --filename=TaskA1/statsCoverage.txt"


# touch TaskA1/Plt/taskARange.plt
# Plotter(){

# echo -e "set terminal png
# set output \"TaskA1/Coverage/$1Range.png\" 
# set title \"$1 vs Coverage Area\"
# set xlabel \"Coverage Area\"
# set ylabel \"$1 ($3)\"
# set grid
# set yrange [0:10]
# set datafile missing \"-nan\"
# plot \"TaskA1/statsPPs.txt\" using 1:$2 with linespoints title \"$1\"
# " > TaskA1/Plt/taskARange.plt

# gnuplot TaskA1/Plt/taskARange.plt
# }

# Plotter "Throughput" "2" "Kbps"
# Plotter "Delay" "3" "ns"
# Plotter "DeliveryRatio" "4" "%"
# Plotter "DropRatio" "5" "%"


#* Nodes
# ./waf --run "scratch/taskA2 --nFlow=20 --nNodes=20 --filename=TaskA2/statsNode.txt"
# ./waf --run "scratch/taskA2 --nFlow=40 --nNodes=40 --filename=TaskA2/statsNode.txt"
# ./waf --run "scratch/taskA2 --nFlow=60 --nNodes=60 --filename=TaskA2/statsNode.txt"
# ./waf --run "scratch/taskA2 --nFlow=80 --nNodes=80 --filename=TaskA2/statsNode.txt"
# ./waf --run "scratch/taskA2 --nFlow=100 --nNodes=100 --filename=TaskA2/statsNode.txt"


# touch TaskA2/Plt/taskA2Node.plt
# Plotter(){

# echo -e "set terminal png
# set output \"TaskA2/Nodes/$1Node.png\" 
# set title \"$1 vs Node\"
# set xlabel \"Number of Nodes\"
# set ylabel \"$1 ($3)\"
# set grid
# set yrange [0:3]
# set datafile missing \"-nan\"
# plot \"TaskA2/statsNode.txt\" using 1:$2 with linespoints title \"$1\"
# " > TaskA2/Plt/taskA2Node.plt

# gnuplot TaskA2/Plt/taskA2Node.plt
# }

# Plotter "Throughput" "2" "Kbps"
# Plotter "Delay" "3" "ns"
# Plotter "DeliveryRatio" "4" "%"
# Plotter "DropRatio" "5" "%"


#* Nodes
# ./waf --run "scratch/taskA2 --nFlow=10 --filename=TaskA2/statsFlow.txt"
# ./waf --run "scratch/taskA2 --nFlow=20 --filename=TaskA2/statsFlow.txt"
# ./waf --run "scratch/taskA2 --nFlow=30 --filename=TaskA2/statsFlow.txt"
# ./waf --run "scratch/taskA2 --nFlow=40 --filename=TaskA2/statsFlow.txt"
# ./waf --run "scratch/taskA2 --nFlow=50 --filename=TaskA2/statsFlow.txt"


# touch TaskA2/Plt/taskA2Flow.plt
# Plotter(){

# echo -e "set terminal png
# set output \"TaskA2/Flow/$1Flow.png\" 
# set title \"$1 vs Flow\"
# set xlabel \"Number of Flow\"
# set ylabel \"$1 ($3)\"
# set grid
# set yrange[0:60]
# set datafile missing \"-nan\"
# plot \"TaskA2/statsFlow.txt\" using 1:$2 with linespoints title \"$1\"
# " > TaskA2/Plt/taskA2Flow.plt

# gnuplot TaskA2/Plt/taskA2Flow.plt
# }

# Plotter "Throughput" "2" "Kbps"
# Plotter "Delay" "3" "ns"
# Plotter "DeliveryRatio" "4" "%"
# Plotter "DropRatio" "5" "%"


# ./waf --run "scratch/taskA2 --nPacketsPerSecond=10 --filename=TaskA2/statsPPS.txt"
# ./waf --run "scratch/taskA2 --nPacketsPerSecond=20 --filename=TaskA2/statsPPS.txt"
# ./waf --run "scratch/taskA2 --nPacketsPerSecond=30 --filename=TaskA2/statsPPS.txt"
# ./waf --run "scratch/taskA2 --nPacketsPerSecond=40 --filename=TaskA2/statsPPS.txt"
# ./waf --run "scratch/taskA2 --nPacketsPerSecond=50 --filename=TaskA2/statsPPS.txt"

# touch TaskA2/Plt/taskA2PPS.plt
# Plotter(){

# echo -e "set terminal png
# set output \"TaskA2/PPS/$1PPS.png\" 
# set title \"$1 vs Packets per second\"
# set xlabel \"Packets per second \"
# set ylabel \"$1 ($3)\"
# set grid
# set datafile missing \"-nan\"
# plot \"TaskA2/statsPPS.txt\" using 1:$2 with linespoints title \"$1\"
# " > TaskA2/Plt/taskA2PPS.plt

# gnuplot TaskA2/Plt/taskA2PPS.plt
# }

# Plotter "Throughput" "2" "Kbps"
# Plotter "Delay" "3" "ns"
# Plotter "DeliveryRatio" "4" "%"
# Plotter "DropRatio" "5" "%"

# ./waf --run "scratch/taskA2 --speed=2 --filename=TaskA2/statsSpeed.txt"
# ./waf --run "scratch/taskA2 --speed=4 --filename=TaskA2/statsSpeed.txt"
# ./waf --run "scratch/taskA2 --speed=6 --filename=TaskA2/statsSpeed.txt"
# ./waf --run "scratch/taskA2 --speed=8 --filename=TaskA2/statsSpeed.txt"
# ./waf --run "scratch/taskA2 --speed=10 --filename=TaskA2/statsSpeed.txt"

touch TaskA2/Plt/taskA2Speed.plt
Plotter(){

echo -e "set terminal png
set output \"TaskA2/Speed/$1Speed.png\" 
set title \"$1 vs Speed of Nodes\"
set xlabel \"Speed of Nodes \"
set ylabel \"$1 ($3)\"
set grid
set datafile missing \"-nan\"
plot \"TaskA2/statsSpeed.txt\" using 1:$2 with linespoints title \"$1\"
" > TaskA2/Plt/taskA2Speed.plt

gnuplot TaskA2/Plt/taskA2Speed.plt
}

Plotter "Throughput" "2" "Kbps"
Plotter "Delay" "3" "ns"
Plotter "DeliveryRatio" "4" "%"
Plotter "DropRatio" "5" "%"
