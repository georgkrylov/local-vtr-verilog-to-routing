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
    _candidate_nodes[type].emplace_back(opNode);
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
    return _candidate_nodes[opt].size();
}

void HardSoftLogicMixer::soft_map_remaining_nodes(netlist_t* netlist) {
    for (unsigned int i = 0; i < mix_hard_blocks::Count; i++) {
        for (unsigned int j = 0; j < _candidate_nodes[i].size(); j++) {
            switch (i) {
                case mix_hard_blocks::MULTIPLIERS:
                    instantiate_simple_soft_multiplier(_candidate_nodes[i][j], PARTIAL_MAP_TRAVERSE_VALUE, netlist);
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
    std::vector<nnode_t*>& nodesVector = _candidate_nodes[type];
    int nodesCount = nodesVector.size();
    int* costs = new int[nodesCount];
    for (int i = 0; i < nodesCount; i++) {
        costs[i] = calculate_multiplier_aware_critical_path(nodesVector[i], netlist);
    }

    int numberOfMultipliers = _hardBlocksCount[mix_hard_blocks::MULTIPLIERS];
    for (int i = 0; i < numberOfMultipliers; i++) {
        int maximalCost = -1;
        int indexOfMaximum = -1;
        for (int j = 0; j < nodesCount; j++) {
            if (maximalCost < costs[j] && (nodesVector[j]->input_port_sizes[0] > 1) && (nodesVector[j]->input_port_sizes[1] > 1)) {
                maximalCost = costs[j];
                indexOfMaximum = j;
            }
        }

        if (indexOfMaximum < 0)
            break;

        costs[indexOfMaximum] = -1;
        switch (type) {
            case mix_hard_blocks::MULTIPLIERS: {
                if (hard_multipliers) {
                    instantiate_hard_multiplier(nodesVector[indexOfMaximum], PARTIAL_MAP_TRAVERSE_VALUE, netlist);
                }
                break;
            }
            default:
                error_message(INC_IMPLEMENTATION, unknown_location, "%s",
                              "Implementation of chooseHardBlocks for %i: Hard block type is incomplete", type);
                break;
        }
    }

    for (int i = nodesCount - 1; i >= 0; i--) {
        if (costs[i] == -1) {
            nodesVector.erase(nodesVector.begin() + i);
        }
    }
    delete[] costs;
}
