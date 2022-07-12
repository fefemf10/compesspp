#include <iostream>
#include <fstream>
#include "lz77.hpp"
#include "lz78.hpp"
int main()
{
	std::ifstream f("file.txt", std::ios::binary | std::ios::in);
	f.seekg(0, std::ios::end);
	std::streampos size{f.tellg()};
	std::vector<unsigned char> in(size);
	f.seekg(0, std::ios::beg);
	f.read(reinterpret_cast<char*>(in.data()), size);
	f.close();
	std::vector<unsigned char> comp = lz78::compress(in);
	std::vector<unsigned char> out = lz78::decompress(comp);
	std::ofstream fc("filec.txt", std::ios::binary | std::ios::out);
	std::ofstream fo("fileout.txt", std::ios::binary | std::ios::out);
	fc.write(reinterpret_cast<const char*>(comp.data()), comp.size());
	comp = lz77::compress(in);
	std::ofstream fcl("filecl.txt", std::ios::binary | std::ios::out);
	fcl.write(reinterpret_cast<const char*>(comp.data()), comp.size());
	fo.write(reinterpret_cast<const char*>(out.data()), out.size());
	fc.close();
	fo.close();
	fcl.close();
	return 0;
}