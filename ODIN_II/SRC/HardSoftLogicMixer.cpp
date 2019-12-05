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
#include <cstdint>
#include <cstring>
#include "OdinGridAnalyzer.hpp"
#include <bits/stdc++.h> 
#include "partial_map.h" // instantiate add_with_carry

HardSoftLogicMixer::HardSoftLogicMixer(t_arch& arch,const config_t config,std::vector<t_physical_tile_type> tileTypes):_analyzer(),_arch(arch){
	_allOptsDisabled = true;
	_tileTypes = tileTypes;
	// By default, disables all optimizations
	for (int i = 0; i < HardBlocksOptimizationTypesEnum::Count; i++){
		_enabledOptimizations[i] = false;
	}

	parseAndSetOptimizationParameters(config);
}

void HardSoftLogicMixer::parseAndSetOptimizationParameters(const config_t config){
	if (config.mix_soft_and_hard_logic!=0){
		int checkValue = -1;
		int mix_soft_and_hard_logic = config.mix_soft_and_hard_logic;
		int temp;
		for (int i = 0; i < HardBlocksOptimizationTypesEnum::Count; i++){
			temp = 1 << (i);
			checkValue = temp & mix_soft_and_hard_logic;
			if (checkValue != 0){
				_enabledOptimizations[i] = true;
			}
		}
		_allOptsDisabled = false;
	}
	//Explicitly set all hard block numbers to max
	for (int i = 0 ; i<HardBlocksOptimizationTypesEnum::Count; i++)
	{
		_hardBlocksCount[i] = INT_MAX;
		_hardBlocksMixingRatio[i] = 1.0f;
	}

	_hardBlocksMixingRatio[HardBlocksOptimizationTypesEnum::MULTIPLIERS] = config.mults_mixing_ratio;
	

	if (config.mults_mixing_ratio >= 0.0f && config.mults_mixing_ratio <= 1.0f){
		_enabledOptimizations[HardBlocksOptimizationTypesEnum::MULTIPLIERS]=true;
		_allOptsDisabled = false;
	}

	if (config.mults_mixing_exact_number_of_multipliers >= 0){
		_hardBlocksMixingRatio[HardBlocksOptimizationTypesEnum::MULTIPLIERS]  = 1.0f;
		_enabledOptimizations[HardBlocksOptimizationTypesEnum::MULTIPLIERS]=true;
		_hardBlocksCount[HardBlocksOptimizationTypesEnum::MULTIPLIERS] = config.mults_mixing_exact_number_of_multipliers;
		_allOptsDisabled = false;
	}
}

void HardSoftLogicMixer::calculateAllGridSizes(){
	if (true ==_allOptsDisabled)
	{
		// std::cerr<<"ODIN won't continue execution of calculateAllGridSizes()"<<
		// 			"since it should not optimize at all";
		return;
	}		
	for (int i = 0 ; i < _arch.grid_layouts.size();i++)
	{
		if (i == 1){
			std::cerr<<"Optimization for multiple layouts was never tested, from now on you are on your own"<<std::endl;
		}
		t_grid_def& ref = _arch.grid_layouts[i];
		std::pair<int,int> widthAndHeight = _analyzer.estimatePossibleDeviceSize(ref);
		_grid_layout_sizes.emplace(i,widthAndHeight);

		for (int currentOptimizationKind = 0 ; currentOptimizationKind < HardBlocksOptimizationTypesEnum::Count; currentOptimizationKind++)
		{
			if (true == _enabledOptimizations[currentOptimizationKind])
			{
				// TODO make heterogeneous blocks happen
				int availableHardBlocks = countHardBlocksInArch(ref,currentOptimizationKind,widthAndHeight);
				int hardBlocksNeeded = inferHardBlocksFromNetlist(currentOptimizationKind);
				int hardBlocksCount = availableHardBlocks;
				if (hardBlocksCount > hardBlocksNeeded)
				{
					hardBlocksCount = hardBlocksNeeded; 
				}
				if (hardBlocksCount < _hardBlocksCount[currentOptimizationKind]){
					_hardBlocksCount[currentOptimizationKind] = hardBlocksCount;
				}
				std::cout<<"The estimated hardBlocksCount"<<_hardBlocksCount[currentOptimizationKind] <<std::endl;
			} 
		}
	}
	scaleHardBlockCounts();
}

int HardSoftLogicMixer::countHardBlocksInArch(t_grid_def& layout ,int hardBlockType,std::pair<int,int> size){
	int result = INT_MAX;
	if (layout.grid_type == FIXED)
	{	
		result = _analyzer.countHardBlocksInFixedLayout(layout,hardBlockType,size,_tileTypes);
	}
	return result;
}	
int HardSoftLogicMixer::inferHardBlocksFromNetlist(int currentOptimizationKind){
	return potentialHardBlockNodes[currentOptimizationKind].size();
}

void HardSoftLogicMixer::scaleHardBlockCounts(){
	for (int i = 0; i < HardBlocksOptimizationTypesEnum::Count; i++){
		if (_hardBlocksMixingRatio[i]!=-1){
		_hardBlocksCount[i] = _hardBlocksCount[i]*_hardBlocksMixingRatio[i];
		}
	}
}

void HardSoftLogicMixer::takeNoteOfAPotentialHardBlockNode( nnode_t * opNode, HardBlocksOptimizationTypesEnum type){
	potentialHardBlockNodes[type].emplace_back(opNode);
}

bool HardSoftLogicMixer::mixHardBlocksOfType(HardBlocksOptimizationTypesEnum type)
	{ 
		bool result = false;
		if (type != HardBlocksOptimizationTypesEnum::Count)
		result = _enabledOptimizations[type];
		return result;
	}

void HardSoftLogicMixer::selectLogicToImplementInHardBlocks(netlist_t *netlist){
	calculateAllGridSizes();
	for (int i = 0; i < HardBlocksOptimizationTypesEnum::Count;i++ ){
		switch (i){
			case HardBlocksOptimizationTypesEnum::MULTIPLIERS: 
				if (_enabledOptimizations[HardBlocksOptimizationTypesEnum::MULTIPLIERS])
				{
					selectMultipliersToImplementInHardBlocks(netlist);
				}
				break;
			case HardBlocksOptimizationTypesEnum::ADDERS:	
				if (_enabledOptimizations[HardBlocksOptimizationTypesEnum::ADDERS])
				{

				}
				break;
			default:
				std::cerr<<"Optimization with number: "<<i<<" does not have an "<<
				"implementation inside of HardSoftLogicMixer::selectLogicToIm"<<
				"plementInHardBlocks"<<std::endl;
				break;
		}
	}
	implementUnassignedLogicInSoftLogic(netlist);
}

void 
HardSoftLogicMixer::implementUnassignedLogicInSoftLogic(netlist_t* netlist){
	for (int i = 0 ; i < HardBlocksOptimizationTypesEnum::Count ; i ++ )
	{
	for (int j = 0 ; j <potentialHardBlockNodes[i].size(); j++)
		{
			switch(i){
				case HardBlocksOptimizationTypesEnum::MULTIPLIERS:
					instantiate_simple_soft_multiplier(potentialHardBlockNodes[i][j],PARTIAL_MAP_TRAVERSE_VALUE,netlist);
					break;
				case HardBlocksOptimizationTypesEnum::ADDERS:
					instantiate_add_w_carry(potentialHardBlockNodes[i][j], PARTIAL_MAP_TRAVERSE_VALUE, netlist);
					break;
				default:
				std::cerr<<"Cleanup for optimization with number: "<<i<<" does not have an "<<
				"implementation inside of HardSoftLogicMixer::implementUnassignedLogicInSoftLogic"<<
				std::endl;
				break;
			}
				
			
		}
	}
}

void 
HardSoftLogicMixer::selectMultipliersToImplementInHardBlocks(netlist_t* netlist){
	std::vector<nnode_t*>& nodesVector = potentialHardBlockNodes[HardBlocksOptimizationTypesEnum::MULTIPLIERS];
	int nodesCount = nodesVector.size();
	int* costs = new int[nodesCount];
	for (int i = 0 ; i < nodesCount;i++){
		costs[i] =calculate_multiplier_aware_critical_path(nodesVector[i],netlist);
	}
	std::cout<<"Costs are:"<<std::endl<<"\t";
	for (int i = 0 ; i < nodesCount;i++){
		std::cout<<costs[i]<<" ";
	}
	std::cout<<std::endl;
	flush(std::cout);
	int numberOfMultipliers = _hardBlocksCount[HardBlocksOptimizationTypesEnum::MULTIPLIERS];
	for (int i = 0 ; i < numberOfMultipliers; i++){
		int maximalCost = costs[0];
		int indexOfMaximum = 0;
		for (int j = 0 ; j < nodesCount; j++ ){
			if (maximalCost<costs[j] && (nodesVector[j]->input_port_sizes[0] > 1)&& (nodesVector[j]->input_port_sizes[1] > 1)){
				maximalCost = costs[j];
				indexOfMaximum = j;
			}
		}
		costs[indexOfMaximum] = -1;
		instantiate_hard_multiplier(nodesVector[indexOfMaximum],PARTIAL_MAP_TRAVERSE_VALUE,netlist);
	}

	for (int i = nodesCount - 1 ; i >= 0; i--){
		if (costs[i]== -1){
			nodesVector.erase(nodesVector.begin()+i);
		}
	}
	delete [] costs;
}
