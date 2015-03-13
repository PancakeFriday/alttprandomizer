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
	uint skip;
	uint bitAnd[2];
	uint test;
	vector<uint> adrList;
};

char* randEntrances(char* content, ranVal obj)
{

}

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
	// it's more intuitive to write it the other way
	char* t = content;
	const int randoNum = indices.size();
	vector<string> randoVec;
	if(obj.max != 0)
	{
		if((obj.max-obj.min+1)%obj.size != 0)
		{
			cout << "Limits are not dividable, this might not work as expected!" << endl;
		}
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
			// need to skip bytes?
			if((j+1)%(obj.skip+1) != 0)
				continue;

			// bitwise and bytes
			if(obj.bitAnd[0] != 0 && obj.bitAnd[1] != 0)
			{
				if((randoVec[indices[i]][obj.bitAnd[0]-1] & obj.bitAnd[1]) == 0)
				{
					cout << "skipped item" << endl;
					//continue;
				}
			}

			t[obj.min+i*obj.size + j] = randoVec[indices[i]][j];
		}
	}

	cout << "Randomizing completed from " << hex << "0x" << obj.min << " to " << "0x" << obj.max << " with size " << "0x" << obj.size << endl;

	return t;
}

char* randomList(char* content, ranVal obj, vector<int> indices)
{
	char* t = content;
	vector<uint> values(indices.size());
	int i=0;
	for(int& x : indices)
	{
		values[x] = t[obj.adrList[i]];
		++i;
	}
	i=0;
	for(uint& x: obj.adrList)
	{
		t[x] = values[i];
		++i;
	}

	cout << "Randomizing completed for list" << endl;

	return t;
}

map<int, ranVal> readConfig(string filename)
{
	ifstream f;
	f.open(filename.c_str(), ios::in);

	int i = -1;
	map<int, ranVal> t;
	string line;
	if(f.is_open())
	{
		while( getline(f,line) )
		{
			if(line[0] == '/' && line[1] == '/')
			{
				getline(f,line);
				if(line.substr(0,5) == "list:")
				{
					++i;
					int pos = 4;
					while(pos <= line.size()-4)
					{
						t[i].adrList.push_back((int)strtol(line.substr(pos+1,line.find(',',pos+1)-pos-1).c_str(), NULL, 0));
						pos = line.find(',',pos+1);
					}
				}
				else if(line[0] == '0' && line[1] == 'x') // make a range
				{
					++i;
					// convert 0xabcdef to int
					t[i].min = (int)strtol(line.c_str(), NULL, 0);
					getline(f,line);
					t[i].max = (int)strtol(line.c_str(), NULL, 0);
				}
				else
				{
					continue;
				}
				getline(f,line);
				t[i].size = (int)strtol(line.c_str(), NULL, 0);
			}
			if(line.substr(0,5) == "skip:")
			{
				t[i].skip = (int)strtol(line.substr(5,-1).c_str(), NULL, 0);
			}
			if(line.substr(0,4) == "and:")
			{
				t[i].bitAnd[0] = (int)strtol(line.substr(4,line.find("&")).c_str(), NULL, 0);
				t[i].bitAnd[1] = (int)strtol(line.substr(line.find("&")+1,-1).c_str(), NULL, 0);
			}
		}
	}

	return t;
}

char* devMode(char* t)
{
	char* test = t;
	char copyCode[0x3c];
	for(int i=0;i<0x3c;++i)
	{
		copyCode[i] = test[0x2748a+i];
	}
	for(int i=0;i<0x3c;++i)
	{
		test[0x2748a+0x3c+i] = copyCode[i];
	}
	return test;
}

int main(int argc, char** argv)
{
	if(argc < 3)
	{
		cout << "Usage: rando [input rom] [output rom] [-c configfile]" << endl;
		return -1;
	}
	string config = "config";
	if(argc >= 4)
	{
		if(strcmp(argv[3], "-c") != 0)
		{
			cout << "Invalid third argument. Needs to be -c followed by the config file" << endl;
			return -1;
		}
		if(argc == 4)
		{
			cout << "No config file provided." << endl;
			return -1;
		}
		config = argv[4];
	}
	// ********* INITIALIZATION ************ //
	bool dev = true;
	uint vecSize;
	map<int, ranVal> Values = readConfig(config);
	if(Values[0].max != 0)
	{
		vecSize = (Values[0].max - Values[0].min) / Values[0].size;
	}
	else if(Values[0].adrList.size() != 0)
	{
		vecSize = Values[0].adrList.size();
	}
	else
	{
		cout << "Something seems to be wrong with your config!" << endl;
		return -1;
	}
	vector<int> ind = getIndices(vecSize);
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

		// Dev mode //
		if(dev)
			memblock = devMode(memblock);

		cout << "start loop " << endl;
		for(map<int, ranVal>::iterator it=Values.begin(); it!=Values.end(); ++it)
		{
			if(Values[0].max != 0)
			{
				memblock = randomRange(memblock, it->second, ind);
			}
			else if(Values[0].adrList.size() != 0)
			{
				memblock = randomList(memblock, it->second, ind);
			}
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
