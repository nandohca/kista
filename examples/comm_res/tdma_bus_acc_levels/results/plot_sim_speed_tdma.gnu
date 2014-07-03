# Gnuplot script file for plotting data in file "sim_speed_tdma.dat"
# This file is called   plot.gnu
set   autoscale                        # scale axes automatically
unset log                              # remove any log-scaling
unset label                            # remove any previous labels
#set xtic auto                          # set xtics automatically
set ytic auto                          # set ytics automatically
#set title "Simmulation cost for estimating TDMA communication penalties"
set xlabel "Slots allocated"
set ylabel "Host Time (s)"
#set key 0.01,100
#set label "Yield Point" at 0.003,260
#set arrow from 0.0028,250 to 0.003,280
#set xr [0.0:0.022]
#set xr [6:3]
#set yr [0:325]

set style line 1 lc rgb '#006070' lt 1 lw 1 pt 3 pi -1 ps 1
set style line 2 lc rgb '#0060BB' lt 1 lw 1 pt 3 pi -1 ps 1
set style line 3 lc rgb '#0060FF' lt 1 lw 2 pt 1 pi -1 ps 1
set style line 4 lc rgb '#1FB01F' lt 1 lw 1 pt 3 pi -1 ps 1
set style line 5 lc rgb '#000000' lt 1 lw 2 pt 7 pi -1 ps 1
set style line 6 lc rgb '#006600' lt 1 lw 2 pt 7 pi -1 ps 1

set pointintervalbox -3

# For exporting
set term gif
set output "sim_speed_tdma.gif"

plot  "sim_speed_tdma.dat" using 1:4:5 w filledcu title 'bound vs exact trade-off', \
	  "sim_speed_tdma.dat" using 1:2 title 'WCCT(L2)' with linespoints ls 4, \
      "sim_speed_tdma.dat" using 1:3 title 'WCCT(L1)' with linespoints ls 3, \
      "sim_speed_tdma.dat" using 1:4 title 'WCCT(L0)' with linespoints ls 5, \
      "sim_speed_tdma.dat" using 1:5 title 'ACT' with linespoints ls 2,\
      "sim_speed_tdma.dat" using 1:6 title 'BCCT(L0)' with linespoints ls 1

#plot 'sim_speed_tdma.dat' u 1:2:3 w filledcu,       '' u 1:2 lt -1 notitle, '' u 1:3 lt -1 notitle

#plot 'sim_speed_tdma.dat' u 1:2:3 w filledcu,       '' u 1:2 lt -1 title 'BCCT(L0)', '' u 1:3 lt -1 title 'WCCT(L0)'

#plot 'sim_speed_tdma.dat' u 1:2:3 w filledcu title 'ACTR', '' u 1:2 lt -1 title 'BCCT(L0)', '' u 1:3 lt -1 title 'WCCT(L0)', \
#      '' using 1:4 title 'WCCT(L1)' with linespoints ls 3, \
#      '' using 1:5 title 'WCCT(L2)' with linespoints ls 1


#pause mouse
