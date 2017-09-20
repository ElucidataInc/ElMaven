/****************************************************************************

    Neural Network Library example program: a simple character classifier. 
    This program works out some stats for the character recogniser. We
    assume you have already generated a char_recognition.nnw with the
    char_recognition program.
    
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
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <time.h>
//#define WAFFLY

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

void usage ()
{
	cerr << "Usage: char_stats Num_iterations Error_range\n";
}

int main (int argc, char **argv)
{ // you can safely adjust the size of the hidden layer (the middle argument)
	nnwork brain ("char_recognition.nnw");

	if (!brain.get_layersize (ALL)) {
		cerr << "Error: File problems.\n";
		return -1;
	}

	float result [26], data [35];
	float range, max;
	int max_idx, i, j, k, N, emax = 0, emin = INT_MAX, etotal = 0;
	int errors [26];
	
	srandom (time (0));
	
// Pick a letter

	if (argc != 3) {
		cerr << "Error: wrong number of args!\n";
		usage ();
		return -1;
	}

	if (!sscanf (argv [1], "%i", &N)) {
		cerr << "Error: expected integer argument!\n";
		usage ();
		return -1;
	}

	if (!sscanf (argv [2], "%f", &range)) {
		cerr << "Error: expected float argument!\n";
		usage ();
		return -1;
	}
	
	for (i = 0; i < 26; i++)
		errors [i] = 0;

	for (i = 0; i < N; i++)
		for (j = 0; j < 26; j++) {
			copy_array (data, Alphabet [j], 35);
			noisify (data, 35, range);

			brain.run (data, result);

			max = 0.;
			max_idx = 0;
	
			for (k = 0; k < 26; k++)
				if (result [k] > max) {
					max = result [k];
					max_idx = k;
				}
				
			if (max_idx != j)
				errors [j]++;
		}

// Print out the cumulative errors for N runs. 

#ifdef WAFFLY
	cout << "Errors:\n";
#endif

	for (i = 0; i < 26; i++) {
#ifdef WAFFLY
		cout << char (i + 'A') << ": " << errors [i] << endl;
#endif		
		etotal += errors [i];
		
		if (errors [i] > emax)
			emax = errors [i];
		
		if (errors [i] < emin)
			emin = errors [i];
	}
	
#ifdef WAFFLY	
	cout << "Average proportion of errors: " << etotal / float (26 * N) << endl;
	cout << "Maximum proportion of errors: " << emax / float (N) << endl;
	cout << "Minimum proportion of errors: " << emin / float (N) << endl;
#else
	cout << range << "\t" << etotal / float (26 * N) << "\t" << emax / float (N) << "\t" << emin / float (N) << endl;
#endif
	return 0;
}
