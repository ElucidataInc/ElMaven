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

// This library implements a very basic three layer backpropagation neural
// network. The two classes are "neuron" which represents an adaptive line
// combiner similar to ADALINE, and "nnwork" which is the entire network.

#ifndef _NEURON_H
#define _NEURON_H
#define INPUT 1
#define NONINPUT 0

struct neuron {
	float *weights;
	float output;
};

class nnlayer {
public:
	neuron *nodes;
	nnlayer (int, int);		// Number of nodes in the layer.
	~nnlayer ();
private:
	int size;
	int weights;
};

#endif
