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

#include <nnwork.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <limits.h>
#include <iostream>
#include <fstream>
#include <assert.h>

using namespace std;
// This class implements a simple three-layer backpropagation network.

// Calculate the sigmoid function. This is the transfer function of the nodes.

float sigmoid (float data)
{
	return (1. / (1. + exp (-data)));
}

// Constructors: create the network.
// Arguments are the dims of the network

nnwork::nnwork (int input, int hidden, int output)
{
	input_size = input;
	hidden_size = hidden;
	output_size = output;
	srand (time (0));
	
	hidden_nodes = new nnlayer (hidden_size, input_size);
	assert (hidden_nodes);
	output_nodes = new nnlayer (output_size, hidden_size);
	assert (output_nodes);
}

// Empty network

nnwork::nnwork ()
{
	input_size = 0;
	hidden_size = 0;
	output_size = 0;
	
	hidden_nodes = 0;
	output_nodes = 0;
}

// Load straight from a file.

nnwork::nnwork (char *filename)
{
	input_size = 0;
	hidden_size = 0;
	output_size = 0;
	
	hidden_nodes = 0;
	output_nodes = 0;
	
	if (!load (filename))
		cerr << "Error: File failed to load." << endl;
}

// Destructor: destroys the network.

nnwork::~nnwork ()
{
	if (output_nodes) delete output_nodes;
	if (hidden_nodes) delete hidden_nodes;
}

// The following function is useful if you want to find out the dimensions
// of a network loaded from a file, or whatever. ALL adds up everything.

int nnwork::get_layersize (int layer)
{
	switch (layer) {
		case (ALL):
			return input_size + hidden_size + output_size;
			
		case (NEUN_INPUT):
			return input_size;

		case (HIDDEN):
			return hidden_size;

		case (OUTPUT):
			return output_size;

		default: {
			cerr << "Warning: no such layer: " << layer << endl;
			return -1;
		}
	}
}

// Training routine for the network. Uses data as input, compares output with
// desired output, computes errors, adjusts weights attached to each node,
// then repeats until the mean squared error at the output is less than 
// max_MSE. The learning rate is eta.

void nnwork::train (float data [], float desired [], float max_MSE, float eta)
{
	float *output, *output_weight_delta, *hidden_weight_delta;
	float MSE, MSE_max = max_MSE * 2., sum; // slight speed enhancement
	int i, j, k;
	long n = 0;
	if (input_size == 0 || hidden_size == 0 || output_size == 0) {
		cerr << "nnwork::train() Warning: stupid dimensions. No action taken." << endl;
        cerr << "input_size=" << input_size << endl;
        cerr << "output_size=" << output_size << endl;
        cerr << "hidden_size=" << hidden_size << endl;
		return;
	}
	
	output = new float [output_size];
	assert (output);
	output_weight_delta = new float [output_size];
	assert (output_weight_delta);
	hidden_weight_delta = new float [hidden_size];
	assert (hidden_weight_delta);

	// Keep going while the Mean Square Error is too high.
	while (1) {
		run (data, output);
		
// Firstly calculate the output layer error terms

		for (k = 0, MSE = 0; k < output_size; k++) {
			output_weight_delta [k] = desired [k] - output [k];
			MSE += output_weight_delta [k] * output_weight_delta [k];
			output_weight_delta [k] *= output [k] * (1 - output [k]);
			if (output[k] != output[k] ) {
				break;
				/*
				for(int l=0; l<input_size; l++ ) { cerr << data[l] << " " << endl; }
				cerr << "k=" << k <<  output_weight_delta [k] << " " << output [k] << " " << desired [k] << endl;
				//sleep(1);
				*/
			}
		}
		

//#ifdef WAFFLY
		n++;	
		//if (n % 10000==0) cerr << "2*MSE (every 10000th) currently is: " << MSE << endl;
		if (n>10e3) break;	//just in case we have a run away process
//#endif		
		if (MSE < MSE_max) break;

// And the hidden layer error terms

		for (j = 0; j < hidden_size; j++) {
			for (k = 0, sum = 0; k < output_size; k++)
				sum += output_weight_delta [k] * output_nodes -> nodes [k].weights [j];

			hidden_weight_delta [j] = sum * hidden_nodes -> nodes [j].output * (1 - hidden_nodes -> nodes [j].output);
		}

// Now update the output weights.

		for (k = 0; k < output_size; k++)
			for (j = 0; j < hidden_size; j++)
				output_nodes -> nodes [k].weights [j] += eta * output_weight_delta [k] * hidden_nodes -> nodes [j].output;

// And the hidden weights.

		for (j = 0; j < hidden_size; j++)
			for (i = 0; i < input_size; i++)
				hidden_nodes -> nodes [j].weights [i] += eta * hidden_weight_delta [j] * data [i];

	}

	//cerr << "A:" << " " << eta << " " << MSE << endl;
	/*
	for (k = 0; k < output_size; k++) {
		for (j = 0; j < hidden_size; j++) cerr << output_nodes -> nodes [k].weights [j] << "\t";
		cerr << endl;
	}
	*/

	delete [] output;
	delete [] output_weight_delta;
	delete [] hidden_weight_delta;
}

vector<float> nnwork::run(vector<float>& data)
{
    float sum;
    vector<float> result = vector<float>(output_size, 0);

    vector<float> dataSpecificOutputNodes = vector<float>(hidden_size, 0);
    for (int j = 0; j < hidden_size; j++) {
        sum = 0;

        // Calculate the output value
        for (int i = 0; i < input_size; ++i)
            sum += hidden_nodes->nodes[j].weights[i] * data.at(i);

        dataSpecificOutputNodes.at(j) = sigmoid(sum);
    }

    for (int k = 0; k < output_size; ++k) {
        sum = 0;

        // Calculate the output value
        for (int j = 0; j < hidden_size; ++j){
            sum += output_nodes->nodes[k].weights[j]
                   * dataSpecificOutputNodes.at(j);
        }

        result.at(k) = sigmoid(sum);
    }
    return result;
}

void nnwork::run(float data [], float result [])
{
    int i, j, k;
    float sum;

    if (input_size <= 0 || hidden_size <= 0 || output_size <= 0) {
        cerr << "nnwork::warning: incorrect dimensions. No action taken."
             << endl;
        cerr << "input_size = " << input_size << endl;
        cerr << "output_size = " << output_size << endl;
        cerr << "hidden_size = " << hidden_size << endl;
        return;
    }

    for (j = 0; j < hidden_size; j++) {
        sum = 0;

        // Calculate the output value
        for (i = 0; i < input_size; i++)
            sum += hidden_nodes -> nodes [j].weights [i] * data [i];
        hidden_nodes -> nodes [j].output = sigmoid (sum);
    }

    for (k = 0; k < output_size; k++) {
        sum = 0;

        // Calculate the output value
        for (j = 0; j < hidden_size; j++)
            sum += output_nodes -> nodes [k].weights [j] *
                   hidden_nodes -> nodes [j].output;

        result [k] = sigmoid (sum);
    }
}

/* 
	Restore the values of the connection weights from a file. Format:

nnlib network weights data file version 1.0
Size: input_nodes hidden_nodes output_nodes
Hidden layer weights:
wh_0,0 wh_0,1 wh_0,2 ... wh_0,input_nodes-1
wh_1,0 wh_1,1 wh_1,2 ... wh_1,input_nodes-1
wh_2,0 wh_2,1 wh_2,2 ... wh_2,input_nodes-1
...
wh_hidden_nodes-1,0 wh_hidden_nodes-1,1 wh_hidden_nodes-1,2 ... wh_hidden_nodes,input_nodes-1
Output layer weights:
wo_0,0 wo_0,1 wo_0,2 ... wo_0,hidden_nodes-1
wo_1,0 wo_1,1 wo_1,2 ... wo_1,hidden_nodes-1
wo_2,0 wo_2,1 wo_2,2 ... wo_2,hidden_nodes-1
...
wo_output_nodes-1,0 wo_output_nodes-1,1 wo_output_nodes-1,2 ... wo_output_nodes-1,hidden_nodes-1


	THERE IS MINIMAL ERROR CHECKING!!! Please don't manually edit the 
	files.
*/

int nnwork::load (char *filename)
{
	ifstream weights (filename);
	int i, j, k;
	int num_input, num_hidden, num_output;
	char temp [50];

	if (weights.good ()) {
		weights.getline (temp, 50, '\n');
		
		if (strcmp (temp, "nnlib network weights data file version 1.0") == 0) {
			weights >> temp >> num_input >> num_hidden >> num_output;

			if (strcmp (temp, "Size:") != 0) {
				cerr << "Error in " << filename << ": no \"Size:\" line." << endl;
				weights.close ();
				return 0;
			}
			
// If required, resize the network	

			input_size = num_input;
			
			if (hidden_size != num_hidden || output_size != num_output) 
				cerr << "Resizing neural network." << endl;

			if (hidden_size != num_hidden) {
				hidden_size = num_hidden;
				if (hidden_nodes) delete hidden_nodes;
				hidden_nodes = new nnlayer (hidden_size, input_size);
				assert (hidden_nodes);
			}
			
			if (output_size != num_output) {
				output_size = num_output;
				if (output_nodes) delete output_nodes;
				output_nodes = new nnlayer (output_size, hidden_size);
				assert (output_nodes);
			}
			
// Read the hidden weights
			weights.getline (temp, 2, '\n');
			weights.getline (temp, 50, '\n');
			
			if (strcmp (temp, "Hidden layer weights:") == 0)
				for (j = 0; j < hidden_size; j++)
					for (i = 0; i < input_size; i++) {
						weights >> hidden_nodes -> nodes [j].weights [i];
						if (!weights.good ()) {
							cerr << "Premature EOF on data file." << endl;
							weights.close ();
							return 0;
						}
					}
			else {
				cerr << "Error in " << filename << ": no  \"Hidden layer weights:\" line." << endl;
				weights.close ();
				return 0;
			}

// And the output weights
			weights.getline (temp, 2, '\n');
			weights.getline (temp, 50, '\n');
			
			if (strcmp (temp, "Output layer weights:") == 0)
				for (k = 0; k < output_size; k++)
					for (j = 0; j < hidden_size; j++) {
						weights >> output_nodes -> nodes [k].weights [j];
						if (!weights.good ()) {
							cerr << "Premature EOF on data file." << endl;
							weights.close ();
							return 0;
						}
					}
			else {
				cerr << "Error in " << filename << ": no  \"Output layer weights:\" line." << endl;
				weights.close ();
				return 0;
			}

			weights.close ();
		} else {
			cerr << "Unsupported file version, or garbage in file." << endl;
			return 0;
		}
		
		return 1;
	} else
		return 0;
}

// Save the current weights to <filename>. Suggested extension is .nnw

int nnwork::save (char *filename)
{
	ofstream weights (filename);
	int i, j, k;

	if (input_size == 0 || hidden_size == 0 || output_size == 0) {
		cerr << "Warning: silly dimensions. No action taken." << endl;
		return 0;
	}

	if (weights.good ()) {
		weights << "nnlib network weights data file version 1.0" << endl;
		weights << "Size: " << input_size << " " << hidden_size << " " << output_size << endl;
		
// Write out the hidden weights

		weights << "Hidden layer weights:" << endl;

		for (j = 0; j < hidden_size; j++) {
			for (i = 0; i < input_size; i++)
				weights << hidden_nodes -> nodes [j].weights [i] << "\t";
			weights << endl;
		}
		
		weights << "Output layer weights:" << endl;

// And the output weights
			
		for (k = 0; k < output_size; k++) {
			for (j = 0; j < hidden_size; j++)
				weights << output_nodes -> nodes [k].weights [j] << "\t";
			weights << endl;
		}

		weights.close ();
		
		return 1;
	} else
		return 0;
}
