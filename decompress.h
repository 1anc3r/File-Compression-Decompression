//============================================================================
// Name : decompress.h
// Author : lancer
// Version : 1.0    2015-12-30 08:52:31
// Copyright : made by lancer
// Description : Ansi-style
// Last Modified time : 2015-12-30 08:52:31
//============================================================================

using namespace std;
using namespace Costella;

string read_huffman_zip_header(istream &in) {
	string header;
	getline(in, header);
	return header;
}

bool read_huffman_node(istream &in, HuffmanNode &node) {
	in.read(reinterpret_cast<char*>(&(node.lchild)), sizeof(node.lchild));
	in.read(reinterpret_cast<char*>(&(node.rchild)), sizeof(node.rchild));
	in.read(reinterpret_cast<char*>(&(node.parent)), sizeof(node.parent));
	if (in) {
		return true;
	}else{
		return false;
	}
}

bool read_huffman_token(istream &in, HuffmanToken &tk) {
	in.read(reinterpret_cast<char*>(&(tk.byte)), sizeof(tk.byte));
	in.read(reinterpret_cast<char*>(&(tk.weight)), sizeof(tk.weight));
	if (in) {
		return true;
	}else{
		return false;
	}
}

bool read_huffman_tree(istream &in, HuffmanTree &ht, TokenList &tokens) {
	long n=0, i = 0, ht_n=0;;
	in.read(reinterpret_cast<char*>(&n), sizeof(n));
	if (!in) {
		return false;
	}

	ht.clear();
	ht_n = 2*n-1;
	for (i = 0; i < ht_n; ++i) {
		HuffmanNode node;
		if (read_huffman_node(in, node) == false) {
			return false;
		}
		ht.push_back(node);
	}

	tokens.clear();
	for (i = 0; i < n; ++i) {
		HuffmanToken tk;
		if (read_huffman_token(in, tk) == false) {
			return false;
		}
		tokens.push_back(tk);
	}
	return true;
}

bool huffman_data_decode(istream &in, ostream &out, const HuffmanTree &ht, const TokenList &tokens) {
	long bit_count = 0;
	long read_count = 0;

	in.read(reinterpret_cast<char*>(&bit_count), sizeof(bit_count));
	if (!in) {
		return false;
	}
	if (tokens.size() == 1) {
		for (long i = 0; i < tokens[0].weight; ++i) {
			out.write(reinterpret_cast<const char*>(&(tokens[0].byte)), sizeof(tokens[0].byte));
			if (!out) {
				return false;
			}
		}
		return true;
	}

	Bitstream::In<long> bin(in);
	long rootpos = ht.size() - 1;
	long huffpos = rootpos;
	while(in && read_count < bit_count) {
		bool bit = false;
		bin.boolean(bit);
		if (!in) {
			break;
		}
		if (bit) {
			if (ht[huffpos].rchild == -1) {
				return false;
			}else{
				huffpos=ht[huffpos].rchild;
			}
		}else{
			if (ht[huffpos].lchild == -1) {
				return false;
			}else{
				huffpos=ht[huffpos].lchild;
			}
		}
		if (ht[huffpos].lchild == -1 && ht[huffpos].rchild == -1) {
			out.write(reinterpret_cast<const char*>(&(tokens[huffpos].byte)), sizeof(tokens[huffpos].byte));
			if (!out) {
				return false;
			}
			huffpos = rootpos;
		}
		++read_count;
	}
	return true;
}

bool huffman_unzip(const char *in_filename, const char *out_filename) {
	HuffmanTree ht;
	TokenList tokens;
	ifstream in;
	ofstream out;
	string header;
	bool r = false;

	in.open(in_filename, ios_base :: in | ios_base :: binary);
	if (!in) {
		clog<<"无法打开输入文件："<<in_filename<<endl;
		return false;
	}
	header = read_huffman_zip_header(in);
	if (header != FILE_HEADER) {
		clog<<"无法读取输入文件，或着它不是hzip格式的压缩文件："<<in_filename<<endl;
		return false;
	}
	if (read_huffman_tree(in, ht, tokens) == false) {
		clog<<"无法从输入文件中读取元信息："<<in_filename<<endl;
	}

	out.open(out_filename, ios_base::out|ios_base::binary);
	if (!out) {
		clog<<"无法打开输出文件："<<out_filename<<endl;
		return false;
	}
	r=huffman_data_decode(in, out, ht, tokens);
	if (r == false) {
		clog<<"输入文件已损坏或写输出文件失败："<<endl<<"\t"<<in_filename<<endl<<"\t"<<out_filename<<endl;
		return false;
	}
	in.close();
	out.close();
	return r;
}

void decompress (string in_filename) {
	string zip_filename = in_filename + "";
	string out_filename = in_filename.substr(0, in_filename.find(".huf"));
	cout<<"正在解压……"<<endl;
	if (huffman_unzip(zip_filename.c_str(), out_filename.c_str())) {
		cout<<"解压完成，输出文件："<<out_filename<<endl;
	} else {
		cout<<"解压失败！"<<endl;
	}
}
