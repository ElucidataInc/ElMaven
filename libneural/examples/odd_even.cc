/****************************************************************************

    Neural Network Library example program: an odd/even number classifier.
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
#include <math.h>
#include <stdlib.h>
#include <time.h>

// The "even" examples (element 0 is the MSB, 4 is the LSB)
float test_data_odd_1 [5] = {1., 0., 0., 1., 1.};
float test_data_odd_2 [5] = {1., 1., 1., 0., 1.};
float test_data_odd_3 [5] = {0., 0., 0., 1., 1.};
float test_data_odd_4 [5] = {0., 1., 0., 0., 1.};
float test_data_odd_5 [5] = {0., 1., 0., 1., 1.};
float test_data_odd_6 [5] = {1., 1., 1., 1., 1.};

// The "odd" examples
float test_data_even_1 [5] = {0., 0., 1., 1., 0.};
float test_data_even_2 [5] = {1., 1., 0., 1., 0.};
float test_data_even_3 [5] = {1., 0., 1., 1., 0.};
float test_data_even_4 [5] = {0., 1., 0., 1., 0.};
float test_data_even_5 [5] = {1., 0., 1., 0., 0.};
float test_data_even_6 [5] = {0., 1., 0., 1., 0.};

int main ()
{
	nnwork brain (5, 3, 1);
	float result [1];
	char character;
	
	cout << "Train or read from file (t|f)? ";
	cin >> character;

	if (character == 't') { // train, do 50 runs.
		for (int i = 0; i < 50; i++) {
			result [0] = 0.9;
// Train with even data		
			brain.train (test_data_even_1, result, 0.0000000005, 0.2);
			brain.train (test_data_even_2, result, 0.0000000005, 0.2);
			brain.train (test_data_even_3, result, 0.0000000005, 0.2);
			brain.train (test_data_even_4, result, 0.0000000005, 0.2);
			brain.train (test_data_even_5, result, 0.0000000005, 0.2);
			brain.train (test_data_even_6, result, 0.0000000005, 0.2);
	
			result [0] = 0.1;

// Train with odd data (counterexample)
			brain.train (test_data_odd_1, result, 0.0000000005, 0.2);
			brain.train (test_data_odd_2, result, 0.0000000005, 0.2);
			brain.train (test_data_odd_3, result, 0.0000000005, 0.2);
			brain.train (test_data_odd_4, result, 0.0000000005, 0.2);
			brain.train (test_data_odd_5, result, 0.0000000005, 0.2);
			brain.train (test_data_odd_6, result, 0.0000000005, 0.2);	

			cout << i + 1 << " training iterations completed.\n";
		}
		brain.save ("odd_even.nnw"); // save the network
	} else if (!brain.load ("odd_even.nnw")) { // try to load from the file
		cerr << "File not found.\n";
		return -1;
	}

// Now run the network with one odd and one even test value.

	cout << "Odd test (should be 0.1):\n";
	brain.run (test_data_odd_1, result);
	cout << result [0] << endl;

	cout << "Even test (should be 0.9):\n";
	brain.run (test_data_even_3, result);
	cout << result [0] << endl;

	return 0;
}
