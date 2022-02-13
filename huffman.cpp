#include "huffman.hpp"

void buildTable(std::vector<unsigned char>& code, std::map<unsigned char, std::vector<unsigned char>>& table, huffman::Node* node)
{
	if (node->left)
	{
		code.push_back(0);
		buildTable(code, table, node->left);
	}
	if (node->right)
	{
		code.push_back(1);
		buildTable(code, table, node->right);
	}
	if (!node->left && !node->right)
		table[node->c] = code;
	if (!code.empty())
		code.pop_back();
}

std::vector<unsigned char> huffman::compress(const std::span<const unsigned char> data)
{
	const size_t dataSize = data.size();
	std::map<unsigned char, size_t> freqTable;
	for (size_t i = 0; i < dataSize; i++)
		++freqTable[data[i]];
	std::list<Node*> t;
	for (const auto& item : freqTable)
	{
		t.push_back(new Node(item.second, item.first));
	}
	while (t.size() != 1)
	{
		t.sort([](const Node* l, const Node* r){ return l->freq < r->freq; });
		Node* l = t.front();
		t.pop_front();
		Node* r = t.front();
		t.pop_front();
		t.push_back(new Node(l, r));
	}
	std::vector<unsigned char> code;
	std::map<unsigned char, std::vector<unsigned char>> table;
	buildTable(code, table, t.front());
	for (size_t i = 0; i < dataSize; i++)
	{
		code.insert(code.cend(), table[data[i]].cbegin(), table[data[i]].cend());
	}
	const size_t codeSize = code.size();
	for (size_t i = 0; i < codeSize / 8u; i++)
	{
		unsigned char temp{};
		for (size_t j = 0; j < 8; j++)
		{
			temp |= code[i*8+j] << 7 - j;
		}
		code[i] = temp;
	}
	{
		unsigned char temp{};
		for (size_t j = 0; j < codeSize%8u; j++)
		{
			temp |= code[codeSize/8u*8 + j] << 7 - j;
		}
		code[codeSize / 8u] = temp;
	}
	std::vector<unsigned char> tableHeader(3);
	tableHeader[2] = code.size() % 8u;
	code.resize(codeSize/8u + 1);
	
	size_t countBytesHeader{};
	for (auto& [key, value] : table)
	{
		tableHeader.push_back(key);
		tableHeader.push_back(freqTable[key]);
		tableHeader.push_back(freqTable[key] >> 8);
		tableHeader.push_back(freqTable[key] >> 16);
		tableHeader.push_back(freqTable[key] >> 24);
		countBytesHeader += 5;
	}
	tableHeader[0] = countBytesHeader;
	tableHeader[1] = countBytesHeader >> 8;
	
	code.insert(code.cbegin(), tableHeader.cbegin(), tableHeader.cend());
	
	return code;
}

std::vector<unsigned char> huffman::decompress(const std::span<const unsigned char> data)
{
	const size_t countBytesHeader = data[1] << 8 | data[0];
	const size_t countBitsData = data[2];
	std::map<unsigned char, size_t> freqTable;
	for (size_t i = 3; i < countBytesHeader+2; i+=5)
		freqTable[data[i]] = data[i+4] << 24 | data[i+3] << 16 | data[i+2] << 8 | data[i+1];
	std::list<Node*> t;
	for (const auto& item : freqTable)
	{
		t.push_back(new Node(item.second, item.first));
	}
	while (t.size() != 1)
	{
		t.sort([](const Node* l, const Node* r) { return l->freq < r->freq; });
		Node* l = t.front();
		t.pop_front();
		Node* r = t.front();
		t.pop_front();
		t.push_back(new Node(l, r));
	}
	std::vector<unsigned char> code;
	unsigned char temp;
	Node* root = t.front();
	for (size_t i = countBytesHeader + 3; i < data.size() - 1; i++)
	{
		for (int j = 7; j >= 0; --j)
		{
			temp = (data[i] >> j & 0x01);
			if (temp & 0x01)
			{
				if (root->right)
					root = root->right;
			}
			else
			{
				if (root->left)
					root = root->left;
			}
			if (!root->left && !root->right)
			{
				temp = 0;
				code.push_back(root->c);
				root = t.front();
			}
		}
	}
	for (int j = 7; j >= 7 - countBitsData; --j)
	{
		temp = (data[data.size()-1] >> j & 0x01);
		if (temp & 0x01)
		{
			if (root->right)
				root = root->right;
		}
		else
		{
			if (root->left)
				root = root->left;
		}
		if (!root->left && !root->right)
		{
			temp = 0;
			code.push_back(root->c);
			root = t.front();
		}
	}
	return code;
}

huffman::Node::Node(size_t freq, char c) : freq(freq), c(c)
{
}

huffman::Node::Node(Node* left, Node* right) : left(left), right(right), freq(left->freq + right->freq)
{
}

bool huffman::Node::operator<(const Node& node) const
{
	return freq < node.freq;
}
