#include "elbrus_cypher.h"
#include "functions.h"

void usage()
{
	cout << "[longest_key160] [password] [generated_key] [ot] [ct] [e|d] [filename_y]" << endl;
	system("PAUSE");
}

int main(int argc, char** argv)
{
	elbrus_cypher cs;
	cs.decypher(argv[1], argv[2]);

	/*if (argc < 8)
	{
		usage();
		return -1;
	}

	elbrus_cypher cs;
	cs.read_long_key_from_file(argv[1]);
	cs.generate_key(argv[2], argv[7]);
	cs.write_key_to_file(argv[3]);

	int msg_len;
	char *msg = read_all_bytes(argv[4], &msg_len);
	
	char *out = NULL;
	int len;

	bool flag = false;
	if (string(argv[6]) == "e")
	{
		cs.encrypt(msg, &out, &len);
		flag = true;
	}
	else if (string(argv[6]) == "d")
	{
		cs.decrypt(msg, &out, &msg_len);
		flag = true;
		len = msg_len;
	}

	if (flag && out != NULL)
	{
		ofstream of(argv[5], ios::binary | ios::ate);
		of.write(out, len);
		of.close();
	}*/

	return 0;
}