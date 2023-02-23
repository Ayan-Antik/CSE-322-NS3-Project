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
# ./waf --run "scratch/taskA --nFlow=20 --nNodes=20 --filename=TaskA1/statsNode.txt"
# ./waf --run "scratch/taskA --nFlow=40 --nNodes=40 --filename=TaskA1/statsNode.txt"
# ./waf --run "scratch/taskA --nFlow=60 --nNodes=60 --filename=TaskA1/statsNode.txt"
# ./waf --run "scratch/taskA --nFlow=80 --nNodes=80 --filename=TaskA1/statsNode.txt"
# ./waf --run "scratch/taskA --nFlow=100 --nNodes=100 --filename=TaskA1/statsNode.txt"


# touch TaskA1/Plt/taskANode.plt
# Plotter(){

# echo -e "set terminal png
# set output \"TaskA1/Nodes/$1Node.png\" 
# set title \"$1 vs Node\"
# set xlabel \"Number of Nodes\"
# set ylabel \"$1 ($3)\"
# set grid
# set datafile missing \"-nan\"
# plot \"TaskA1/statsNode.txt\" using 1:$2 with linespoints title \"$1\"
# " > TaskA1/Plt/taskANode.plt

# gnuplot TaskA1/Plt/taskANode.plt
# }

# Plotter "Throughput" "2" "Kbps"
# Plotter "Delay" "3" "ns"
# Plotter "DeliveryRatio" "4" "%"
# Plotter "DropRatio" "5" "%"