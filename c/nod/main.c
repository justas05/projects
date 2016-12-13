#include <stdio.h>

void usage()
{
	printf("usage: ./nod [a] [b]\n");
}

int nod(int a, int b)
{
	if (a == b || a%b == 0)
		return b;
	
	return nod(b, a - b * (a / b));
}


int main(int argc, char** argv)
{
	if (argc < 3)
	{
		usage();
		exit(1);
	}
	int a = atoi(argv[1]);
	int b = atoi(argv[2]);

	printf("(%d, %d) = %d\n", a, b, (a > b) ? nod(a,b) : nod(b,a));


	return 0;
}
