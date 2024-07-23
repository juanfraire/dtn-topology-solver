set terminal svg size 1500,1000
set output "TotSim_01_arcDistr3D.svg"
set key
set datafile separator ","
set xlabel "Arc (i-j)"
set ylabel "K State"
set zlabel "Time"
set view 29,53
set hidden3d
set xyplane 0
splot 'TotSim_01_arcDistr3D.csv' using 1:3:4:xticlabels(2) with pm3d title "TotSim_01"
