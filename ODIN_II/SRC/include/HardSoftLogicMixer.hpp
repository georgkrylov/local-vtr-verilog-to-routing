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

#include "physical_types.h"
#include "odin_types.h"
#include <utility> // std::pair, std::make_pair
#include <map>
#include <vector>
#include "OdinGridAnalyzer.hpp"
#include "multipliers.h"
#include "netlist_analyzer.h"

class HardSoftLogicMixer {
  public:
    HardSoftLogicMixer(t_arch& arch, const config_t configuration, std::vector<t_physical_tile_type> tileTypes);
    /*----------------------------------------------------------------------
     * Function: calculateGridSizes 
     *   For all the layouts represented in the architecture file, populates
     *   the _grid_layout_sicalculateAllGridSizeszes map with corresponding estimated sizes
     *---------------------------------------------------------------------*/
    void grid_statistics();

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
    /* This function parses the configuration bitmask and sets
     * the corresponding boolean variables in the _enabledOptimizationsArray.
     */
    void parse_opt_parameters(const config_t);
    int hard_blocks_arch(t_grid_def& layout, int hardBlockType, std::pair<int, int> size);
    void scale_counts();
    /* After done mapping logic nodes onto hard blocks, 
     * this function should be called to assure remaining blocks
     * are assigned to be implemented in soft logic
     */
    void soft_map_remaining_nodes(netlist_t* netlist);
    int hard_blocks_needed(int currentOptimizationKind);
    void choose_hard_blocks(netlist_t* netlist, mix_hard_blocks type);
    /* This map holds estimated device sizes that would
     * correspond to the architecture, keyed by the pointers to 
     * layout  description
     */
    std::map<int, std::pair<int, int>> _grid_layout_sizes;

    // This array is composed of vectors, that store nodes that
    // are potential candidates for performing mixing optimization
    std::vector<nnode_t*> _candidate_nodes[mix_hard_blocks::Count];
    int _hardBlocksCount[mix_hard_blocks::Count];
    // These booleans store devices selected for optimization
    bool _allOptsDisabled;
    bool _enabledOptimizations[mix_hard_blocks::Count];
    /* If  mixing soft and hard_logic is set, the possible size of the device 
     * should be estimated 
     */
    OdinGridAnalyzer _analyzer;
    float _hardBlocksMixingRatio[mix_hard_blocks::Count];
    t_arch& _arch;
    std::vector<t_physical_tile_type> _tileTypes;
};

#endif