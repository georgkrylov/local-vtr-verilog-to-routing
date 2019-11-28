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

#include "OdinGridAnalyzer.hpp"
#include <utility>      // std::pair, std::make_pair
#include <vector>
#include <iostream>
#include <bits/stdc++.h> 

std::pair<int,int> 
OdinGridAnalyzer::estimatePossibleDeviceSize(t_grid_def& layout){
	std::pair<int,int> result;
	if (layout.grid_type == FIXED)
	{	
		int width;
		int height;
		width = layout.width;
		height = layout.height;
		result = std::make_pair(width,height);
	}
	else
	{
		result = std::make_pair(INT_MAX,INT_MAX);
	}	
	return result;
}	

int OdinGridAnalyzer::countHardBlocksInFixedLayout(t_grid_def& layout ,int hardBlockType,std::pair<int,int> size){
	int width = size.first;
	int height = size.second;
	//TODO account with fill
	std::vector<std::vector<std::pair<int,int>>> grid (height,std::vector<std::pair<int,int>>(width,std::make_pair(-1,-1)));
	std::vector<t_grid_loc_def>::iterator pointer;
	std::cout<<"BlockDefsAre:"<<std::endl;
	for (pointer =  layout.loc_defs.begin();pointer<layout.loc_defs.end(); pointer++){
		std::cout<<"\t"<<pointer->block_type<<"\tx\ty"<<std::endl;
		std::cout<<"Start_expr\t" << pointer->x.start_expr<<"\t"<<pointer->y.start_expr<<std::endl;
		std::cout<<"End_expr\t" << pointer->x.end_expr<<"\t"<<pointer->y.end_expr<<std::endl;
		std::cout<<"Incr_expr\t" << pointer->x.incr_expr<<"\t"<<pointer->y.incr_expr<<std::endl;
		std::cout<<"Repeat_expr:\t" << pointer->x.incr_expr<<"\t"<<pointer->y.incr_expr<<std::endl;
		std::cout<<pointer->meta;
	
	}
	
}