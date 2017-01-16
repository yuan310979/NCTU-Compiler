int intvalue;
float floatvalue;

int main() {
	int a, b, c;
	bool d;
	float e, f;
	
	// read 3 integer and 2 float
	read a;
	read b;
	read c;
	read e;
	read f;
	
	print "--------------------\n";
	
	print c;
	print " ";
	print a;
	print " ";
	print b;
	print " ";
	print e;
	print " ";
	print f;
	print "\n";
	
	// swap b and c, take intvalue as temp storage
	intvalue = b;
	b = c;
	c = intvalue;
	print b;
	print " ";
	print c;
	print "\n";
	
	// swap e and f, take gr as temp storage
	floatvalue = f;
	f = e;
	e = floatvalue;
	print e;
	print " ";
	print f;
	print "\n";
	
	d = true;
	print d;
	print "\n";

	return 0;
}


/*
Input: 
6
28
496
1.4142
3.1415

Output: 
--------------------
496 6 28 1.4142 3.1415
496 28
3.1415 1.4142
true
*/
