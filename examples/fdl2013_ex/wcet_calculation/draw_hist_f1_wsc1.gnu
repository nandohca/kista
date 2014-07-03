MyBlue="#91BEF9"
DarkMyBlue="#0549A1"
clear
reset
unset key
# Make the x axis labels easier to read.
set xtics rotate out
# Select histogram data
set style data histogram
# Give the bars a plain fill pattern, and draw a solid line around them.
set style fill solid border

set term gif
set output "times_f1_sc1.gif"

#plot 'times_f1.dat' using 2:xticlabels(1)
set style histogram clustered
#set auto y
#set auto y2
set yrange[0:5000000]
set y2range[0:10000000]

#set ytics 10 nomirror tc lt 1
#set ylabel '2*x'  lt 1
#set y2tics 20 nomirror tc lt 2
#set y2label '4*x' tc lt 2

set ytics 1E6 nomirror tc lt 1
#set ylabel 'full random' tc lt 1

set y2tics 1E6 nomirror tc lt 2
#set y2tics 1E6 nomirror tc rgb DarkMyBlue
#set y2label 'scenario' tc rgb DarkMyBlue

#plot for [COL=2:3] 'times_f3_sc1.dat' using COL:xticlabels(1) title columnheader
#plot for [COL=2:3] 'times_f3_sc1.dat' using COL:xticlabels(1) axes x1y2
#plot for [COL=2:3] 'times_f3_sc1.dat' using COL:xticlabels(1) COL:lt(2) 1 COL:lt(3) 2 axes x1y2
# Since it gives problems, by now, we scale manually column 3
plot for [COL=2:3] 'times_f1_sc1_scaled.dat' using COL:xticlabels(1) axes x1y1

#plot for [COL=2:3] 'times_f1_sc1_scaled.dat' using COL:xticlabels(1) ti col fc rgb "#FF0000", '' using 3 ti col fc rgb "#FF0000" axes x1y1

#plot 'times_f1_sc1_scaled.dat' using 2:xticlabels(1) ti col fc rgb "#FF0000", '' using 3 ti col fc rgb MyBlue  axes x1y1

#set label "partially-ordered input" at 342,3500
#set arrow from 0.0028,250 to 0.003,280

#pause mouse
