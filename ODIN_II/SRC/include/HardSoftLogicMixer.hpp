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
     * of adders should be mixed in the process of synthesis
     */
    bool mixAdders(){ return !_shouldNotOptimizeAtAll && _mixAdders;}

    /* Returns whether the hard blocks and soft logic implementation 
     * of multipliers should be mixed in the process of synthesis
     */
    bool mixMultipliers(){ return !_shouldNotOptimizeAtAll && _mixMultipliers;}
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
     * returns:
     *---------------------------------------------------------------------*/
    void takeNoteOfAMultiply( nnode_t * node);
private:
    /* This function parses the configuration bitmask and sets
     * the corresponding variables.
     */
    void parseAndSetOptimizationParameters(int);

    /* This map holds estimated device sizes that would
     * correspond to the architecture, keyed by the pointers to 
     * layout  description
     */
    std::map<int,std::pair<int,int>> _grid_layout_sizes; 

    std::vector<nnode_t*> _multiplierNodes; 

    // These booleans store devices selected for optimization
    bool _mixMultipliers;
    bool _mixAdders;
  	/* If  mixing soft and hard_logic is set, the possible size of the device 
	 * should be estimated 
	 */  
    OdinGridAnalyzer _analyzer;
    const t_arch& _arch;
    bool _shouldNotOptimizeAtAll;
};

#endif