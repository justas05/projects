#include "functions.h"

char* read_all_bytes(char* filename, int* read)
{
	ifstream ifs(filename, ios::binary | ios::ate);
	ifstream::pos_type pos = ifs.tellg();

	int length = pos;

	char* p_text = new char[length + 1];
	ifs.seekg(0, ios::beg);
	ifs.read(p_text, length);
	ifs.close();

	p_text[length] = '\0';
	*read = length;
	return p_text;
}