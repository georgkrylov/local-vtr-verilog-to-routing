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

#include "Hard_Soft_Logic_Mixer.hpp"
#include <iostream>

HardSoftLogicMixer::HardSoftLogicMixer(const t_arch& Arch,int config):_arch(Arch){
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

void HardSoftLogicMixer::estimate_possible_device_size(){
	if (_shouldNotOptimizeAtAll)
		return;
	bool multipleArchitectureFiles = _arch.grid_layouts.size() > 1 ? true : false;
	// At the development stage, the estimate works only for one device, otherwise it
	// should not be turned on, because it was not tested thoroughly
	if (multipleArchitectureFiles)
		{
		std::cerr<<"Odin won't continue execution since requested optimization parameters are not implemented: multiple architecture files and mixing soft and hard logic";
		std::cerr<<_arch.grid_layouts.size();
		exit(6);
		}
	if (_arch.grid_layouts[0].grid_type == FIXED)
	{	
		int width;
		int height;
		width = _arch.grid_layouts[0].width;
		height = _arch.grid_layouts[0].height;
		_grid_layout_sizes.emplace_back(width,height);
	}
	
	else
	{
		std::cerr<<"Odin won't continue execution since requested optimization parameters are not implemented: auto_layout and mixing soft and hard logic\n";
		exit(6);
	}		
	
}