#include "elbrus_cypher.h"
#include "functions.h"

const int elbrus_cypher::permut[16] = { 6, 12, 15, 9, 2, 0, 11, 13, 14, 4, 7, 1, 10, 8, 3, 5 };
const int elbrus_cypher::revers_permut[16] = { 5, 11, 4, 14, 9, 15, 0, 10, 13, 3, 12, 6, 1, 7, 8, 2 };

/*map<char, int> Square = {
{ 'a', 0x02 }, { 'h', 0x01 }, { 'o', 0x04 }, { 'v', 0x0D },
{ 'b', 0x13 }, { 'i', 0x09 }, { 'p', 0x07 }, { 'w', 0x15 },
{ 'c', 0x12 }, { 'j', 0x16 }, { 'q', 0x0F }, { 'x', 0x17 },
{ 'd', 0x0A }, { 'k', 0x0B }, { 'r', 0x05 }, { 'y', 0x11 },
{ 'e', 0x08 }, { 'l', 0x06 }, { 's', 0x10 }, { 'z', 0x19 },
{ 'f', 0x14 }, { 'm', 0x18 }, { 't', 0x00 }, { '*', 0x1F },
{ 'g', 0x0E }, { 'n', 0x03 }, { 'u', 0x0C }
};*/

map<char, int> elbrus_cypher::Square = map<char, int>{
	{ 'A', 0x02 }, { 'H', 0x01 }, { 'O', 0x04 }, { 'V', 0x0D },
	{ 'B', 0x13 }, { 'I', 0x09 }, { 'P', 0x07 }, { 'W', 0x15 },
	{ 'C', 0x12 }, { 'J', 0x16 }, { 'Q', 0x0F }, { 'X', 0x17 },
	{ 'D', 0x0A }, { 'K', 0x0B }, { 'R', 0x05 }, { 'Y', 0x11 },
	{ 'E', 0x08 }, { 'L', 0x06 }, { 'S', 0x10 }, { 'Z', 0x19 },
	{ 'F', 0x14 }, { 'M', 0x18 }, { 'T', 0x00 }, { '*', 0x1F },
	{ 'G', 0x0E }, { 'N', 0x03 }, { 'U', 0x0C }
};

map<int, char> elbrus_cypher::reverse_Square = map<int, char>{
	{ 0x02, 'A' }, { 0x01, 'H' }, { 0x04, 'O' }, { 0x0D, 'V' },
	{ 0x13, 'B' }, { 0x09, 'I' }, { 0x07, 'P' }, { 0x15, 'W' },
	{ 0x12, 'C' }, { 0x16, 'J' }, { 0x0F, 'Q' }, { 0x17, 'X' },
	{ 0x0A, 'D' }, { 0x0B, 'K' }, { 0x05, 'R' }, { 0x11, 'Y' },
	{ 0x08, 'E' }, { 0x06, 'L' }, { 0x10, 'S' }, { 0x19, 'Z' },
	{ 0x14, 'F' }, { 0x18, 'M' }, { 0x00, 'T' }, { 0x1F, '*' },
	{ 0x0E, 'G' }, { 0x03, 'N' }, { 0x0C, 'U' }
};

elbrus_cypher::elbrus_cypher()
{
	long_key = new char[size_k] { '\0' };
	reg = new char[size_r] { '\0' };
	key = new char[size_k] { '\0' };
}
elbrus_cypher::~elbrus_cypher()
{
	delete[] long_key;
	delete[] key;
	delete[] reg;
}

int elbrus_cypher::check_msg(char** msg)
{
	int msg_len = strlen(*msg);
	if (msg_len % fold)
	{
		int temp = (msg_len / fold + 1) * fold;
		char *str = new char[temp + 1];
		memset(str, '*', temp);
		for (int i(0); i < msg_len; str[i] = (*msg)[i], ++i);
		str[temp] = '\0';
		*msg = str;
		return temp;
	}

	return msg_len;
}

void elbrus_cypher::encrypt_block(char *block, char *out)
{
	char *p_K = key;
	char *p_R = block;

	for (int l(0); l < size_k; ++l)
	{
		*p_R = (char)(permut[((*p_K + std::accumulate(block, block + size_r, 0)) & 0xF)]);

		p_R = ((l + 1) % size_r) ? p_R + 1 : block;
		++p_K;
	}

	for (int j(0); j < size_r; ++j)
	{
		out[j] = *p_R;
		++p_R;

		if (p_R > block + size_r - 1)
			p_R = block;
	}
}
void elbrus_cypher::encrypt(char *msg, char **out, int *len)
{
	*len = check_msg(&msg);
	*out = new char[*len / fold * size_r];

	char *p_msg = msg;
	char *p_ct = *out;

	int i(0);
	for (; i < *len / fold; ++i, p_msg += fold, p_ct += size_r)
	{
		int temp(0);
		for (int k(0); k < fold; ++k)
			temp |= (Square[p_msg[k]] << 32 - 5 * (k + 1));



		for (int k(0); k < size_r; ++k)
			p_ct[k] = (char)((temp >> 28 - 4 * k) & (0xF));

		encrypt_block(p_ct, p_ct);
	}

	*len = *len / fold * size_r;
}


void elbrus_cypher::decrypt_block(char *block, char *out)
{
	char *p_K = key + size_k - 1;
	char *p_R = block + size_r - 1;

	for (int l(0); l < size_k; ++l)
	{
		char temp = accumulate(block, block + size_r, 0) - *p_R;

		*p_R = ((revers_permut[*p_R] - *p_K - temp) + 160) & 0xF;

		p_R = ((l + 1) % size_r) ? p_R - 1 : block + size_r - 1;
		--p_K;
	}

	for (int j(0); j < size_r; ++j)
	{
		++p_R;

		if (p_R > block + size_r - 1)
			p_R = block;
		out[j] = *p_R;
	}
}
void elbrus_cypher::decrypt(char *msg, char **out, int *len)
{
	*out = new char[*len];

	char *p_msg = msg;
	char *p_ot = *out;

	int i(0);
	for (; i < *len / size_r; ++i, p_msg += size_r, p_ot += size_r)
		decrypt_block(p_msg, p_ot);

	char *p_text_writer = *out;
	p_ot = *out;
	for (int i(0); i < *len / size_r; ++i)
	{
		int temp(0);
		for (int k(0); k < size_r; ++k, ++p_ot)
			temp |= (*p_ot << 28 - 4 * k);

		for (int k(0); k < fold; ++k, ++p_text_writer)
			*p_text_writer = reverse_Square[(int)(temp >> (32 - 5 * (k + 1))) & 0x1F];
	}
	*p_text_writer = '\0';
	*len = strlen(*out);
}

void elbrus_cypher::write_key_to_file(char* filename)
{
	ofstream of(filename, ios::binary | ios::ate);
	of.write(key, size_k);
	of.close();
}
void elbrus_cypher::generate_key(char* filename, char* filename_y)
{
	int pass_len;
	char *pass = read_password_from_file(filename, &pass_len);
	char *out = new char[pass_len];
	swap(key, long_key);

	for (int i(0); i < size_k;)
	{
		for (int j(0); j < size_r; ++j)
			long_key[i++] = pass[j];

		encrypt_block(pass, out);
		swap(pass, out);
	}
	swap(key, long_key);

	ofstream of(filename_y, ios::binary | ios::ate);
	of.write(key, size_k);
	of.close();

	for (int i(0); i < size_k; ++i)
		key[i] ^= long_key[i];

	delete out;
}

void elbrus_cypher::read_long_key_from_file(char* filename)
{
	int key_len;
	char* key = read_all_bytes(filename, &key_len);

	if (key_len < size_k / 2)
		throw new exception();

	for (int i(0); i < size_k; ++i)
		long_key[i] = (key[i / 2] >> 4 * (1 - i % 2)) & 0xF;
}
char* elbrus_cypher::read_password_from_file(char* filename, int *len)
{
	char* pass = read_all_bytes(filename, len);

	if (*len < size_r / 2)
		throw new exception();

	return pass;
}
char* elbrus_cypher::read_msg_from_file(char* filename, int *len)
{
	return read_all_bytes(filename, len);
}

void elbrus_cypher::write_long_key_to_file()
{
	int len_k = 20;
	char *K = new char[len_k];
	K[0] = 0x45; K[5] = 0x00; K[10] = 0x80; K[15] = 0x80;
	K[1] = 0x00; K[6] = 0x3E; K[11] = 0x00; K[16] = 0x00;
	K[2] = 0x00; K[7] = 0x00; K[12] = 0x00; K[17] = 0x00;
	K[3] = 0x6C; K[8] = 0x00; K[13] = 0x00; K[18] = 0x00;
	K[4] = 0x00; K[9] = 0x1A; K[14] = 0x02; K[19] = 0x0F;

	ofstream of("longest_key160", ios::binary | ios::out);
	of.write(K, len_k);
	of.close();
}
void elbrus_cypher::write_pass_to_file()
{
	int pass_len = 8;
	char *password = new char[pass_len];
	password[0] = 0x5;
	password[1] = 0x1;
	password[2] = 0xE;
	password[3] = 0x5;
	password[4] = 0x0;
	password[5] = 0x7;
	password[6] = 0x0;
	password[7] = 0x3;

	ofstream of("password", ios::binary | ios::out);
	of.write(password, pass_len);
	of.close();
}

struct mid
{
	char **a;
	vector<char> d;
};

void elbrus_cypher::decypher(char* filename_ot, char* filename_ct)
{
	int size_ot(0);
	int size_ct(0);

	char *ot = read_all_bytes(filename_ot, &size_ot);
	char *ct = read_all_bytes(filename_ct, &size_ct);

	int blocks(size_ot / size_r);

	int size_ot_regs(size_ot);
	int size_ct_regs(size_ct);

	char *ot_regs = new char[size_ot_regs];
	char *ct_regs = new char[size_ct_regs];

	for (int i(0); i < size_ot_regs; ++i)
	{
		//ot_regs[i] = (ot[i / 2] >> 4 * (1 - i % 2)) & 0xF;
		//ct_regs[i] = (ct[i / 2] >> 4 * (1 - i % 2)) & 0xF;
		ot_regs[i] = ot[i];
		ct_regs[i] = ct[i];
	}

	list<mid*> list_d;
	vector<char> d = vector<char>{ 1, 3, 0, 0, 0, 2, 2, 0, 0, 0, 0, 3, 3, 2, 0, 0, 0, 1, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 3 };
	char **a = new char*[blocks];
	for (int i(0); i < blocks; ++i)
		a[i] = new char[49];

	for (int i(0); i < blocks; ++i)
		for (int j(0); j < size_r; ++j)
			a[i][41 + j] = (ct_regs + i * size_r)[j];

	int up;
	int count_delt(0);
	char mod = 4;
	for (int i(0); i < mod; ++i)
	{
		key[39] = i;

		for (int j(0); j < blocks; ++j)
			a[j][40] = (((revers_permut[a[j][48]] % mod - accumulate(&a[j][41], &a[j][47], 0) - key[39]) % mod) + mod) % mod;

		up = (1 << 20);
		for (int temp(0); temp < up; ++temp)
		{
			/*d[7] = (temp >> 0) % mod;
			d[15] = (temp >> 2) % mod;
			d[16] = (temp >> 4) % mod;
			d[23] = (temp >> 6) % mod;
			d[24] = (temp >> 8) % mod;
			d[25] = (temp >> 10) % mod;
			d[31] = (temp >> 12) % mod;
			d[32] = (temp >> 14) % mod;
			d[33] = (temp >> 16) % mod;
			d[34] = (temp >> 18) % mod;*/

			bool flag = true;
			for (int j(0); j < blocks; ++j)
			{
				a[j][32] = (((a[j][40] + revers_permut[a[j][40]] % mod - revers_permut[a[j][41]] % mod + d[31]) % mod) + mod) % mod;
				a[j][33] = (((a[j][41] + revers_permut[a[j][41]] % mod - revers_permut[a[j][42]] % mod + d[32]) % mod) + mod) % mod;
				a[j][34] = (((a[j][42] + revers_permut[a[j][42]] % mod - revers_permut[a[j][43]] % mod + d[33]) % mod) + mod) % mod;
				a[j][35] = (((a[j][43] + revers_permut[a[j][43]] % mod - revers_permut[a[j][44]] % mod + d[34]) % mod) + mod) % mod;

				a[j][24] = (((a[j][32] + revers_permut[a[j][32]] % mod - revers_permut[a[j][33]] % mod + d[23]) % mod) + mod) % mod;
				a[j][25] = (((a[j][33] + revers_permut[a[j][33]] % mod - revers_permut[a[j][34]] % mod + d[24]) % mod) + mod) % mod;
				a[j][26] = (((a[j][34] + revers_permut[a[j][34]] % mod - revers_permut[a[j][35]] % mod + d[25]) % mod) + mod) % mod;

				a[j][16] = (((a[j][24] + revers_permut[a[j][24]] % mod - revers_permut[a[j][25]] % mod + d[15]) % mod) + mod) % mod;
				a[j][17] = (((a[j][25] + revers_permut[a[j][25]] % mod - revers_permut[a[j][26]] % mod + d[16]) % mod) + mod) % mod;

				a[j][8] = (((a[j][16] + revers_permut[a[j][16]] % mod - revers_permut[a[j][17]] % mod + d[7]) % mod) + mod) % mod;

				if (a[j][8] != ((ot_regs + j * size_r)[7] % mod))
					flag = false;
			}

			if (flag)
			{
				char **na = new char*[blocks];
				for (int i(0); i < blocks; ++i)
				{
					na[i] = new char[49];
					for (int z(0); z < 49; ++z, na[i][z] = a[i][z]);
				}
				mid *m = new mid();
				m->a = na;
				m->d = d;

				list_d.push_back(m);
			}
		}
	}

	count_delt = list_d.size();
	for (int q(0); q < count_delt; ++q)
	{
		list<mid*>::iterator it = list_d.begin();

		up = (1 << 10);
		for (int temp(0); temp < up; ++temp)
		{
			/*(*it)->d[38] = ((temp >> 0) % mod);
			(*it)->d[30] = ((temp >> 2) % mod);
			(*it)->d[22] = ((temp >> 4) % mod);
			(*it)->d[14] = ((temp >> 6) % mod);
			(*it)->d[6] = ((temp >> 8) % mod);*/


			/*ofstream e("temp_d", ios::app);
			for (int p(0); p < (*it)->d.size(); ++p)
				e << +(*it)->d[p] << " ";
			e << endl;
			/*e << temp << " ";
			e << " d[38] " << ((temp >> 0) % mod);
			e << " d[30] " << ((temp >> 2) % mod);
			e << " d[22] " << ((temp >> 4) % mod);
			e << " d[14] " << ((temp >> 6) % mod);
			e << " d[6] " << ((temp >> 8) % mod);
			e << endl;
			e.close();*/


			bool flag = true;
			for (int j(0); j < blocks; ++j)
			{
				(*it)->a[j][39] = ((((*it)->a[j][47] + revers_permut[(*it)->a[j][47]] % mod - revers_permut[(*it)->a[j][48]] % mod  + (*it)->d[38]) % mod) + mod) % mod;
				(*it)->a[j][31] = ((((*it)->a[j][39] + revers_permut[(*it)->a[j][39]] % mod - revers_permut[(*it)->a[j][40]] % mod  + (*it)->d[30]) % mod) + mod) % mod;
				(*it)->a[j][23] = ((((*it)->a[j][31] + revers_permut[(*it)->a[j][31]] % mod - revers_permut[(*it)->a[j][32]] % mod  + (*it)->d[22]) % mod) + mod) % mod;
				(*it)->a[j][15] = ((((*it)->a[j][23] + revers_permut[(*it)->a[j][23]] % mod - revers_permut[(*it)->a[j][24]] % mod  + (*it)->d[14]) % mod) + mod) % mod;
				(*it)->a[j][7] = ((((*it)->a[j][15] + revers_permut[(*it)->a[j][15]] % mod - revers_permut[(*it)->a[j][16]] % mod + (*it)->d[6]) % mod) + mod) % mod;

				if ((*it)->a[j][7] != (ot_regs + j * size_r)[6] % mod)
					flag = false;
			}

			if (flag)
			{
				char **na = new char*[blocks];
				for (int i(0); i < blocks; ++i)
				{
					na[i] = new char[49];
					for (int z(0); z < 49; ++z, na[i][z] = (*it)->a[i][z]);
				}
				mid *m = new mid();
				m->a = na;
				m->d = (*it)->d;

				list_d.push_back(m);
			}
		}
		list_d.pop_front();
	}

	count_delt = list_d.size();
	for (int q(0); q < count_delt; ++q)
	{
		list<mid*>::iterator it = list_d.begin();

		for (int temp(0); temp < (1 << 10); ++temp)
		{
			/*(*it)->d[37] = (temp >> 0) % mod;
			(*it)->d[29] = (temp >> 2) % mod;
			(*it)->d[21] = (temp >> 4) % mod;
			(*it)->d[13] = (temp >> 6) % mod;
			(*it)->d[5] = (temp >> 8) % mod;*/

			bool flag = true;
			for (int j(0); j < blocks; ++j)
			{
				(*it)->a[j][38] = ((((*it)->a[j][46] + revers_permut[(*it)->a[j][46]] % mod  - revers_permut[(*it)->a[j][47]] % mod  + (*it)->d[37]) % mod) + mod) % mod;
				(*it)->a[j][30] = ((((*it)->a[j][38] + revers_permut[(*it)->a[j][38]] % mod  - revers_permut[(*it)->a[j][39]] % mod  + (*it)->d[29]) % mod) + mod) % mod;
				(*it)->a[j][22] = ((((*it)->a[j][30] + revers_permut[(*it)->a[j][30]] % mod  - revers_permut[(*it)->a[j][31]] % mod  + (*it)->d[21]) % mod) + mod) % mod;
				(*it)->a[j][14] = ((((*it)->a[j][22] + revers_permut[(*it)->a[j][22]] % mod  - revers_permut[(*it)->a[j][23]] % mod  + (*it)->d[13]) % mod) + mod) % mod;
				(*it)->a[j][6] = ((((*it)->a[j][14] + revers_permut[(*it)->a[j][14]] % mod - revers_permut[(*it)->a[j][15]] % mod + (*it)->d[5]) % mod) + mod) % mod;

				if ((*it)->a[j][6] != (ot_regs + j * size_r)[5] % mod)
					flag = false;
			}

			if (flag)
			{
				char **na = new char*[blocks];
				for (int i(0); i < blocks; ++i)
				{
					na[i] = new char[49];
					for (int z(0); z < 49; ++z, na[i][z] = (*it)->a[i][z]);
				}
				mid *m = new mid();
				m->a = na;
				m->d = (*it)->d;

				list_d.push_back(m);
			}
		}
		list_d.pop_front();
	}

	count_delt = list_d.size();
	for (int q(0); q < count_delt; ++q)
	{
		list<mid*>::iterator it = list_d.begin();
		for (int temp(0); temp < (1 << 10); ++temp)
		{
			/*(*it)->d[36] = (temp >> 0) % mod;
			(*it)->d[28] = (temp >> 2) % mod;
			(*it)->d[20] = (temp >> 4) % mod;
			(*it)->d[12] = (temp >> 6) % mod;
			(*it)->d[4] = (temp >> 8) % mod;*/

			bool flag = true;
			for (int j(0); j < blocks; ++j)
			{
				(*it)->a[j][37] = ((((*it)->a[j][45] + revers_permut[(*it)->a[j][45]] % mod  - revers_permut[(*it)->a[j][46]] % mod  + (*it)->d[36]) % mod) + mod) % mod;
				(*it)->a[j][29] = ((((*it)->a[j][37] + revers_permut[(*it)->a[j][37]] % mod  - revers_permut[(*it)->a[j][38]] % mod  + (*it)->d[28]) % mod) + mod) % mod;
				(*it)->a[j][21] = ((((*it)->a[j][29] + revers_permut[(*it)->a[j][29]] % mod  - revers_permut[(*it)->a[j][30]] % mod  + (*it)->d[20]) % mod) + mod) % mod;
				(*it)->a[j][13] = ((((*it)->a[j][21] + revers_permut[(*it)->a[j][21]] % mod  - revers_permut[(*it)->a[j][22]] % mod  + (*it)->d[12]) % mod) + mod) % mod;
				(*it)->a[j][5] = ((((*it)->a[j][13] + revers_permut[(*it)->a[j][13]] % mod - revers_permut[(*it)->a[j][14]] % mod + (*it)->d[4]) % mod) + mod) % mod;

				if ((*it)->a[j][5] != (ot_regs + j * size_r)[4] % mod)
					flag = false;
			}

			if (flag)
			{
				char **na = new char*[blocks];
				for (int i(0); i < blocks; ++i)
				{
					na[i] = new char[49];
					for (int z(0); z < 49; ++z, na[i][z] = (*it)->a[i][z]);
				}
				mid *m = new mid();
				m->a = na;
				m->d = (*it)->d;

				list_d.push_back(m);
			}
		}
		list_d.pop_front();
	}

	count_delt = list_d.size();
	for (int q(0); q < count_delt; ++q)
	{
		list<mid*>::iterator it = list_d.begin();
		for (int temp(0); temp < (1 << 10); ++temp)
		{
			/*(*it)->d[35] = (temp >> 0) % mod;
			(*it)->d[27] = (temp >> 2) % mod;
			(*it)->d[19] = (temp >> 4) % mod;
			(*it)->d[11] = (temp >> 6) % mod;
			(*it)->d[3] = (temp >> 8) % mod;*/

			bool flag = true;
			for (int j(0); j < blocks; ++j)
			{
				(*it)->a[j][36] = ((((*it)->a[j][44] + revers_permut[(*it)->a[j][44]] % mod  - revers_permut[(*it)->a[j][45]] % mod  + (*it)->d[35]) % mod) + mod) % mod;
				(*it)->a[j][28] = ((((*it)->a[j][36] + revers_permut[(*it)->a[j][36]] % mod  - revers_permut[(*it)->a[j][37]] % mod  + (*it)->d[27]) % mod) + mod) % mod;
				(*it)->a[j][20] = ((((*it)->a[j][28] + revers_permut[(*it)->a[j][28]] % mod  - revers_permut[(*it)->a[j][29]] % mod  + (*it)->d[19]) % mod) + mod) % mod;
				(*it)->a[j][12] = ((((*it)->a[j][20] + revers_permut[(*it)->a[j][20]] % mod  - revers_permut[(*it)->a[j][21]] % mod  + (*it)->d[11]) % mod) + mod) % mod;
				(*it)->a[j][4] = ((((*it)->a[j][12] + revers_permut[(*it)->a[j][12]] % mod - revers_permut[(*it)->a[j][13]] % mod + (*it)->d[3]) % mod) + mod) % mod;

				if ((*it)->a[j][4] != (ot_regs + j * size_r)[3] % mod)
					flag = false;
			}

			if (flag)
			{
				char **na = new char*[blocks];
				for (int i(0); i < blocks; ++i)
				{
					na[i] = new char[49];
					for (int z(0); z < 49; ++z, na[i][z] = (*it)->a[i][z]);
				}
				mid *m = new mid();
				m->a = na;
				m->d = (*it)->d;

				list_d.push_back(m);
			}
		}
		list_d.pop_front();
	}

	count_delt = list_d.size();
	for (int q(0); q < count_delt; ++q)
	{
		list<mid*>::iterator it = list_d.begin();
		for (int temp(0); temp < (1 << 8); ++temp)
		{
			/*(*it)->d[26] = (temp >> 0) % mod;
			(*it)->d[18] = (temp >> 2) % mod;
			(*it)->d[10] = (temp >> 4) % mod;
			(*it)->d[2] = (temp >> 6) % mod;*/

			bool flag = true;
			for (int j(0); j < blocks; ++j)
			{
				(*it)->a[j][27] = ((((*it)->a[j][35] + revers_permut[(*it)->a[j][35]] % mod  - revers_permut[(*it)->a[j][36]] % mod  + (*it)->d[26]) % mod) + mod) % mod;
				(*it)->a[j][19] = ((((*it)->a[j][27] + revers_permut[(*it)->a[j][27]] % mod  - revers_permut[(*it)->a[j][28]] % mod  + (*it)->d[18]) % mod) + mod) % mod;
				(*it)->a[j][11] = ((((*it)->a[j][19] + revers_permut[(*it)->a[j][19]] % mod  - revers_permut[(*it)->a[j][20]] % mod  + (*it)->d[10]) % mod) + mod) % mod;
				(*it)->a[j][3] = ((((*it)->a[j][11] + revers_permut[(*it)->a[j][11]] % mod - revers_permut[(*it)->a[j][12]] % mod + (*it)->d[2]) % mod) + mod) % mod;

				if ((*it)->a[j][3] != (ot_regs + j * size_r)[2] % mod)
					flag = false;
			}

			if (flag)
			{
				char **na = new char*[blocks];
				for (int i(0); i < blocks; ++i)
				{
					na[i] = new char[49];
					for (int z(0); z < 49; ++z, na[i][z] = (*it)->a[i][z]);
				}
				mid *m = new mid();
				m->a = na;
				m->d = (*it)->d;

				list_d.push_back(m);
			}
		}
		list_d.pop_front();
	}

	count_delt = list_d.size();
	for (int q(0); q < count_delt; ++q)
	{
		list<mid*>::iterator it = list_d.begin();
		for (int temp(0); temp < (1 << 6); ++temp)
		{
			/*(*it)->d[17] = (temp >> 0) % mod;
			(*it)->d[9] = (temp >> 2) % mod;
			(*it)->d[1] = (temp >> 4) % mod;*/

			bool flag = true;
			for (int j(0); j < blocks; ++j)
			{
				(*it)->a[j][18] = ((((*it)->a[j][26] + revers_permut[(*it)->a[j][26]] % mod - revers_permut[(*it)->a[j][27]] % mod + (*it)->d[17]) % mod) + mod) % mod;
				(*it)->a[j][10] = ((((*it)->a[j][18] + revers_permut[(*it)->a[j][18]] % mod - revers_permut[(*it)->a[j][19]] % mod + (*it)->d[9]) % mod) + mod) % mod;
				(*it)->a[j][2] = ((((*it)->a[j][10] + revers_permut[(*it)->a[j][10]] % mod - revers_permut[(*it)->a[j][11]] % mod + (*it)->d[1]) % mod) + mod) % mod;

				if ((*it)->a[j][2] != (ot_regs + j * size_r)[1] % mod)
					flag = false;
			}

			if (flag)
			{
				char **na = new char*[blocks];
				for (int i(0); i < blocks; ++i)
				{
					na[i] = new char[49];
					for (int z(0); z < 49; ++z, na[i][z] = (*it)->a[i][z]);
				}
				mid *m = new mid();
				m->a = na;
				m->d = (*it)->d;

				list_d.push_back(m);
			}
		}
		list_d.pop_front();
	}

	count_delt = list_d.size();
	for (int q(0); q < count_delt; ++q)
	{
		list<mid*>::iterator it = list_d.begin();
		for (int temp(0); temp < (1 << 4); ++temp)
		{
			/*(*it)->d[8] = (temp >> 0) % mod;
			(*it)->d[0] = (temp >> 2) % mod;*/

			bool flag = true;
			for (int j(0); j < blocks; ++j)
			{
				(*it)->a[j][9] = ((((*it)->a[j][17] + revers_permut[(*it)->a[j][17]] % mod - revers_permut[(*it)->a[j][18]] % mod + (*it)->d[8]) % mod) + mod) % mod;
				(*it)->a[j][1] = ((((*it)->a[j][9] + revers_permut[(*it)->a[j][9]] % mod - revers_permut[(*it)->a[j][10]] % mod + (*it)->d[0]) % mod) + mod) % mod;

				if ((*it)->a[j][1] != (ot_regs + j * size_r)[0] % mod)
					flag = false;
			}

			if (flag)
			{
				char **na = new char*[blocks];
				for (int i(0); i < blocks; ++i)
				{
					na[i] = new char[49];
					for (int z(0); z < 49; ++z, na[i][z] = (*it)->a[i][z]);
				}
				mid *m = new mid();
				m->a = na;
				m->d = (*it)->d;

				list_d.push_back(m);
			}
		}
		list_d.pop_front();

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	}


	delete ot;
	delete ct;

	delete ot_regs;
	delete ct_regs;

	for (int i(0); i < blocks; ++i)
		delete a[i];
	delete[] a;
}
