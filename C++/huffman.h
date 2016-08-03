//============================================================================
// Name : huffman.h
// Author : lancer
// Version : 1.0    2015-12-30 08:52:31
// Copyright : made by lancer
// Description : Ansi-style
// Last Modified time : 2015-12-30 08:52:31
//============================================================================

using namespace std;
using namespace Costella;

struct HuffmanNode{
	long weight;
	long parent;
	long lchild;
	long rchild;
};

struct HuffmanToken{
	unsigned char byte;
	long weight;
};

struct HuffmanCode{
	unsigned char byte;
	vector<int> code;
};

typedef vector<HuffmanNode> HuffmanTree;
typedef vector<HuffmanToken> TokenList;
typedef vector<HuffmanCode> HuffmanCodes;

void init_huffman_tree(HuffmanTree &ht, const TokenList &tokens) {
	HuffmanNode default_node = {0,-1,-1,-1};
	TokenList::size_type n;
	ht.clear();
	n = tokens.size();
	ht.assign(2*n-1, default_node);
	for (TokenList::size_type i = 0; i < n; ++i) {
		ht[i].weight = tokens[i].weight;
	}
}

void find_min_weight_positions(const HuffmanTree &ht, long end, long &min_pos1, long &min_pos2) {
	long min_weight1, min_weight2;
	min_weight1 = min_weight2 = numeric_limits<long>::max();
	long i;
	for (i = 0; i < end; ++i) {
		if (ht[i].parent != -1) {
			continue;
		}
		if (ht[i].weight <= min_weight1) {
			min_weight2 = min_weight1;
			min_pos2 = min_pos1;
			min_weight1 = ht[i].weight;
			min_pos1 = i;
		}else if (ht[i].weight < min_weight2) {
			min_weight2 = ht[i].weight;
			min_pos2 = i;
		}
	}
}

void create_huffman_tree(HuffmanTree &ht, const TokenList &tokens) {
	init_huffman_tree(ht, tokens);

	HuffmanTree::size_type nodecount = ht.size();
	for (TokenList::size_type i = tokens.size(); i<nodecount; ++i) {
		long min_pos1 = -1, min_pos2 = -1;
		find_min_weight_positions(ht, i, min_pos1, min_pos2);
		ht[min_pos1].parent = ht[min_pos2].parent = i;
		ht[i].lchild = min_pos1;
		ht[i].rchild = min_pos2;
		ht[i].weight = ht[min_pos1].weight+ht[min_pos2].weight;
	}
	return;
}

void create_huffman_code(const HuffmanTree &ht,long ht_index,unsigned char byte,HuffmanCode &hc) {
	long i = ht_index;
	hc.byte = byte;

	while (ht[i].parent != -1) {
		if (ht[ht[i].parent].lchild == i) {
			hc.code.push_back(0);
		}else{
			hc.code.push_back(1);
		}
		i = ht[i].parent;
	}
	reverse(hc.code.begin(), hc.code.end());
	return;
}

void create_huffman_codes(const HuffmanTree &ht, const TokenList &tokens, HuffmanCodes &hcs) {
	hcs.clear();
	for (int i = 0; i < 256; ++i) {
		HuffmanCode hc;hc.byte = i;
		hcs.push_back(hc);
	}

	TokenList::size_type wordcount = tokens.size();
	for (TokenList::size_type i = 0; i<wordcount; ++i) {
		HuffmanCode hc;
		unsigned char byte = tokens[i].byte;
		create_huffman_code(ht, i, byte, hc);
		hcs[byte].code = hc.code;
	}
}

void print_huffman_code(const HuffmanCode &hc, const TokenList &tk, long &tk_i) {
	if (hc.code.size() == 0) {
		return;
	}
	if (isgraph(hc.byte)) {
		cout<<"'"<<hc.byte<<"'";
	}else{
		cout<<"0x"<<hex<< static_cast<int>(hc.byte)<<dec;
	}
	cout<<"\tweight "<<tk[tk_i].weight<<"\tcode:";
	vector<int>::const_iterator iter, iter_end;
	for (iter = hc.code.begin(), iter_end = hc.code.end(); iter != iter_end; ++iter) {
		cout<<" "<<(*iter);
	}
	++tk_i;
	cout<<endl;
}

void print_huffman_codes(const HuffmanCodes &hcs,const TokenList &tk) {
	long tk_i = 0;
	HuffmanCodes::const_iterator iter, iter_end;
	for (iter = hcs.begin(), iter_end = hcs.end(); iter != iter_end; ++iter) {
		print_huffman_code((*iter), tk, tk_i);
	}
}
