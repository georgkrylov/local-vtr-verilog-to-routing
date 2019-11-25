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

#include "netlist_analyzer.h"

int calculate_multiplier_aware_critical_path(nnode_t* node,netlist_t* netlist){
    int i,j;
	if (node->traverse_visited != MULT_OPTIMIZATION_TRAVERSE_VALUE){
		/*this is a new node so depth visit it */

		/* mark that we have visitied this node now */
		node->traverse_visited = MULT_OPTIMIZATION_TRAVERSE_VALUE;
        int multiplyMultiplier = 5;
		for (i = 0; i < node->num_output_pins; i++)
		{
			if (node->output_pins[i]->net)
			{
				nnet_t *next_net = node->output_pins[i]->net;
				if(next_net->fanout_pins)
				{
					for (j = 0; j < next_net->num_fanout_pins; j++)
					{
						if (next_net->fanout_pins[j])
						{
							if (next_net->fanout_pins[j]->node)
							{
							/* recursive call point */
								calculate_multiplier_aware_critical_path(next_net->fanout_pins[j]->node, netlist);
							}
						}
					}
				}
			}
		}
        node->traverse_visited = PARTIAL_MAP_TRAVERSE_VALUE;
	}
}