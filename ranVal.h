#ifndef __RANVAL_H
#define __RANVAL_H

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
#include <bitset>

using namespace std;

class ranValWrap
{
	private:
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

		char* content;
		vector<int> indices;
		int numentries;

		void getIndices(uint size);
		vector<int> shuffle(vector<int>unshuffled);
		void randomList(ranVal obj);
		map<int, ranVal> readConfig(string filename);
		void randomRange(ranVal obj, vector<int> ind_shuffled, vector<int> ind_unshuffled=vector<int>());
	public:
		ranValWrap(char* c);
		char* getContent();
		void devMode();
		void randEntrances();

};

#endif /* __RANVAL_H */
