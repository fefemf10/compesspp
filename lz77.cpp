#include "lz77.hpp"

struct Node
{
	unsigned short offset;
	unsigned short length;
	char next;
};
constexpr size_t sizeofNode = 5;

std::vector<unsigned char> lz77::compress(const std::span<const unsigned char> data)
{
	constexpr const size_t maxSizeBuffer = 1024*64;
	size_t sizeBuffer = 0;
	std::vector<unsigned char> out;
	std::vector<unsigned char> buffer(maxSizeBuffer);
	size_t shiftPos{};
	
	auto findMatching = [&](Node& node)
	{
		size_t bestLength{}, bestPos{};
		for (long long i = 0; i < sizeBuffer;)
		{
			long long l{};
			while (shiftPos + l < data.size() && i >= l && buffer[i - l] == data[shiftPos + l])
				++l;
			if (l > bestLength)
			{
				bestPos = i + 1;
				bestLength = l;
			}
			l ? i += l : ++i;
		}
		node.offset = bestPos;
		node.length = bestLength;
	};
	auto shiftBuffer = [&](size_t size)
	{
		sizeBuffer += size;
		if (sizeBuffer > maxSizeBuffer)
			sizeBuffer = maxSizeBuffer;
		for (int i = sizeBuffer - 1; i >= size; i--)
		{
			buffer[i] = buffer[i - size];
		}
		for (int i = (size - 1); i >= 0; i--)
		{
			buffer[i % sizeBuffer] = data[shiftPos + size - 1 - i];
		}
	};
	while (shiftPos <= data.size())
	{
		Node node{};
		findMatching(node);
		if (node.length + shiftPos < data.size())
		{
			node.next = data[node.length + shiftPos];
			shiftBuffer(node.length + 1);
		}
		else
			node.next = '\n';
		shiftPos += node.length + 1;
		out.insert(out.end(), reinterpret_cast<unsigned char*>(&node), reinterpret_cast<unsigned char*>(&node) + sizeofNode);
	}
	return out;
}

std::vector<unsigned char> lz77::decompress(const std::span<const unsigned char> data)
{
	const size_t countNode = data.size() / sizeofNode;
	std::vector<unsigned char> out;
	for (size_t i = 0; i < countNode; i++)
	{
		const Node* node = reinterpret_cast<const Node*>(&data[i * sizeofNode]);
		size_t start{};
		size_t s{};
		if (node->length > 0)
		{
			s = out.size();
			start = out.size() - node->offset;

			for (size_t j = 0; j < node->length; j++)
			{
				out.push_back(out[start + j]);
			}
		}
		if (i != countNode - 1)
			out.push_back(node->next);
	}
	return out;
}
