#!/bin/bash
VTR_ROOT=~/vtr-verilog-to-routing/
TASK_PATH=`pwd`
TASK_NAME=${TASK_PATH%"config"}
TASK_NAME=${TASK_NAME##*"tasks/"}

rm -rf 0.*
rm -rf 1.0
prevvar=0.05
for (( i=20; i<=20; i++ ))
do
var=`python -c "print($i/20)"`
sed -i 's/multipliers_mixing_ratio="'$prevvar'"/multipliers_mixing_ratio="'$var'"/g' config/multconfig.xml 
prevvar=$var
$VTR_ROOT/vtr_flow/scripts/run_vtr_task.pl -j8 $TASK_NAME
mv run001 $var
here=`pwd`
cd $var
for elem in $(find . -mindepth 3  -maxdepth 3 -type d)
do
    cd $elem
    cp -r simulation_init simulation_use_this
    cp odin_config.xml no_opt_config.xml
    sed -i 's/odin.blif/odin.second_run.blif/g' ./no_opt_config.xml
    sed '/mix_soft_hard_blocks/d' -i ./no_opt_config.xml
    $VTR_ROOT/ODIN_II/odin_II -c ./no_opt_config.xml -t ./simulation_use_this/input_vectors -T ./simulation_use_this/output_vectors &> custom_sim_res.out
    echo "==================================================="
    echo "Disregard previous message about failed simulation"
    echo "The result is"
    echo "==================================================="
    grep "matches output"  custom_sim_res.out
    echo "==================================================="
    echo "Details are available in"
    NEW_PATH=`pwd`
    echo "$NEW_PATH""/custom_sim_res.out"
    cd ../../..
done
cd $here
done
