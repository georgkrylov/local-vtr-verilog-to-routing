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
#ifndef ODIN_GRID_ANALYZER_HPP
#define ODIN_GRID_ANALYZER_HPP

#include "physical_types.h"
#include "odin_types.h"
#include <utility>

class OdinGridAnalyzer{
    public:
        /*----------------------------------------------------------------------
        * Function: estimatePossibleDeviceSize 
        *   Estimates possible size of the device: for a fixed layout, it is 
        *   known, for a dynamic layout, some assumptions are required 
        * Parameters: 
        *   layout: const t_grid_def : layout to explore
        * returns:
        *   Pair of integers, that correspond to estimated width and height of 
        *   the device
        *---------------------------------------------------------------------*/
        std::pair<int,int> estimatePossibleDeviceSize(const t_grid_def&);
    private:
};
#endif