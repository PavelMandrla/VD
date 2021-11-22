set terminal pdfcairo enhanced color font "sans,6" fontscale 1.0 linewidth 1 rounded dashlength 1 background "white" #size 10cm,6cm
set encoding utf8

set output "weight.pdf"

#definovani barvy os
set style line 80 linetype 1 linecolor "#808080" linewidth 2
set border 3 back linestyle 80

#mrizka
set style line 81 lt 0
set style line 81 lt rgb "0x00D3D3D3"
set style line 81 lw 0.5
set grid back linestyle 81

set xtics nomirror
set ytics nomirror

set title "Váha Pavla Mandrly během let v období od 1. 1. 2020 do 31. 12. 2021" font "sans-Bold"
set xlabel "Datum"
set ylabel "Váha (kg)"

#set key outside bottom right Left title "Probes:" enhanced font "sans-Italic" #reverse box

#set xrange [1:10]
set yrange [80:120]

set xdata time
set timefmt "%Y-%m-%d"
set format x "%d.%m.%Y"
set xtics rotate by -90

set xrange ["2020-01-01":"2021-12-31"]

set arrow from "2020-01-01",86.49 to "2021-12-31",86.49 filled back dt 3 lw 1 lc rgb "orange" nohead
set arrow from "2020-01-01",103.788 to "2021-12-31",103.788 filled back dt 3 lw 1 lc rgb "red" nohead

set label "BMI 25 (nadváha)" at "2021-07z-01",86.49
set label "BMI 30 (obezita)" at "2021-07-01",103.788

set style line 1 lt rgb "#A00000" lw 2 pt 0

f(x) = a + b*x
#fit f(x) "data_processed.csv" index 0 using 1:2 via a,b#,c,d

# Need full using spec for x time data -> missing using 1:2
set datafile separator ","

plot "data_processed.csv" index 0 using 1:2 w lp ls 1 title "váha Pavla Mandrly" #, \
  #"data_processed.csv" index 1 using 1:2 title "Inside" with lines#, f(x) title "Inside approx."

pause -1
