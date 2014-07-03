clear
reset
unset key
# Make the x axis labels easier to read.
set xtics rotate out
# Select histogram data
set style data histogram
# Give the bars a plain fill pattern, and draw a solid line around them.
set style fill solid border

#set style histogram clustered
#plot for [COL=2:4] 'date_mins.tsv' using COL:xticlabels(1) title columnheader

set term gif
set output "times_f1.gif"

plot 'times_f1.dat' using 2:xticlabels(1)

#pause mouse
