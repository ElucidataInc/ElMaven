/****************************************************************************

    Neural Network Library
    Copyright (C) 1998 Daniel Franklin

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
    02111-1307, USA.

 ****************************************************************************/

#ifndef _NNWORK_H
#define _NNWORK_H
#include "neuron.h"
#include <string.h>
#include <vector>

#define ALL 0
#define NEUN_INPUT 1
#define HIDDEN 2
#define OUTPUT 3

// Sigmoid function. Basically a differentiable threshold function.

float sigmoid (float);

// This class implements a simple three-layer backpropagation network.

class nnwork {
// Initialise with the dimensions of the network (input, hidden, output)
public:
	nnwork (int, int, int);
	nnwork ();
	nnwork (char*);
	~nnwork ();
	
	
// returns dims of network - argument is either ALL, INPUT, HIDDEN or OUTPUT
// (see above). ALL gives total nodes (useful to see if network is empty).

	int get_layersize (int);

// Training args are input, desired output, minimum error, learning rate

	void train (float [], float [], float, float);

    /**
     * @brief Given a vector of inputs, outputs a vector of adjusted values.
     * @details This method should only be used after the network has been
     * suitably trained. When it is ready, data are applied at the input, and
     * the output (basically a boolean output but could be different) goes into
     * the result. Naturally, data and result are the same size as the input and
     * output vectors respectively.
     * @param data A vector of input float values.
     * @return Weight-adjusted output vector, coming as the result of pretrained
     * neural net.
     */
    std::vector<float> run(std::vector<float>& data);

    /**
     * @brief Legacy function that does the same thing as the other `run` method
     * except this one does update the NN.
     */
    void run(float [], float []);

// Arg for load and save is just the filename.

	int load (char*);
	int save (char*);

private:
	nnlayer *output_nodes;
	nnlayer *hidden_nodes;
	int input_size;
	int output_size;
	int hidden_size;
};

#endif
