#pragma once
#include <vector>
#include <span>
#include <map>
#include <string>
namespace lz78
{
	std::vector<unsigned char> compress(const std::span<const unsigned char> data);
	std::vector<unsigned char> decompress(const std::span<const unsigned char> data);
};