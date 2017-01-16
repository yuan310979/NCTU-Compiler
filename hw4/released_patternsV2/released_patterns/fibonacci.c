int fibonacci( int n );

int main() {
	int i;

	for( i = 1 ; i <= 20 ; i = i + 1 ) {
		print fibonacci( i );
		print "  ";
	}
	
	print "\n";
	
	return 0;
}

int fibonacci( int n ) {
	if( n == 1 || n == 2 ) {
		return 1;
	}
		
	else {
		return fibonacci( n - 1 ) + fibonacci( n - 2 );
	}

	return 0;
}

/*
Input:
No input

Output:
1  1  2  3  5  8  13  21  34  55  89  144  233  377  610  987  1597  2584  4181  6765
*/
