#include <stdio.h>
#include <sys/limits.h>
#define MAX_LENGTH 128

void usage()
{
	printf("usage: ./main [num]\n");
}

char* dec_to_bin(float num)
{
	char* res = malloc(MAX_LENGTH);

	int h = (int)num;
	float d = num - h;

	int i = 0;
	for (i = 0; i < CHAR_BIT * sizeof(int); ++i)
		res[CHAR_BIT*sizeof(int) - 1 - i] = ((h >> i) & 1) + '0';
	res[i] = ',';
	for (++i; i < MAX_LENGTH; ++i)
	{
		d *= 2;
		int temp = (int)d;
		res[i] = temp + '0';
		d -= temp;
	}

	return res;
}


int main(int argc, char** argv)
{
	if (argc < 2)
	{
		usage();
		exit(1);
	}
	float num = atof(argv[1]);
	char* result = dec_to_bin(num);


	printf("Number %f to bin is equal %s\n", num, result);

	free(result);
	return 0;
}
