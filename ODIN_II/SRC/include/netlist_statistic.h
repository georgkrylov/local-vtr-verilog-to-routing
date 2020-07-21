#ifndef NETLIST_STATISTIC_HPP
#define NETLIST_STATISTIC_HPP

#define MULT_OPTIMIZATION_TRAVERSE_VALUE 1337
#define MULT_OPTIMIZATION_WEIGHT_ 1

#include "netlist_utils.h"

stat_t* get_stats(signal_list_t* input_signals, signal_list_t* output_signals, netlist_t* netlist, uintptr_t traverse_mark_number);
stat_t* get_stats(nnode_t** node_list, long long node_count, netlist_t* netlist, uintptr_t traverse_mark_number);
stat_t* get_stats(nnet_t** net_list, long long net_count, netlist_t* netlist, uintptr_t traverse_mark_number);
stat_t* get_stats(nnode_t* node, netlist_t* netlist, uintptr_t traverse_mark_number);
stat_t* get_stats(nnet_t* net, netlist_t* netlist, uintptr_t traverse_mark_number);

stat_t* delete_stat(stat_t* stat);

void init_stat(netlist_t* netlist);
void compute_statistics(netlist_t* netlist, bool display);
int calculate_multiplier_aware_critical_path(nnode_t* node, netlist_t* netlist);

#endif // NETLIST_STATISTIC_HPP
