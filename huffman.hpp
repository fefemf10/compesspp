#pragma once
#include <vector>
#include <span>
#include <algorithm>
#include <map>
#include <list>
namespace huffman
{
	struct Node
	{
		Node* left{};
		Node* right{};
		size_t freq{};
		char c{};
		Node() = default;
		Node(size_t freq, char c);
		Node(Node* left, Node* right);
		bool operator<(const Node& node) const;
	};
	std::vector<unsigned char> compress(const std::span<const unsigned char> data);
	std::vector<unsigned char> decompress(const std::span<const unsigned char> data);
};