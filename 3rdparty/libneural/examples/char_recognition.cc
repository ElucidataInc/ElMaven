/****************************************************************************

    Neural Network Library example program: a simple character classifier.
    Copyright (C) 1998 Daniel Franklin

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 1, or (at your option)
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

 ****************************************************************************/

#include <nnwork.h>
#include <iostream.h>
#include <iomanip.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>

extern float Alphabet [26][35];

// Copy size elements from source []  to destination []

void copy_array (float destination [], float source [], int size)
{
	for (int i = 0; i < size; i++)
		destination [i] = source [i];
}

// Add some noise to data []

void noisify (float data [], int size, float range)
{
	for (int i = 0; i < size; i++)
		data [i] += (range - 2 * range * random () / float (RAND_MAX));
}

// Print the width x height character in data []

void print_char (float data [], int height, int width)
{
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++)
			if (data [j + i * width] >= 0)
				cout << "#";
			else
				cout << " ";
		cout << endl;
	}
	cout << endl;
}

int main ()
{ // you can safely adjust the size of the hidden layer (the middle argument)
	nnwork brain (35, 60, 26);
	float result [26];
	char character;
	float range, max;
	int max_idx, i, j, N;
	unsigned long t;
	float data [35];
	
// Initially, all cell entries other than result [0] are "false"
// note that "result" is used both for training (desired result) and running.

	for (i = 1; i < 26; i++)
		result [i] = 0.1;
		
	srandom (time (0));
	
	cout << "Train or read from file (t|f)? ";
	cin >> character;

// For some reason the network starts learning faster when it starts at A.
// Don't ask me why.

	N = 0;

	if (character == 't') {
		for (i = 0; i < 100; i++) {
				
			if (N == 26) 
				N--;

			result [N] = 0.9;
// Copy one of the letters
			copy_array (data, Alphabet [N], 35);
// Introduce some noise
			noisify (data, 35, 1.0);
// Train the network
			brain.train (data, result, 0.000001, 0.3);
				
			result [N] = 0.1;
			if (!(i % 10)) cout << "Trained run number " << i << endl;
			N = int (26.0 * random () / float (RAND_MAX));
		}
// Save the data
		brain.save ("char_recognition.nnw");

// Or attempt to load the data

	} else if (!brain.load ("char_recognition.nnw")) {
		cerr << "File not found.\n";
		return -1;
	}
	
// Pick a letter

	cout << "Choose a letter (A-Z or a-z): ";
	cin >> character;

	if (!isalpha (character)) {
		cerr << "Error: must be alphabetic character\n";
		return -1;
	}
	
	copy_array (data, Alphabet [toupper (character) - 'A'], 35);
	
	cout << "Max. amplitude of additive noise (suggest 1.5, i.e. +1.5 -> -1.5)? ";
	cin >> range;
	
// Introduce some noise

	noisify (data, 35, range);

	cout << "The character presented to the neural network:\n";

// Display the character + noise

	print_char (data, 7, 5);	

	brain.run (data, result);
	
	max = 0.;
	max_idx = 0;
	
// Get the best match

	for (i = 0; i < 26; i++)
		if (result [i] > max) {
			max = result [i];
			max_idx = i;
		}

	cout << "Best match: " << char ('A' + max_idx) << endl;

	return 0;
}
