#include <iostream>
#include <fstream>
#include "lz77.hpp"
int main()
{
	std::ifstream f("file.txt");
	std::string s;
	f >> s;
	std::vector<unsigned char> in(s.begin(), s.end());
	lz77::compress(in);
	return 0;
}