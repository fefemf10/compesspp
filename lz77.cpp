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
	constexpr const size_t maxSizeBuffer = 5;
	size_t sizeBuffer{};
	std::vector<unsigned char> out;
	size_t shiftPos{};
	size_t shiftBuffer{};
	
	auto findMatching = [&](Node& node)
	{
		size_t bestLength{0}, bestPos{};
		for (long long i = 0; i < sizeBuffer;)
		{
			long long l{};
			while (shiftPos + l < data.size() && i + l % sizeBuffer < sizeBuffer && data[shiftBuffer + i + l % sizeBuffer] == data[shiftPos + l])
				++l;
			if (l >= bestLength && l != 0)
			{
				bestPos = sizeBuffer-i;
				bestLength = l;
			}
			l ? i += l : ++i;
		}
		node.offset = bestPos;
		node.length = bestLength;
	};
	while (shiftPos <= data.size())
	{
		Node node{};
		findMatching(node);
		shiftPos += node.length;
		if (shiftPos < data.size())
		{
			node.next = data[shiftPos];
		}
		else
			node.next = '\n';
		++shiftPos;
		if (sizeBuffer < maxSizeBuffer)
		{
			sizeBuffer += node.length + 1;
			if (sizeBuffer > maxSizeBuffer)
				sizeBuffer = maxSizeBuffer;
		}
		if (sizeBuffer == maxSizeBuffer)
			shiftBuffer = shiftPos - maxSizeBuffer;
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
