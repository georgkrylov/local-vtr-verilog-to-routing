#!/bin/bash

prevvar=1
for (( i=20; i<=20; i++ ))
do
var=`python -c "print($i/20)"`
sed -i 's/multipliers_mixing_ratio="'$prevvar'"/multipliers_mixing_ratio="'$var'"/g' config/multconfig.xml 
prevvar=$var
/local_scratch/vtr-verilog-to-routing/vtr_flow/scripts/run_vtr_task.pl -j8 mixing_optimization/or1200_only
mv run001 $var
done
