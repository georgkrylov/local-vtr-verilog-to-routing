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
#include "HardSoftLogicMixer.hpp"
#include <iostream>
#include <vector>
#include "OdinGridAnalyzer.hpp"

HardSoftLogicMixer::HardSoftLogicMixer(const t_arch& arch,int config):_analyzer(),_arch(arch){
	_mixMultipliers = false;
	_mixAdders = false;
	_shouldNotOptimizeAtAll = true;
	parseAndSetOptimizationParameters(config);
}

void HardSoftLogicMixer::parseAndSetOptimizationParameters(int config){
	if (config != 0)
		_shouldNotOptimizeAtAll = false;
	if ((config & 1) == 1){
		_mixMultipliers = true;
	}
	if (((config>> 1) & 1) == 1){
		_mixAdders = true;
	}
}

void HardSoftLogicMixer::calculateAllGridSizes(){
	if (true ==_shouldNotOptimizeAtAll)
	{
		std::cerr<<"ODIN won't continue execution of calculateAllGridSizes()"<<
					"since it should not optimize at all";
		exit(6);
	}		
	for (int i = 0 ; i < _arch.grid_layouts.size();i++)
	{
		const t_grid_def& ref =   _arch.grid_layouts[i];
		std::pair<int,int> widthAndHeight = _analyzer.estimatePossibleDeviceSize(ref);
		_grid_layout_sizes.emplace(i,widthAndHeight);
		
	}
	
}
void HardSoftLogicMixer::takeNoteOfAMultiply(const nnode_t * multNode){
	_multiplierNodes.emplace_back(multNode);
}
void HardSoftLogicMixer::selectLogicToImplementInHardBlocks(){

}