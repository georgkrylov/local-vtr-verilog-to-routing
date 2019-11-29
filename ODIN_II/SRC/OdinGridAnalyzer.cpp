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
#include <iostream>
#include <bits/stdc++.h> 
#include <algorithm>  // std::min
#include "expr_eval.h" // parse_formula 

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
	//TODO account with fill,corners,perimeter
	std::vector<std::tuple<int,int,int>> grid (height,std::vector<std::pair<int,int>>(width,std::make_pair(-1,-1)));
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
			int grid_width = size.first;
			int grid_height = size.second;
			int w = 0;
			int h = 0;
			if (tileTypeOfABlockWithHigherPriority!=nullptr){
				std::cout<<"Width of a block is:\t"<<tileTypeOfABlockWithHigherPriority->width<<"\tHeight of a block is:"
						<<tileTypeOfABlockWithHigherPriority->height<<std::endl;
				w = tileTypeOfABlockWithHigherPriority->width;
				h = tileTypeOfABlockWithHigherPriority->height;
			}

			fillGridWithBlockType(grid,&(*pointer),tileTypeOfABlockWithHigherPriority,grid_width,grid_height);
	
		}
	}
	//TODO consider different mults
}
void
OdinGridAnalyzer::fillGridWithBlockType(std::vector<std::vector<std::pair<int,int>>>grid, 
										t_grid_loc_def* grid_loc_def, t_physical_tile_type* type,
										int grid_width,int grid_height)
	{
		/**
		 * This code is a lightweight version of vpr/src/base/SetupGrid.cpp
		 */
		t_formula_data vars;
		vars.set_var_value("W", grid_width);
		vars.set_var_value("H", grid_height);
		vars.set_var_value("w", type->width);
		vars.set_var_value("h", type->height);

		//Load the x specification
		auto& xspec = grid_loc_def->x;

		VTR_ASSERT_MSG(!xspec.start_expr.empty(), "x start position must be specified");
		VTR_ASSERT_MSG(!xspec.end_expr.empty(), "x end position must be specified");
		VTR_ASSERT_MSG(!xspec.incr_expr.empty(), "x increment must be specified");
		VTR_ASSERT_MSG(!xspec.repeat_expr.empty(), "x repeat must be specified");

		size_t startx = parse_formula(xspec.start_expr, vars);
		size_t endx = parse_formula(xspec.end_expr, vars);
		size_t incrx = parse_formula(xspec.incr_expr, vars);
		size_t repeatx = parse_formula(xspec.repeat_expr, vars);

		//Load the y specification
		auto& yspec = grid_loc_def->y;

		VTR_ASSERT_MSG(!yspec.start_expr.empty(), "y start position must be specified");
		VTR_ASSERT_MSG(!yspec.end_expr.empty(), "y end position must be specified");
		VTR_ASSERT_MSG(!yspec.incr_expr.empty(), "y increment must be specified");
		VTR_ASSERT_MSG(!yspec.repeat_expr.empty(), "y repeat must be specified");

		size_t starty = parse_formula(yspec.start_expr, vars);
		size_t endy = parse_formula(yspec.end_expr, vars);
		size_t incry = parse_formula(yspec.incr_expr, vars);
		size_t repeaty = parse_formula(yspec.repeat_expr, vars);

		std::cout<<"\t"<<"\t"<<"\tx\tevaluates to\ty\tevaluates to"<<std::endl;
		std::cout<<"_____________________________________________________________"<<std::endl;
		std::cout<<"Start_expr\t" << grid_loc_def->x.start_expr<<"\t"<<startx
				<<"\t"<<grid_loc_def->y.start_expr<<"\t"<<starty<<std::endl;

		std::cout<<"End_expr\t" << grid_loc_def->x.end_expr<<"\t"<<endx
				<<"\t"<<grid_loc_def->y.end_expr<<"\t"<<endy<<std::endl;

		std::cout<<"Incr_expr\t" << grid_loc_def->x.incr_expr<<"\t"<<incrx
				<<"\t"<<grid_loc_def->y.incr_expr<<"\t"<<incry<<std::endl;

		std::cout<<"Repeat_expr\t" << grid_loc_def->x.repeat_expr<<"\t"<<repeatx
				<<"\t"<<grid_loc_def->y.repeat_expr<<"\t"<<repeaty<<std::endl;

		std::cout<<"Priority:\t" <<grid_loc_def->priority<<std::endl;

        //Check start against the device dimensions
        // Start locations outside the device will never create block instances
        if (startx > grid_width - 1) {
            // if (warn_out_of_range) {
            //     VTR_LOG_WARN("Block type '%s' grid location specification startx (%s = %d) falls outside device horizontal range [%d,%d]\n",
            //                  type->name, xspec.start_expr.c_str(), startx, 0, grid_width - 1);
            // }
            // continue; //No instances will be created
			return;
        }

        if (starty > grid_height - 1) {
            // if (warn_out_of_range) {
            //     VTR_LOG_WARN("Block type '%s' grid location specification starty (%s = %d) falls outside device vertical range [%d,%d]\n",
            //                  type->name, yspec.start_expr.c_str(), starty, 0, grid_height - 1);
            // }
            // continue; //No instances will be created
			return;
        }

        //Check end against the device dimensions
        if (endx > grid_width - 1) {
            // if (warn_out_of_range) {
            //     VTR_LOG_WARN("Block type '%s' grid location specification endx (%s = %d) falls outside device horizontal range [%d,%d]\n",
            //                  type->name, xspec.end_expr.c_str(), endx, 0, grid_width - 1);
            // }
			return;
        }

        if (endy > grid_height - 1) {
            // if (warn_out_of_range) {
            //     VTR_LOG_WARN("Block type '%s' grid location specification endy (%s = %d) falls outside device vertical range [%d,%d]\n",
            //                  type->name, yspec.end_expr.c_str(), endy, 0, grid_height - 1);
            // }
			return;
        }

        //The end must fall after (or equal) to the start
        if (endx < startx) {
            printf("Grid location specification endx (%s = %d) can not come before startx (%s = %d) for block type '%s'",
                            xspec.end_expr.c_str(), endx, xspec.start_expr.c_str(), startx, type->name);
        }

        if (endy < starty) {
            printf("Grid location specification endy (%s = %d) can not come before starty (%s = %d) for block type '%s'",
                            yspec.end_expr.c_str(), endy, yspec.start_expr.c_str(), starty, type->name);
        }

        //The minimum increment is the block dimension
        // VTR_ASSERT(type->width > 0);
        if (incrx < size_t(type->width)) {
            printf("Grid location specification incrx for block type '%s' must be at least"
                            " block width (%d) to avoid overlapping instances (was %s = %d)",
                            type, type->width, xspec.incr_expr.c_str(), incrx);
        }

        VTR_ASSERT(type->height > 0);
        if (incry < size_t(type->height)) {
             printf("Grid location specification incry for block type '%s' must be at least"
                    " block height (%d) to avoid overlapping instances (was %s = %d)",
                    type->name, type->height, yspec.incr_expr.c_str(), incry);
        }

        //The minimum repeat is the region dimension
        size_t region_width = endx - startx + 1; //+1 since start/end are both inclusive
        if (repeatx < region_width) {
			printf(     "Grid location specification repeatx for block type '%s' must be at least"
                    	" the region width (%d) to avoid overlapping instances (was %s = %d)",
                        type->name, region_width, xspec.repeat_expr.c_str(), repeatx);
        }

        size_t region_height = endy - starty + 1; //+1 since start/end are both inclusive
        if (repeaty < region_height) {
			printf("Grid location specification repeaty for block type '%s' must be at least"
                   " the region height (%d) to avoid overlapping instances (was %s = %d)",
                   type->name, region_height, xspec.repeat_expr.c_str(), repeaty);
        }

        //VTR_LOG("Applying grid_loc_def for '%s' priority %d startx=%s=%zu, endx=%s=%zu, starty=%s=%zu, endx=%s=%zu,\n",
        //            type->name, grid_loc_def.priority,
        //            xspec.start_expr.c_str(), startx, xspec.end_expr.c_str(), endx,
        //            yspec.start_expr.c_str(), starty, yspec.end_expr.c_str(), endy);

        size_t x_end = 0;
        for (size_t kx = 0; x_end < grid_width; ++kx) { //Repeat in x direction
            size_t x_start = startx + kx * repeatx;
            x_end = endx + kx * repeatx;

            size_t y_end = 0;
            for (size_t ky = 0; y_end < grid_height; ++ky) { //Repeat in y direction
                size_t y_start = starty + ky * repeaty;
                y_end = endy + ky * repeaty;

                size_t x_max = std::min(x_end, grid_width - 1);
                size_t y_max = std::min(y_end, grid_height - 1);

                //Fill in the region
                for (size_t x = x_start; x + (type->width - 1) <= x_max; x += incrx) {
                    for (size_t y = y_start; y + (type->height - 1) <= y_max; y += incry) {
						grid[x][]
                        set_grid_block_type(grid_loc_def.priority, type, x, y, grid, grid_priorities, grid_loc_def.meta);
                    }
                }
            }
        }

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

// int OdinGridAnalyzer::parseExpression(std::string& expr, int position,int sign, int W, int H, int w, int h){
// 	if (position == expr.size()) return 0;
// 	while (position< expr.size() && expr[position]==' ') position++;
// 	if (position == expr.size()) return 0;
// 	if (expr[position]=='w')
// 		return sign * w + parseExpression(expr,position+1,1,W,H,w,h);
// 	else if (expr[position]=='W')
// 		return sign * W + parseExpression(expr,position+1,1,W,H,w,h);
// 	else if (expr[position]=='h')
// 		return sign * h + parseExpression(expr,position+1,1,W,H,w,h);
// 	else if (expr[position]=='H')
// 		return sign * H + parseExpression(expr,position+1,1,W,H,w,h);
// 	else if (expr[position]=='+')
// 		return parseExpression(expr,position+1,1,W,H,w,h);
// 	else if (expr[position]=='-')
// 		return parseExpression(expr,position+1,-1,W,H,w,h);
// 	else {
// 		int tempBorder = position;
// 		while (tempBorder < expr.size() && expr[tempBorder]!=' ') tempBorder++;
// 		std::string digitCandidate = expr.substr (position,tempBorder);
// 		std::cout<<"digitCandidate is" <<digitCandidate<<std::endl;
// 		fflush(stdout);
// 		int number = std::stoi(digitCandidate);
// 		if (tempBorder == expr.size()) return sign*number;
// 		return sign*number+parseExpression(expr,position+1,1,W,H,w,h);
// 	}
// }
