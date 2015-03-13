#include "ranVal.h"

int simplerand (int i) { srand(time(0)); return rand()%i; }

ranValWrap::ranValWrap(char* c) : content(c)
{

}

void ranValWrap::getIndices(uint size)
{
	srand ( unsigned ( std::time(0)  )  );
	for(int i=0; i<size; ++i)
	{
		indices.push_back(i);
	}
	random_shuffle(indices.begin(), indices.end(), simplerand);
}

vector<int> ranValWrap::shuffle(vector<int>unshuffled)
{
	random_shuffle(unshuffled.begin(), unshuffled.end(), simplerand);
	return unshuffled;
}

void ranValWrap::randomList(ranVal obj)
{
	vector<uint> values(indices.size());
	int i=0;
	for(int& x : indices)
	{
		values[x] = content[obj.adrList[i]];
		++i;
	}
	i=0;
	for(uint& x: obj.adrList)
	{
		content[x] = values[i];
		++i;
	}

	cout << "Randomizing completed for list" << endl;
}

map<int, ranValWrap::ranVal> ranValWrap::readConfig(string filename)
{
	ifstream f;
	f.open(filename.c_str(), ios::in);

	int i = -1;
	map<int, ranValWrap::ranVal> t;
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

	numentries = i;

	return t;
}

char* ranValWrap::getContent()
{
	return content;
}

void ranValWrap::devMode()
{
	char copyCode[0x3c];
	for(int i=0;i<0x3c;++i)
	{
		copyCode[i] = content[0x2748a+i];
	}
	for(int i=0;i<0x3c;++i)
	{
		content[0x2748a+0x3c+i] = copyCode[i];
	}
}

void ranValWrap::randomRange(ranVal obj, vector<int> ind_shuffled, vector<int> ind_unshuffled)
{
	// it's more intuitive to write it the other way
	char* t = content;
	const int randoNum = (obj.max-obj.min+1)/obj.size;
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

	bool cont = false;
	uint skip = 0;
	vector<int>::iterator it = ind_shuffled.begin();
	// shake shake
	for(uint i=0; i<randoNum; ++i)
	{
		cont = false;
		// check if the index is not in the list
		for(int k=0; k<ind_shuffled.size(); ++k)
		{
			if(i==ind_shuffled[k])
				cont = true;
		}
		if(!cont)
		{
			cout << "skip " << i << endl;
			++skip;
			continue;
		}

		for(int j=0;j<obj.size;++j)
		{
			t[obj.min+i*obj.size + j] = randoVec[ind_shuffled[i-skip]][j];
		}
		++it;
	}

	cout << "Randomizing completed from " << hex << "0x" << obj.min << " to " << "0x" << obj.max << " with size " << "0x" << obj.size << ", skipped " << skip << " objects" << endl;
}

void ranValWrap::randEntrances()
{
	// so entrances are in the entrance cfg file and exits respectively
	map<int, ranVal> entrances, exits;
	entrances = readConfig("entrances");
	exits = readConfig("exits");

	// as indices, we take all entrances that have no exit
	int numentrances, numexits;
	numentrances = (entrances[0].max-entrances[0].min+1)/entrances[0].size;
	numexits = (exits[0].max-exits[0].min+1)/exits[0].size;

	map<uint, uint> entrances_back;
	string ventr, vexit;
	bool skip;
	char* content_c = content;
	vector<int> unshuffled;

	for(int i=0; i<numentrances; ++i)
	{
		skip = false;
		ventr = content[entrances[i].min+i*entrances[i].size];
		for(int j=0; j<numexits; ++j)
		{
			vexit = content[exits[j].min + j*exits[j].size];
			if(ventr == vexit)
				skip = true;
		}
		if(skip)
		{
			continue;
		}

		unshuffled.push_back(i);
	}

	vector<int> shuffled = shuffle(unshuffled);
	// Randomize all entries in the config
	for(int k=0; k<numentries; ++k)
	{
		randomRange(entrances[k], shuffled, unshuffled);
	}
}
