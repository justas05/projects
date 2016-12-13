#pragma once

#include <functional>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <numeric>
#include <string>
#include <memory>
#include <vector>
#include <list>
#include <map>

using namespace std;

class elbrus_cypher
{
private:
	static const int fold = 6;						// fold block length (кратность длины блока)
	static const int size_k = 40;					// key length
	static const int size_r = 8;					// register length
	static const int permut[];						// permutation
	static const int revers_permut[];				// revers permutation
	static map<char, int> Square;		// alphabet
	static map<int, char> reverse_Square;

	char* long_key;									// long key
	char* reg;										// register (dont use)
	char* key;										// key

	// check message length and fill it with '*'
	int check_msg(char** msg);

	char* read_password_from_file(char* filename, int* len);

	void write_long_key_to_file();
	void write_pass_to_file();
	
	void encrypt_block(char *block, char *out);
	void decrypt_block(char *block, char *out);
public:
	elbrus_cypher();
	~elbrus_cypher();

	void read_long_key_from_file(char* filename);
	char* read_msg_from_file(char* filename, int *len);

	void generate_key(char* filename, char* filename_y);
	void write_key_to_file(char* filename);
	void encrypt(char *msg, char **out, int *len);
	void decrypt(char *msg, char **out, int *len);

	void decypher(char* filename_ot, char* filename_ct);
};
