#!/bin/bash

# /*
#  * Copyright (c) 2016 NITK, Surathkal
#  *
#  * This program is free software; you can redistribute it and/or modify
#  * it under the terms of the GNU General Public License version 2 as
#  * published by the Free Software Foundation;
#  *
#  * This program is distributed in the hope that it will be useful,
#  * but WITHOUT ANY WARRANTY; without even the implied warranty of
#  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  * GNU General Public License for more details.
#  *
#  * You should have received a copy of the GNU General Public License
#  * along with this program; if not, write to the Free Software
#  * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#  *
#  * Authors: Mohit P. Tahiliani <tahiliani@nitk.edu.in>
#  *
#  */

# This script runs a series of dumbbell simulations with different 
# TCP variants and varying Round Trip Time(RTT). The RTT values are 
# varied from 0.01 second to 1 second

# set output directory.
New_DIR="."
Original_DIR=`pwd`

# set working DIR
if [ ! -e "$New_DIR/tcp-eval-results" ]; then
    mkdir "$New_DIR/tcp-eval-results"
fi

# set working DIR
if [ ! -e "$New_DIR/tcp-eval-results/dumbbell" ]; then
    mkdir "$New_DIR/tcp-eval-results/dumbbell"
fi

# set working DIR
if [ ! -e "$New_DIR/tcp-eval-results/dumbbell/rtt" ]; then
    mkdir "$New_DIR/tcp-eval-results/dumbbell/rtt"
fi

i=0
while true ; do
    if [ ! -e "$New_DIR/tcp-eval-results/dumbbell/rtt/exp$i" ]; then
        mkdir  "$New_DIR/tcp-eval-results/dumbbell/rtt/exp$i"   
        Working_DIR="$New_DIR/tcp-eval-results/dumbbell/rtt/exp$i"
        break
    fi
    let "i=i+1"
done

# run a parameter change session
# params are changing variable, scope and output file.
run_a_session ( ) {

    for tcp_var in TcpTahoe TcpReno TcpNewReno TcpWestwood TcpWestwoodPlus
        do
            for var in $1  
                do
                    ./waf --run "drive-dumbbell --rttp=$var --tcp_variant=$tcp_var --fileName=$Working_DIR/rttp.$tcp_var"
            done
    done
}

# dwaw an eps graph
# params are output_eps_filename, title, x_range, y_range, x_label, y_label, key, gnuplot_file, input_data_file, column_x, column_y
function draw_eps ( ) {
    echo "set terminal postscript eps color
    set output \"$1\"
    set title \"$2\"
    set xrange [$3:$4]
    set yrange [$5:$6]
    set xlabel \"$7\"
    set logscale x
    set ylabel \"$8\"
    set key $9
    set grid " >> ${10}
    echo -n "plot " >>${10}

    echo -n \'./${11}.TcpTahoe\' using ${12}:${13} title \'Tahoe\' with lp, >>${10}
    echo -n \'./${11}.TcpReno\' using ${12}:${13} title \'Reno\' with lp, >>${10}
    echo -n \'./${11}.TcpNewReno\' using ${12}:${13} title \'Newreno\' with lp, >>${10}
    echo -n \'./${11}.TcpWestwood\' using ${12}:${13} title \'Westwood\' with lp, >>${10}
    echo -n \'./${11}.TcpWestwoodPlus\' using ${12}:${13} title \'Westwood+\' with lp, >>${10}

    gnuplot ${10}
}

# generate tex files
# params are section name, graph name.
function output_tex () {
    echo "\documentclass[11pt]{article}
    \usepackage{fullpage,latexsym,graphicx, subfigure}
    \begin{document}
    \title{The TCP Performance Comparison Results for Changing RTT } \maketitle" >>myreport.tex
    echo "\section{$1}
    \begin{figure} [htbp]
    \begin{center}
    \includegraphics[width=2.5in]{utilization.eps}
    \includegraphics[width=2.5in]{queueLength.eps}
    \includegraphics[width=2.5in]{dropRate.eps}
    \end{center}
    \caption{Utilization, Queue length and Drop rate with $1} \label{$1}
    \end{figure} " >> myreport.tex
}

# run a series of dumbbell simulations
run_a_session "0.01 0.02 0.05 0.08 0.10 0.16 0.20 0.30 0.40 0.60 0.80 1.0"

# all work done in Working_DIR
cd $Working_DIR

# draw the graph for bottleneck link utilization
draw_eps "utilization.eps" "Link Utilization with RTT Changes" 0.01 "" 0.1 105 "RTT (s) Log Scale" "Link Utilization (%)" "right bottom" "utilization.gnuplot" "rttp" 2 4
     
# draw the graph for mean queue length
draw_eps "queueLength.eps" "Percent of Mean Queue Length with RTT Changes" 0.01 "" 0.1 100 "RTT (s) Log Scale" "Mean Queue Length (%)" "right top" "queueLength.gnuplot" "rttp" 2 5

# draw the graph for packet drop rate
draw_eps "dropRate.eps" "Packet Drop Rate with RTT Changes" 0.01 "" -1 9 "RTT (s) Log Scale" "Packet Drop Rate (%)" "right top" "dropRate.gnuplot" "rttp" 2 6

# generate a pdf file containing the resultant graphs
output_tex "Changing RTT"
echo "\end{document}" >>myreport.tex
latex -no-file-line-error myreport.tex
dvipdf myreport.dvi 

