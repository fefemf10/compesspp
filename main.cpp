#include <iostream>
#include <fstream>
#include <functional>
#include "lz77.hpp"
#include "lz78.hpp"
#include "huffman.hpp"

std::function c = huffman::compress;
std::function c8 = lz77::compress;
std::function d = huffman::decompress;
std::function d8 = lz77::decompress;

std::vector<unsigned char> read(const std::string& name)
{
	std::ifstream f(name, std::ios::binary | std::ios::in);
	f.seekg(0, std::ios::end);
	std::streampos size{ f.tellg() };
	std::vector<unsigned char> in(size);
	f.seekg(0, std::ios::beg);
	f.read(reinterpret_cast<char*>(in.data()), size);
	f.close();
	return in;
}

void write(const std::string& name, const std::span<const unsigned char> data)
{
	std::ofstream f(name, std::ios::binary | std::ios::out);
	f.write(reinterpret_cast<const char*>(data.data()), data.size());
	f.close();
}

int main()
{
	std::vector<unsigned char> in = read("file.txt");
	std::vector<unsigned char> comp = c((in));
	write("filec8.txt", comp);
	write("fileo8.txt", (d(comp)));
	return 0;
}