int intvalue;
float floatvalue;

int main() {
	int integer_a, integer_b, integer_c;
	bool bool_d;
	float float_e, float_f;
	
	// read 3 integer and 2 float
	read integer_a;
	read integer_b;
	read integer_c;
	read float_e;
	read float_f;
	
	print "--------------------\n";
	
	print integer_c;
	print " ";
	print integer_a;
	print " ";
	print integer_b;
	print " ";
	print float_e;
	print " ";
	print float_f;
	print "\n";
	
	// swap integer_b and integer_c, take intvalue as temp storage
	intvalue = integer_b;
	integer_b = integer_c;
	integer_c = intvalue;
	print integer_b;
	print " ";
	print integer_c;
	print "\n";
	
	// swap float_e and float_f, take gr as temp storage
	floatvalue = float_f;
	float_f = float_e;
	float_e = floatvalue;
	print float_e;
	print " ";
	print float_f;
	print "\n";
	
	bool_d = true;
	print bool_d;
	print "\n";

	return 0;
}


/*
Input: 
6 28 496 1.4142 3.1415

Output: 
--------------------
496 6 28 1.4142 3.1415
496 28
3.1415 1.4142
true
*/
