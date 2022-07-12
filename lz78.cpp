#include "lz78.hpp"

struct Node
{
	unsigned int length;
	char next;
};
constexpr size_t sizeofNode = 5;

std::vector<unsigned char> lz78::compress(const std::span<const unsigned char> data)
{
	std::map<std::string, unsigned int> dictionary;
	std::string buffer;
	std::vector<unsigned char> out;
	for (size_t i = 0; i < data.size(); i++)
	{
		if (dictionary.contains(buffer + std::string(1, data[i])))
		{
			buffer.append(1, data[i]);
		}
		else
		{
			Node node{ dictionary[buffer], data[i] };
			out.insert(out.end(), reinterpret_cast<unsigned char*>(&node), reinterpret_cast<unsigned char*>(&node) + sizeofNode);
			dictionary[buffer + std::string(1, data[i])] = dictionary.size();
			buffer.clear();
		}
	}
	if (!buffer.empty())
	{
		const char last = *(buffer.crbegin());
		buffer.pop_back();
		Node node{ dictionary[buffer], last };
		out.insert(out.end(), reinterpret_cast<unsigned char*>(&node), reinterpret_cast<unsigned char*>(&node) + sizeofNode);
	}
    return out;
}

std::vector<unsigned char> lz78::decompress(const std::span<const unsigned char> data)
{
	const size_t countNode = data.size() / sizeofNode;
	std::vector<unsigned char> out;
	std::vector<std::string> dictionary{""};
	for (size_t i = 0; i < countNode; i++)
	{
		const Node* node = reinterpret_cast<const Node*>(&data[i * sizeofNode]);
		std::string word = dictionary[node->length] + node->next;
		out.insert(out.end(), word.begin(), word.end());
		dictionary.push_back(word);
	}
    return out;
}
