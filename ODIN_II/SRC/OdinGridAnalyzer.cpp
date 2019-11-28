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

int OdinGridAnalyzer::countHardBlocksInFixedLayout(t_grid_def& layout ,int hardBlockType,std::pair<int,int> size,std::vector<t_physical_tile_type> tileTypes){
	int width = size.first;
	int height = size.second;
	//TODO account with fill
	std::vector<std::vector<std::pair<int,int>>> grid (height,std::vector<std::pair<int,int>>(width,std::make_pair(-1,-1)));
	std::vector<t_grid_loc_def>::iterator pointer;
	std::string typeTag = getArchDescriptionTag(hardBlockType);
	std::cout<<typeTag<<std::endl;
	int priorityOfTheDesiredBlock = findDesiredBlockPriority(layout,typeTag);
	std::cout<<"BlockDefsAre:"<<std::endl;
	for (pointer =  layout.loc_defs.begin();pointer<layout.loc_defs.end(); pointer++){
		// Case of == is the tile type we are looking for, and a clash
		// if there are collisions, vpr would fail anyway
		if (priorityOfTheDesiredBlock <= pointer->priority){
			std::cout<<pointer->block_type<<":"<<std::endl;
			t_physical_tile_type* tileTypeOfABlockWithHigherPriority = findAssociatedTileType(tileTypes,pointer->block_type);
			int W = size.first;
			int H = size.second;
			int w = 0;
			int h = 0;
			if (tileTypeOfABlockWithHigherPriority!=nullptr){
				std::cout<<"Width of a block is:\t"<<tileTypeOfABlockWithHigherPriority->width<<"\tHeight of a block is:"
						<<tileTypeOfABlockWithHigherPriority->height<<std::endl;
				w = tileTypeOfABlockWithHigherPriority->width;
				h = tileTypeOfABlockWithHigherPriority->height;
			}
			std::cout<<"\t"<<"\t"<<"\tx\tevaluates to\ty\tevaluates to"<<std::endl;
			std::cout<<"_____________________________________________________________"<<std::endl;
			std::cout<<"Start_expr\t" << pointer->x.start_expr<<"\t"<<parseExpression(pointer->x.start_expr,0,1,W,H,w,h)
					<<"\t"<<pointer->y.start_expr<<"\t"<<parseExpression(pointer->y.start_expr,0,1,W,H,w,h)<<std::endl;

			std::cout<<"End_expr\t" << pointer->x.end_expr<<"\t"<<parseExpression(pointer->x.end_expr,0,1,W,H,w,h)
					<<"\t"<<pointer->y.end_expr<<"\t"<<parseExpression(pointer->y.end_expr,0,1,W,H,w,h)<<std::endl;
			
			std::cout<<"Incr_expr\t" << pointer->x.incr_expr<<"\t"<<parseExpression(pointer->x.incr_expr,0,1,W,H,w,h)
					<<"\t"<<pointer->y.incr_expr<<"\t"<<parseExpression(pointer->y.incr_expr,0,1,W,H,w,h)<<std::endl;
			
			std::cout<<"Repeat_expr\t" << pointer->x.repeat_expr<<"\t"<<parseExpression(pointer->x.repeat_expr,0,1,W,H,w,h)
					<<"\t"<<pointer->y.repeat_expr<<"\t"<<parseExpression(pointer->y.repeat_expr,0,1,W,H,w,h)<<std::endl;

			std::cout<<"Priority:\t" <<pointer->priority<<std::endl;


		}
	}
	//TODO consider different mults
}

int OdinGridAnalyzer::findDesiredBlockPriority(t_grid_def& layout,std::string& typeTag){
	int result = -1;
	std::vector<t_grid_loc_def>::iterator pointer;
	for (pointer =  layout.loc_defs.begin();pointer<layout.loc_defs.end(); pointer++){
		if (typeTag.compare(pointer->block_type) == 0){
			result =  pointer->priority;
		}
	}
	//TODO are there any unspecified priorities?
	return result;
}

t_physical_tile_type* OdinGridAnalyzer::findAssociatedTileType(std::vector<t_physical_tile_type> tileTypes,std::string& typeTag){
	t_physical_tile_type* result = nullptr;
	std::vector<t_physical_tile_type>::iterator pointer;
	for (pointer =  tileTypes.begin();pointer< tileTypes.end(); pointer++){
		std::string stringForComparison = std::string(pointer->name);
		if (stringForComparison.compare(typeTag)==0){
			result = &(*pointer);
		}
	}
	if (result == nullptr)
	{
		std::cerr<<"Was not able to find associated tile type"<<std::endl;
	}
	return result;
}

std::string OdinGridAnalyzer::getArchDescriptionTag(int hardBlockType){
	std::string result;
	switch (hardBlockType)
	{
	case HardBlocksOptimizationTypesEnum::MULTIPLIERS:
		// TODO find all blocks
		result = "mult_36";
		
		break;
	
	default:
		result = "The requested hardBlockType is not supported";
		break;
	}
	return result;
}
int OdinGridAnalyzer::parseExpression(std::string& expr, int position,int sign, int W, int H, int w, int h){
	if (position == expr.size()) return 0;
	while (position< expr.size() && expr[position]==' ') position++;
	if (position == expr.size()) return 0;
	if (expr[position]=='w')
		return sign * w + parseExpression(expr,position+1,1,W,H,w,h);
	else if (expr[position]=='W')
		return sign * W + parseExpression(expr,position+1,1,W,H,w,h);
	else if (expr[position]=='h')
		return sign * h + parseExpression(expr,position+1,1,W,H,w,h);
	else if (expr[position]=='H')
		return sign * H + parseExpression(expr,position+1,1,W,H,w,h);
	else if (expr[position]=='+')
		return parseExpression(expr,position+1,1,W,H,w,h);
	else if (expr[position]=='-')
		return parseExpression(expr,position+1,-1,W,H,w,h);
	else {
		int tempBorder = position;
		while (tempBorder < expr.size() && expr[tempBorder]!=' ') tempBorder++;
		std::string digitCandidate = expr.substr (position,tempBorder);
		std::cout<<"digitCandidate is" <<digitCandidate<<std::endl;
		fflush(stdout);
		int number = std::stoi(digitCandidate);
		if (tempBorder == expr.size()) return sign*number;
		return sign*number+parseExpression(expr,position+1,1,W,H,w,h);
	}
}
