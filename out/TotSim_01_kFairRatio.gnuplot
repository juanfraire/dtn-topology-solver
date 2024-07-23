set terminal svg size 1280,480
set output "TotSim_01_kFairRatio.svg"
set key left top
set datafile separator ","
set xlabel "State [k]"
set ylabel "Ratio"
set style line 1 lc rgb '#0025ad' pt 5 lt 1 lw 2 # --- blue
set style line 2 lc rgb '#00ad88' pt 7 lt 1 lw 2 # --- cyan
set style line 3 lc rgb '#09ad00' pt 9 lt 1 lw 2 # --- green
set style line 4 lc rgb '#ba55cc' pt 13 lt 1 lw 2 # --- violet
set style line 5 lc rgb '#ff8000' pt 15 lt 1 lw 2 # --- orange
set style line 11 lc rgb '#808080' lt 1 
set border 3 back ls 11
set tics nomirror
set style line 12 lc rgb '#808080' lt 0 lw 1
set grid back ls 12
set xtics rotate by -90 left
set yrange [0:]
plot 'TotSim_01_kFairRatio.csv' using 2:xticlabels(1) with linespoints ls 1 title "TotSim_01_Raj_Jain", 'TotSim_01_kFairRatio.csv' using 3:xticlabels(1) with linespoints ls 2 title "TotSim_01_MinMax"
