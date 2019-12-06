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
#include <map>

class OdinGridAnalyzer{
    public:
        OdinGridAnalyzer(const t_arch& arch);
        /*---------------------------------------------------------------------------------------------
        * Estimate possible size of the device: for the fixed layout, it is known,
        * for the dynamic layout, some assumptions are required 
        *-------------------------------------------------------------------------*/
        void estimate_possible_device_size(std::map<std::string,std::pair<int,int>> gridLayoutSizes);
    private:
        const t_arch& _arch;
};
#endif