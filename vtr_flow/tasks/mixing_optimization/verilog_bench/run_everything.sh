#!/bin/bash

prevvar=0.55
for (( i=12; i<=20; i++ ))
do
var=`python -c "print($i/20)"`
sed -i 's/multipliers_mixing_ratio="'$prevvar'"/multipliers_mixing_ratio="'$var'"/g' config/multconfig.xml 
prevvar=$var
/home/george/vtr-verilog-to-routing/vtr_flow/scripts/run_vtr_task.pl -j8 mixing_optimization/verilog_bench
mv run001 $var
done