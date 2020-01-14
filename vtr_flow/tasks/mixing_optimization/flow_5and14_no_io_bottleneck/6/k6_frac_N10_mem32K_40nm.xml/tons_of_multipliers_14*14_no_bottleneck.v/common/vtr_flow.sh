#!/bin/bash

VTR_RUNTIME_ESTIMATE_SECONDS=0
VTR_MEMORY_ESTIMATE_BYTES=0

VTR_RUNTIME_ESTIMATE_HUMAN_READABLE="0 seconds"
VTR_MEMORY_ESTIMATE_HUMAN_READABLE="0.00 MiB"

#We redirect all command output to both stdout and the log file with 'tee'.

#Begin I/O redirection
{

    /home/george/vtr-verilog-to-routing/vtr_flow/scripts/run_vtr_flow.pl /home/george/vtr-verilog-to-routing/vtr_flow/benchmarks/mix_soft_and_hard_logic//tons_of_multipliers_14*14_no_bottleneck.v /home/george/vtr-verilog-to-routing/vtr_flow/arch/mix_soft_and_hard_logic/k6_frac_N10_mem32K_40nm.xml  -name 'mixing_optimization/flow_5and14_no_io_bottleneck: k6_frac_N10_mem32K_40nm.xml/tons_of_multipliers_14*14_no_bottleneck.v/common'   -temp_dir .  -custom_odin_config_full_name /home/george/vtr-verilog-to-routing/vtr_flow/tasks/mixing_optimization/flow_5and14_no_io_bottleneck/config/multconfig.xml

    #The IO redirection occurs in a sub-shell,
    #so we need to exit it with the correct code
    exit $?

} |& tee vtr_flow.out
#End I/O redirection

#We used a pipe to redirect IO.
#To get the correct exit status we need to exit with the
#status of the first element in the pipeline (i.e. the real
#command run above)
exit ${PIPESTATUS[0]}
