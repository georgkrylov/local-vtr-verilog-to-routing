/*
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef ODIN_GRID_ANALYZER_HPP
#define ODIN_GRID_ANALYZER_HPP

#include "physical_types.h"
#include "odin_types.h"
#include <utility> // std::pair, std::make_pair
#include <tuple>
#include "device_grid.h"
#include "SetupGrid.h"
#include <vector> // std::vector

class OdinGridAnalyzer {
  public:
    /*----------------------------------------------------------------------
     * Function: estimate_device_size 
     *   Estimates possible size of the device: for a fixed layout, it is 
     *   known, for a dynamic layout, some assumptions are required 
     * Parameters: 
     *   layout: const t_grid_def : layout to explore
     * returns:
     *   Pair of integers, that correspond to estimated width and height of 
     *   the device
     *---------------------------------------------------------------------*/
    std::pair<int, int> estimate_device_size(t_grid_def&);

    void createAndCacheDeviceLayout(t_arch& arch);
    int count_in_fixed(t_grid_def& layout, int hardBlockType, std::pair<int, int> size, std::vector<t_physical_tile_type> tiletypes);
    int block_priority(t_grid_def& layout, std::string& typeTag);
    ssize_t tile_index(std::vector<t_physical_tile_type> tileTypes, std::string& typeTag);
    DeviceGrid cached_grid;
  private:
    std::string arch_tag(int hardBlockType);
    // int parseExpression(std::string& expr, int position,int sign, int W, int H, int w, int h);

    /**
     * This function mimics the behaviour of the build_device_grid 
     * It is segreged into a separate function, to improve maintainability 
     */
    void fill_with_block(std::vector<std::vector<std::pair<char*, int>>>& grid, t_grid_loc_def* grid_def, std::vector<t_physical_tile_type> tileTypes, int indexOfTile, ssize_t grid_width, ssize_t grid_height);

    void set_grid_block_type(int priority, t_physical_tile_type* type, int x, int y, std::vector<std::vector<std::pair<char*, int>>>& grid);

};
#endif
