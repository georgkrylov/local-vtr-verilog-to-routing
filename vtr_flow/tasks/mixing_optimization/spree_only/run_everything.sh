#!/bin/bash

prevvar=0.05
for (( i=1; i<=20; i++ ))
do
var=`python -c "print($i/20)"`
sed -i 's/multipliers_mixing_ratio="'$prevvar'"/multipliers_mixing_ratio="'$var'"/g' config/multconfig.xml 
prevvar=$var
/local_scratch/vtr-verilog-to-routing/vtr_flow/scripts/run_vtr_task.pl -j8 mixing_optimization/spree_only
mv run001 $var
done
