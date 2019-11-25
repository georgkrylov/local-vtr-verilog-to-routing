/*
Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef HARD_SOFT_LOGIC_MIXER_HPP
#define HARD_SOFT_LOGIC_MIXER_HPP

#include "physical_types.h"
#include "odin_types.h"
#include <utility>      // std::pair, std::make_pair
#include <map>
#include <vector>
#include "OdinGridAnalyzer.hpp"
#include "multipliers.h"

class HardSoftLogicMixer{
public:
    HardSoftLogicMixer(const t_arch& arch,int);
    /*----------------------------------------------------------------------
    * Function: calculateGridSizes 
    *   For all the layouts represented in the architecture file, populates
    *   the _grid_layout_sizes map with corresponding estimated sizes
    *---------------------------------------------------------------------*/
    void calculateAllGridSizes();

    /* Returns whether the hard blocks and soft logic implementation 
     * of multipliers should be mixed in the process of synthesis
     */
    bool mixMultipliers(){ return _enabledOptimizations[HardBlocksOptimizationTypesEnum::MULTIPLIERS] ;}
    /*----------------------------------------------------------------------
     * Function: selectLogicToImplementInHardBlocks 
     * Calculates number of available hard blocks by issuing a call,
     * traverses the netlist and statistics to figure out
     * which operation should be implemented on the hard block
     * Parameters: 
     * returns:
     *---------------------------------------------------------------------*/
    void selectLogicToImplementInHardBlocks(netlist_t *netlist);

    /*----------------------------------------------------------------------
     * Function: selectLogicToImplementInHardBlocks 
     * Calculates number of available hard blocks by issuing a call,
     * traverses the netlist and statistics to figure out
     * which operation should be implemented on the hard block
     * Parameters: 
     *      node_t * : pointer to candidate node
     *      HardBlocksOptimizationTypesEnum : type of the hard block to optimize
     * returns:
     *---------------------------------------------------------------------*/
    void takeNoteOfAPotentialHardBlockNode( nnode_t * node, HardBlocksOptimizationTypesEnum type);
private:
    /* This function parses the configuration bitmask and sets
     * the corresponding boolean variables in the _enabledOptimizationsArray.
     */
    void parseAndSetOptimizationParameters(int);

    /* This map holds estimated device sizes that would
     * correspond to the architecture, keyed by the pointers to 
     * layout  description
     */
    std::map<int,std::pair<int,int>> _grid_layout_sizes; 

    // This array is composed of vectors, that store nodes that
    // are potential candidates for performing mixing optimization
    std::vector<nnode_t*>potentialHardBlockNodes[HardBlocksOptimizationTypesEnum::Count];

    // These booleans store devices selected for optimization
    bool _allOptsDisabled ;
    bool _enabledOptimizations[HardBlocksOptimizationTypesEnum::Count];
  	/* If  mixing soft and hard_logic is set, the possible size of the device 
	 * should be estimated 
	 */  
    OdinGridAnalyzer _analyzer;
    const t_arch& _arch;
};

#endif