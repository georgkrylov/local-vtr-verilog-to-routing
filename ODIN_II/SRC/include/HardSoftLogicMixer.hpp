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

#ifndef HARD_SOFT_LOGIC_MIXER_HPP
#define HARD_SOFT_LOGIC_MIXER_HPP

#include "odin_types.h" // mix_hard_blocks, config_t

class HardSoftLogicMixer {
  public:
    HardSoftLogicMixer(const config_t& configuration);

    /* Returns whether the hard blocks and soft logic implementation 
     * of multipliers should be mixed in the process of synthesis
     */
    bool softenable(mix_hard_blocks type);
    /*----------------------------------------------------------------------
     * Function: map_deferred_blocks 
     * For all  noted nodes, that were noted as candidates to be implemented 
     * on the hard blocks, launches corresponding procedure of chosing the
     * corresponding blocks
     * Parameters: netlist_t *
     * returns:
     *---------------------------------------------------------------------*/
    void map_deferred_blocks(netlist_t* netlist);

    /*----------------------------------------------------------------------
     * Function: note_candidate_node
     * Calculates number of available hard blocks by issuing a call,
     * traverses the netlist and statistics to figure out
     * which operation should be implemented on the hard block
     * Parameters: 
     *      node_t * : pointer to candidate node
     *      mix_hard_blocks : type of the hard block to optimize
     * returns:
     *---------------------------------------------------------------------*/
    void note_candidate_node(nnode_t* node, mix_hard_blocks type);

  private:
    /* ----------------------------------------------------------------------
     * Function parse_opt_parameters
     * This function parses the configuration bitmask and sets
     * the corresponding boolean variables in the _enabledOptimizationsArray.
     *----------------------------------------------------------------------*/
    void parse_opt_parameters(const config_t);

    /*----------------------------------------------------------------------
     * Function: grid_statistics
     *   For all the layouts represented in the architecture file, populates
     *   array of all optimizations with estimated numbers for hard blocks
     *---------------------------------------------------------------------*/
    void grid_statistics();

    /*----------------------------------------------------------------------
     * Function: scale_counts
     * Applies parameter scaling to the count of hard blocks
     *---------------------------------------------------------------------*/
    void scale_counts();

    /* ----------------------------------------------------------------------
     * Function: soft_map_remaining_nodes
     * After finishing mapping logic nodes onto hard blocks, 
     * this function should be called to assure remaining blocks
     * are assigned to be implemented in soft logic
     *---------------------------------------------------------------------*/
    void soft_map_remaining_nodes(netlist_t* netlist);

    /*----------------------------------------------------------------------
     * Function: hard_blocks_needed
     * Returns cached value calculated from netlist, for a specific optimiza
     * tion kind
     *---------------------------------------------------------------------*/
    int hard_blocks_needed(int currentOptimizationKind);

    /*----------------------------------------------------------------------
     * Function: choose_hard_blocks
     * Functiion selects and instantiates operations on hard blocks for a 
     * given optimization kind
     *---------------------------------------------------------------------*/
    void choose_hard_blocks(netlist_t* netlist, mix_hard_blocks type);

    // This array is composed of vectors, that store nodes that
    // are potential candidates for performing mixing optimization
    std::vector<nnode_t*> _candidate_nodes[mix_hard_blocks::Count];

    // The array contains estimated size of the hard blocks
    int _hardBlocksCount[mix_hard_blocks::Count];
    // These booleans store devices selected for optimization
    bool _allOptsDisabled;
    bool _enabledOptimizations[mix_hard_blocks::Count];
    float _hardBlocksMixingRatio[mix_hard_blocks::Count];
};

#endif
