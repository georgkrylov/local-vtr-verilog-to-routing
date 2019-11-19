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
#include <utility>      // std::pair, std::make_pair
#include <map>

class OdinGridAnalyzer;

class HardSoftLogicMixer{
public:
    HardSoftLogicMixer(const t_arch& arch,int);
    ~HardSoftLogicMixer();

    void calculateGridSizes();


    /* Returns whether the hard blocks and soft logic implementation 
     * of adders should be mixed in the process of synthesis
     */

    bool mixAdders(){ return _mixAdders;}

    /* Returns whether the hard blocks and soft logic implementation 
     * of multipliers should be mixed in the process of synthesis
     */
    bool mixMultipliers(){ return _mixMultipliers;}
    
private:
    /* This function parses the configuration bitmask and sets
     * the corresponding variables.
     */
    void parseAndSetOptimizationParameters(int);

    /* This map holds estimated device sizes that would
     * correspond to the architecture, keyed by layout name
     */
    std::map<std::string,std::pair<int,int>> _grid_layout_sizes; 

    // These booleans store devices selected for optimization
    bool _mixMultipliers;
    bool _mixAdders;
  	/* If  mixing soft and hard_logic is set, the possible size of the device 
	 * should be estimated 
	 */  
    OdinGridAnalyzer* _analyzer;
    bool _shouldNotOptimizeAtAll;
};



#endif