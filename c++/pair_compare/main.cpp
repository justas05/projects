#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <list>

#include <stdlib.h>

#define MAX_LEN_LINE 256

using namespace std;


int read_matrix_from_file(char* filename, vector < vector<double> > & matrix, int size)
{
    ifstream input(filename);

    if (!input.good())
    {
        cout << "Error open file " << filename << endl;
        return -1;
    }
    
    for (int i(0); i < size; ++i)
    {
	    vector<double> line;
	    for (int j(0); j < size; ++j)
	    {
	    	double temp;
	    	input >> temp;
		line.push_back(temp);
	    }
	    matrix.push_back(line);
    }

    input.close();

    return 0;
}

int fill_experts(list<vector<vector <double> > > &experts, int files_count, char **files, int size)
{
    for (int i(0); i < files_count; ++i)
    {
        vector<vector<double> > matrix;
        read_matrix_from_file(files[i], matrix, size);
        experts.push_back(matrix);
    }
    return 0;
}

int usage()
{
    cout << "./a.out [count] [diff] [count_objects] [files...]" << endl;
    return 0;
}

int main(int argc, char** argv)
{
    list<vector < vector<double> > > experts;

    if (argc < 5)
    {
        usage();
        return -1;
    }

    int iterations = atoi(argv[1]);
    double diff = atof(argv[2]);
    int count_objects = atoi(argv[3]);

    fill_experts(experts, argc - 4, &argv[4], count_objects);
    
    return 0;
}
