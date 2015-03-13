#include "ranVal.h"
using namespace std;

int main(int argc, char** argv)
{
	if(argc < 3)
	{
		cout << "Usage: rando [input rom] [output rom]" << endl;
		return -1;
	}

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

		ranValWrap ranObj(memblock);
		ranObj.devMode();
		ranObj.randEntrances();

		// Get the content
		memblock = ranObj.getContent();

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
