int main(){ 
	float a, b;
	
	read a;
	read b;
	
	if( a == b ) {
		print a;
		print " is equal to ";
		print b;
		print "\n";
	}
	
	if( a > b ) {
		print a;
		print " is greater than ";
		print b;
		print "\n";
	}
	
	else {
		print a;
		print " is less than or equal to ";
		print b;
		print "\n";
	}

	return 0;
}


/*
Input:
1.732
1.414

Output:
1.732 is greater than 1.414
*/

