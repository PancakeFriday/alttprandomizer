#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <map>
#include <random>
#include <chrono>
#include <stdio.h>
#include <string.h>
#include <math.h>

using namespace std;

int myrandom (int i) { return rand()%i; }

struct ranVal
{
	uint min;
	uint max;
	uint size;
};

vector<int> getIndices(uint size)
{
	srand ( unsigned ( std::time(0)  )  );
	vector<int> t;
	for(int i=0; i<size; ++i)
	{
		t.push_back(i);
	}
	random_shuffle(t.begin(), t.end(), myrandom);

	return t;
}

char* randomRange(char* content, ranVal obj, vector<int> indices)
{
	char* t = content;
	const int randoNum = (obj.max-obj.min+1)/obj.size;
	vector<string> randoVec;
	if((obj.max-obj.min+1)%obj.size != 0)
	{
		cout << "Limits are not dividable, this might not work as expected!" << endl;
	}

	// copy the contents
	string alloc(obj.size, '0');
	for(int i=0; i<randoNum; ++i)
	{
		for(int j=0; j<obj.size; ++j)
		{
			alloc[j] = content[obj.min +i*obj.size+j];
		}
		randoVec.push_back(alloc);
	}

	// shake shake
	for(int i=0; i<randoNum; ++i)
	{
		for(int j=0;j<obj.size;++j)
		{
			t[obj.min+i*obj.size + j] = randoVec[indices[i]][j];
		}
	}

	cout << "Randomizing completed from " << hex << "0x" << obj.min << " to " << "0x" << obj.max << " with size " << "0x" << obj.size << endl;

	return t;
}

map<int, ranVal> readConfig(string filename)
{
	ifstream f;
	f.open(filename.c_str(), ios::in);

	int i = 0;
	map<int, ranVal> t;
	string line;
	if(f.is_open())
	{
		while( getline(f,line) )
		{
			if(line[0] == '0' && line[1] == 'x')
			{
				// convert 0xabcdef to int
				t[i].min = (int)strtol(line.c_str(), NULL, 0);
				getline(f,line);
				t[i].max = (int)strtol(line.c_str(), NULL, 0);
				getline(f,line);
				t[i].size = (int)strtol(line.c_str(), NULL, 0);
				++i;
			}
		}
	}

	return t;
}

int main(int argc, char** argv)
{
	if(argc != 3)
	{
		cout << "Usage: rando [input rom] [output rom]" << endl;
		return -1;
	}
	// ********* INITIALIZATION ************ //
	vector<int> ind = getIndices(133);
	map<int, ranVal> Values = readConfig("config");
	// ********* UNTIL HERE *************** //

	fstream f;
	f.open(argv[1], ios::in | ios::binary | ios::ate);
	streampos size;

	char* memblock;

	if(f.is_open())
	{
		size = f.tellg();
		memblock = new char[size];
		f.seekg(0, ios::beg);
		f.read(memblock, size);
		f.close();

		cout << "start loop " << endl;
		for(map<int, ranVal>::iterator it=Values.begin(); it!=Values.end(); ++it)
		{
			memblock = randomRange(memblock, it->second, ind);
		}
		cout << "end loop" << endl;

		// Write memblock to file
		f.open(argv[2], ios::out | ios::binary);
		f.write(memblock, size);
		f.close();

		delete[] memblock;
	}
	else
		cout << "shit happened" << endl;

	return 0;
}
