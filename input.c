main ( )
{
	int a ;
	int b ;
	read ( a ) ;
	read ( b ) ;
	for ( a = 1; a < 10 ; a = a + 1 )
	{
		b = b + 1 ;
		int c ;		
		if ( a > b )
		{
			c = a - 1 ;
		}
		else
		{
			c = a + 1;
		}		
	}
	write ( b ) ;	
}		
