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

# This script runs a series of parking-lot simulations with different TCP variants
# and varying number of FTP flows. The forward FTP flows are varied from 1 to 100

# set output directory.
New_DIR="."
Original_DIR=`pwd`

# set working DIR
if [ ! -e "$New_DIR/tcp-eval-results" ]; then
    mkdir "$New_DIR/tcp-eval-results"
fi

# set working DIR
if [ ! -e "$New_DIR/tcp-eval-results/parking-lot" ]; then
    mkdir "$New_DIR/tcp-eval-results/parking-lot"
fi

# set working DIR
if [ ! -e "$New_DIR/tcp-eval-results/parking-lot/ftp" ]; then
    mkdir "$New_DIR/tcp-eval-results/parking-lot/ftp"
fi

i=0
while true ; do
    if [ ! -e "$New_DIR/tcp-eval-results/parking-lot/ftp/exp$i" ]; then
        mkdir  "$New_DIR/tcp-eval-results/parking-lot/ftp/exp$i"   
        Working_DIR="$New_DIR/tcp-eval-results/parking-lot/ftp/exp$i"
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
                    ./waf --run "drive-parking-lot --nFwdFtpFlows=$var --tcp_variant=$tcp_var --fileName=$Working_DIR/ftp.$tcp_var"
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
    \title{The TCP Performance Comparison Results for Changing FTP Flows } \maketitle" >>myreport.tex
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


# run a series of parking-lot simulations
run_a_session "1 3 5 7 10 15 20 30 60 100"

# all work done in Working_DIR
cd $Working_DIR

# draw the graph for bottleneck link utilization
draw_eps "utilization.eps" "Link Utilization with FTP Changes" 1 "" 0 100 "FTP Log Scale" "Link Utilization (%)" "right bottom" "utilization.gnuplot" "ftp" 3 4  
   
# draw the graph for mean queue length
draw_eps "queueLength.eps" "Percent of Mean Queue Length with FTP Changes" 1 "" 0 100 "FTP Log Scale" "Mean Queue Length (%)" "right top" "queueLength.gnuplot" "ftp" 3 5

# draw the graph for packet drop rate
draw_eps "dropRate.eps" "Packet Drop Rate with FTP Changes" 1 "" -1 9 "FTP Log Scale" "Packet Drop Rate (%)" "right top" "dropRate.gnuplot" "ftp" 3 6

# generate a pdf file containing the resultant graphs
output_tex "Changing FTP Flows"
echo "\end{document}" >>myreport.tex
latex -no-file-line-error myreport.tex
dvipdf myreport.dvi 
