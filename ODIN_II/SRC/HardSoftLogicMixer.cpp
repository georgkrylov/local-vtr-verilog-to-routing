/*
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */
#include "HardSoftLogicMixer.hpp"

#include <stdint.h> // INT_MAX
#include <vector>

#include "multipliers.h"       // instantiate_simple_soft_multiplier
#include "netlist_statistic.h" // calculate_multiplier_aware_critical_path
#include "odin_error.h"        // error_message

HardSoftLogicMixer::HardSoftLogicMixer(const config_t& config) {
    _allOptsDisabled = true;
    // By default, disables all optimizations
    for (int i = 0; i < mix_hard_blocks::Count; i++) {
        _enabledOptimizations[i] = false;
    }

    parse_opt_parameters(config);
}

void HardSoftLogicMixer::parse_opt_parameters(const config_t config) {
    if (config.mix_soft_and_hard_logic != 0) {
        int check = -1;
        int mix_soft_and_hard_logic = config.mix_soft_and_hard_logic;
        int temp;
        for (int i = 0; i < mix_hard_blocks::Count; i++) {
            temp = 1 << (i);
            check = temp & mix_soft_and_hard_logic;
            if (check != 0) {
                _enabledOptimizations[i] = true;
            }
        }
        _allOptsDisabled = false;
        //Explicitly set all hard block numbers to max
        for (int i = 0; i < mix_hard_blocks::Count; i++) {
            _hardBlocksCount[i] = INT_MAX;
            _hardBlocksMixingRatio[i] = 1.0f;
        }

        // with addition of rest of optimizations, loop across config variables should be
        // implemented here
        _hardBlocksMixingRatio[mix_hard_blocks::MULTIPLIERS] = config.mults_mixing_ratio;

        if (config.mults_mixing_ratio >= 0.0f && config.mults_mixing_ratio <= 1.0f) {
            _enabledOptimizations[mix_hard_blocks::MULTIPLIERS] = true;
            _allOptsDisabled = false;
        }

        if (config.mults_mixing_exact_number_of_multipliers >= 0) {
            _hardBlocksMixingRatio[mix_hard_blocks::MULTIPLIERS] = 1.0f;
            _enabledOptimizations[mix_hard_blocks::MULTIPLIERS] = true;
            _hardBlocksCount[mix_hard_blocks::MULTIPLIERS] = config.mults_mixing_exact_number_of_multipliers;
            _allOptsDisabled = false;
        }
    }
}

void HardSoftLogicMixer::note_candidate_node(nnode_t* opNode, mix_hard_blocks type) {
    _nodes_by_opt[type].push_back({opNode, 0});
}

bool HardSoftLogicMixer::softenable(mix_hard_blocks type) {
    bool result = false;
    if (type != mix_hard_blocks::Count) {
        result = _enabledOptimizations[type];
    }
    return result;
}

void HardSoftLogicMixer::map_deferred_blocks(netlist_t* netlist) {
    if (_allOptsDisabled) {
        return;
    }
    grid_statistics();
    for (int i = 0; i < mix_hard_blocks::Count; i++) {
        switch (i) {
            case mix_hard_blocks::MULTIPLIERS:
                if (_enabledOptimizations[mix_hard_blocks::MULTIPLIERS]) {
                    choose_hard_blocks(netlist, mix_hard_blocks::MULTIPLIERS);
                }
                break;
            default:
                error_message(INC_IMPLEMENTATION, unknown_location, "%s",
                              "Optimization with number: %i does not have an implementation inside of HardSoftLogicMixer::map_deferred_blocks\n", i);
                break;
        }
    }
    soft_map_remaining_nodes(netlist);
}

void HardSoftLogicMixer::grid_statistics() {
    if (true == _allOptsDisabled) {
        return;
    }

    for (int opt = 0; opt < mix_hard_blocks::Count; opt++) {
        if (true == _enabledOptimizations[opt]) {
            // with development for fixed_layout, this value will change
            int availableHardBlocks = INT_MAX;
            int hardBlocksNeeded = hard_blocks_needed(opt);
            int hardBlocksCount = availableHardBlocks;

            if (hardBlocksCount > hardBlocksNeeded) {
                hardBlocksCount = hardBlocksNeeded;
            }

            if (hardBlocksCount < _hardBlocksCount[opt]) {
                _hardBlocksCount[opt] = hardBlocksCount;
            }
        }
    }
    scale_counts();
}

void HardSoftLogicMixer::scale_counts() {
    for (int i = 0; i < mix_hard_blocks::Count; i++) {
        if (_hardBlocksMixingRatio[i] != -1) {
            _hardBlocksCount[i] = _hardBlocksCount[i] * _hardBlocksMixingRatio[i];
        }
    }
}

int HardSoftLogicMixer::hard_blocks_needed(int opt) {
    return _nodes_by_opt[opt].size();
}

void HardSoftLogicMixer::soft_map_remaining_nodes(netlist_t* netlist) {
    for (unsigned int i = 0; i < mix_hard_blocks::Count; i++) {
        for (unsigned int j = 0; j < _nodes_by_opt[i].size(); j++) {
            switch (i) {
                case mix_hard_blocks::MULTIPLIERS:
                    instantiate_simple_soft_multiplier(_nodes_by_opt[i][j].node, PARTIAL_MAP_TRAVERSE_VALUE, netlist);
                    break;
                default:
                    error_message(INC_IMPLEMENTATION, unknown_location, "%s",
                                  "Cleanup for optimization with number: Optimization with number: %i does not have an implementation inside of HardSoftLogicMixer::soft_map_remaining_nodes\n", i);
                    break;
            }
        }
    }
}

void HardSoftLogicMixer::choose_hard_blocks(netlist_t* netlist, mix_hard_blocks type) {
    if (type == mix_hard_blocks::Count) {
        error_message(INC_IMPLEMENTATION, unknown_location, "%s",
                      "Running choseHardBlocks with Count should never happen");
        exit(6);
    }

    std::vector<WeightedNode>& weighted_nodes = _nodes_by_opt[type];
    size_t nodes_count = weighted_nodes.size();

    // compute weights for all noted nodes
    for (size_t i = 0; i < nodes_count; i++) {
        weighted_nodes[i].weight = calculate_multiplier_aware_critical_path(weighted_nodes[i].node, netlist);
    }

    // per optimization, instantiate hard logic
    for (int i = 0; i < _hardBlocksCount[type]; i++) {
        int maximal_cost = -1;
        int index = -1;
        for (size_t j = 0; j < nodes_count; j++) {
            // if found a new maximal cost that is higher than a current maximum AND is not restricted by input
            // params for minimal "hardenable" multiplier width
            if (maximal_cost < weighted_nodes[j].weight && (weighted_nodes[j].node->input_port_sizes[0] > 1) && (weighted_nodes[j].node->input_port_sizes[1] > 1)) {
                maximal_cost = weighted_nodes[j].weight;
                index = j;
            }
        }

        // if there are no suitable nodes left, leave the loop to
        // implement remaining nodes in soft logic
        if (index < 0)
            break;

        // indicate for future iterations the node was hardened
        weighted_nodes[index].weight = -1;

        switch (type) {
            case mix_hard_blocks::MULTIPLIERS: {
                if (hard_multipliers) {
                    instantiate_hard_multiplier(weighted_nodes[index].node, PARTIAL_MAP_TRAVERSE_VALUE, netlist);
                }
                break;
            }
            default:
                error_message(INC_IMPLEMENTATION, unknown_location, "%s",
                              "Implementation of chooseHardBlocks for %i: Hard block type is incomplete", type);
                break;
        }
    }

    // From the end of the vector, remove all nodes that were implemented in hard logic. The remaining
    // nodes will be instantiated in soft_map_remaining_nodes
    for (int i = nodes_count - 1; i >= 0; i--) {
        if (weighted_nodes[i].weight == -1) {
            weighted_nodes.erase(weighted_nodes.begin() + i);
        }
    }
}
