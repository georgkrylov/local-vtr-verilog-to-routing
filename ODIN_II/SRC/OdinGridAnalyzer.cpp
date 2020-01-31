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
	int grid_width = size.first;
	int grid_height = size.second;
	//TODO account with fill,corners,perimeter
	std::vector<std::vector<std::pair<char*,int>>> grid (grid_height,std::vector<std::pair<char*,int>>(grid_width,std::make_pair(nullptr,-1)));
	std::vector<t_grid_loc_def>::iterator pointer;
	std::string typeTag = getArchDescriptionTag(hardBlockType);
	std::cout<<typeTag<<std::endl;
	int priorityOfTheDesiredBlock = findDesiredBlockPriority(layout,typeTag);
	std::cout<<"Desired block's priority is: "<<priorityOfTheDesiredBlock<<std::endl;
	std::cout<<"BlockDefsAre:"<<std::endl;
	for (pointer =  layout.loc_defs.begin();pointer<layout.loc_defs.end(); pointer++){
		// Case of == is the tile type we are looking for, and a clash
		// if there are collisions, vpr would fail anyway
		if (priorityOfTheDesiredBlock <= pointer->priority){
			std::cout<<pointer->block_type<<":"<<std::endl;
			ssize_t indexOfTile  = findIndexOfAssociatedTileType(tileTypes,pointer->block_type);
			if (indexOfTile == -1)
				{
					std::cout<<" Was not able to find a tile"<<std::endl;
					flush(std::cout);
					exit(6);
				}
			t_physical_tile_type& tileTypeOfABlockWithHigherPriority = tileTypes[indexOfTile];
			int w = 0;
			int h = 0;
			std::cout<<"Width of a block is:\t"<<tileTypeOfABlockWithHigherPriority.width<<"\tHeight of a block is:"
				<<tileTypeOfABlockWithHigherPriority.height<<std::endl;
			w = tileTypeOfABlockWithHigherPriority.width;
			h = tileTypeOfABlockWithHigherPriority.height;
		
			fillGridWithBlockType(grid,&(*pointer),tileTypes,indexOfTile,grid_width,grid_height);
		}
	}
	std::cout<<"Grid is:"<<std::endl;
	for (size_t x = 0; x<grid_width; x += 1) 
	{
	for (size_t y = 0; y <grid_height; y += 1) 
		{
		if (grid[x][y].first != nullptr){
			std::cout<<"("<<grid[x][y].first<<","<<grid[x][y].second<<")\t";
		} else {
			std::cout<<"( u, -1), ";
		}
		}
	std::cout<<std::endl;
	}
	int count = 0;
	std::cout<<"Grid is:"<<std::endl;
	for (size_t x = 0; x<grid_width; x += 1) 
	{
	for (size_t y = 0; y <grid_height; y += 1) 
		{
		if (grid[x][y].first != nullptr){
			std::string stringToCompare = std::string(grid[x][y].first);
			if (stringToCompare.compare(typeTag)==0)
				count = count + 1;
		} 
		}
	}
	//TODO consider different mults
	return count;
}
void
OdinGridAnalyzer::fillGridWithBlockType(std::vector<std::vector<std::pair<char*,int>>>& grid, 
										t_grid_loc_def* grid_loc_def, 
										std::vector<t_physical_tile_type> tileTypes,
										ssize_t indexOfTile,
										int grid_width,int grid_height)
	{
		t_physical_tile_type* type = &(tileTypes[indexOfTile]);
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
			return;
        }

        if (starty > grid_height - 1) {
			return;
        }

        //Check end against the device dimensions
        if (endx > grid_width - 1) {
			return;
        }

        if (endy > grid_height - 1) {
			return;
        }

        //The end must fall after (or equal) to the start
        if (endx < startx) {
            printf("Grid location specification endx (%s = %lu) can not come before startx (%s = %lu) for block type '%s'",
                            xspec.end_expr.c_str(), endx, xspec.start_expr.c_str(), startx, type->name);
        }

        if (endy < starty) {
            printf("Grid location specification endy (%s = %lu) can not come before starty (%s = %lu) for block type '%s'",
                            yspec.end_expr.c_str(), endy, yspec.start_expr.c_str(), starty, type->name);
        }

        //The minimum increment is the block dimension
        VTR_ASSERT(type->width > 0);
        if (incrx < size_t(type->width)) {
            printf("Grid location specification incrx for block type '%s' must be at least"
                            " block width (%d) to avoid overlapping instances (was %s = %lu)",
                            type->name, type->width, xspec.incr_expr.c_str(), incrx);
        }

        VTR_ASSERT(type->height > 0);
        if (incry < size_t(type->height)) {
             printf("Grid location specification incry for block type '%s' must be at least"
                    " block height (%d) to avoid overlapping instances (was %s = %lu)",
                    type->name, type->height, yspec.incr_expr.c_str(), incry);
        }

        //The minimum repeat is the region dimension
        size_t region_width = endx - startx + 1; //+1 since start/end are both inclusive
        if (repeatx < region_width) {
			printf(     "Grid location specification repeatx for block type '%s' must be at least"
                    	" the region width (%lu) to avoid overlapping instances (was %s = %lu)",
                        type->name, region_width, xspec.repeat_expr.c_str(), repeatx);
        }

        size_t region_height = endy - starty + 1; //+1 since start/end are both inclusive
        if (repeaty < region_height) {
			printf("Grid location specification repeaty for block type '%s' must be at least"
                   " the region height (%lu) to avoid overlapping instances (was %s = %lu)",
                   type->name, region_height, xspec.repeat_expr.c_str(), repeaty);
        }

        size_t x_end = 0;
        for (size_t kx = 0; x_end < grid_width; ++kx) { //Repeat in x direction
            size_t x_start = startx + kx * repeatx;
            x_end = endx + kx * repeatx;

            size_t y_end = 0;
            for (size_t ky = 0; y_end < grid_height; ++ky) { //Repeat in y direction
                size_t y_start = starty + ky * repeaty;
                y_end = endy + ky * repeaty;

                size_t x_max = std::min((int)x_end, grid_width - 1);
                size_t y_max = std::min((int)y_end, grid_height - 1);

                //Fill in the region
                for (size_t x = x_start; x + (type->width - 1) <= x_max; x += incrx) {
                    for (size_t y = y_start; y + (type->height - 1) <= y_max; y += incry) {
						
                        set_grid_block_type(grid_loc_def->priority, type, x, y, grid);
                    }
                }
            }
        }
	}
/**
 *  First is type (char*), second is priority
 */ 
void OdinGridAnalyzer::set_grid_block_type(int priority,t_physical_tile_type* type, int x_root, int y_root,std::vector<std::vector<std::pair<char*,int>>>&  grid){
	if (grid[x_root][y_root].second < priority){
            grid[x_root][y_root].first = type->name;
            grid[x_root][y_root].second = priority;

	// This block is commented out, to simplify counting
	// if this code is enabled, the hard block will occupy as
	// much grid tiles as it should, however, a special counting
	// procedure should be implemented. That being said,
	// somebody might need that code in future, whereas
	// it is not required at current state

    // for (size_t x = x_root; x < x_root + type->width; ++x) {
    //     size_t x_offset = x - x_root;
    //     for (size_t y = y_root; y < y_root + type->height; ++y) {
    //         size_t y_offset = y - y_root;
    //         auto& grid_tile = grid[x][y];
    //         grid[x][y].first = type->name;
    //         grid[x][y].second = priority;
    //     }
    // }

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

ssize_t OdinGridAnalyzer::findIndexOfAssociatedTileType(std::vector<t_physical_tile_type> tileTypes,std::string& typeTag){
	ssize_t result = -1;
	ssize_t iter = -1;
	std::cout<< "The type tag is"<<typeTag<<std::endl;
	std::vector<t_physical_tile_type>::iterator pointer;

	for (pointer =  tileTypes.begin();pointer< tileTypes.end(); pointer++){
		iter ++ ; 
		std::string stringForComparison = std::string(pointer->name);
		std::cout << stringForComparison<<std::endl;
		if (stringForComparison.compare(typeTag)==0){
			result = iter;
			break;
		}
	}
	if (result == -1)
	{
		std::cerr<<"Was not able to find associated tile type"<<std::endl;
	}
	return result;
}

std::string OdinGridAnalyzer::getArchDescriptionTag(int hardBlockType){
	std::string result;
	switch (hardBlockType)
	{
	case mix_hard_blocks::MULTIPLIERS:
	{	
		result = "mult_36";
		break;
	}
	case mix_hard_blocks::ADDERS:
	{
		result = "adder";
		break;
	}
	default:
		result = "The requested hardBlockType is not supported";
		break;
	}
	return result;
}
