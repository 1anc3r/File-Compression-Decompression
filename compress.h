//============================================================================
// Name : compress.h
// Author : lancer
// Version : 1.0    2015-12-30 08:52:31
// Copyright : made by lancer
// Description : Ansi-style
// Last Modified time : 2015-12-30 08:52:31
//============================================================================

#define FILE_HEADER "huffman made by lancer"

using namespace std;
using namespace Costella;

bool is_empty_token(const HuffmanToken &tk) {
	return tk.weight == 0;
}

TokenList collect_word_list(istream &in) {
	TokenList tokens;
	TokenList result_tokens;
	unsigned char byte;
	for (int i = 0; i < 256; ++i) {
		HuffmanToken token = {i, 0};
		tokens.push_back(token);
	}
	while(in) {
		if (!in.read(reinterpret_cast<char*>(&byte), sizeof(byte))) {
			break;
		}
		++(tokens[byte].weight);
	}
	remove_copy_if (tokens.begin(), tokens.end(), back_inserter(result_tokens), is_empty_token);
	return result_tokens;
}

bool write_huffman_node(ostream &out, const HuffmanNode &node) {
	out.write(reinterpret_cast<const char*>(&(node.lchild)), sizeof(node.lchild));
	out.write(reinterpret_cast<const char*>(&(node.rchild)), sizeof(node.rchild));
	out.write(reinterpret_cast<const char*>(&(node.parent)), sizeof(node.parent));
	if (out) {
		return true;
	} else {
		return false;
	}
}

bool write_huffman_token(ostream &out, const HuffmanToken &tk) {
	out.write(reinterpret_cast<const char*>(&(tk.byte)), sizeof(tk.byte));
	out.write(reinterpret_cast<const char*>(&(tk.weight)), sizeof(tk.weight));
	if (out) {
		return true;
	} else {
		return false;
	}
}

bool write_huffman_tree(ostream &out, const HuffmanTree &ht, const TokenList &tokens)
{
	long n=tokens.size();
	out.write(reinterpret_cast<const char*>(&n), sizeof(n));
	if (!out) {
		return false;
	}

	HuffmanTree::const_iterator huffiter, huffiter_end;
	for (huffiter=ht.begin(), huffiter_end = ht.end(); huffiter != huffiter_end; ++huffiter) {
		if (write_huffman_node(out,(*huffiter)) == false) {
			return false;
		}
	}

	TokenList::const_iterator tokeniter, tokeniter_end;
	for (tokeniter = tokens.begin(), tokeniter_end = tokens.end(); tokeniter != tokeniter_end; ++tokeniter) {
		if (write_huffman_token(out, (*tokeniter)) == false) {
			return false;
		}
	}
	return true;
}

bool write_huffman_zip_header(ostream &out) {
	out<<FILE_HEADER<<"\n";
	if (out) {
		return true;
	} else {
		return false;
	}
}

void write_huffman_code(Bitstream::Out<long> &bout, const vector<int> &code) {
	vector<int>::const_iterator iter, iter_end;
	for (iter = code.begin(), iter_end = code.end(); iter != iter_end; ++iter) {
		bout.boolean((*iter) == 1);
	}
	return;
}

bool huffman_data_encode(istream &in, ostream &out, const HuffmanCodes &hcs)
{
	long write_start_pos = out.tellp();
	long bit_count = 0;
	out.write(reinterpret_cast<const char*>(&bit_count), sizeof(bit_count));
	if (!out) {
		return false;
	}

	Bitstream::Out<long> bout(out);
	bit_count = bout.position();
	while(in) {
		unsigned char byte = 0;
		if (!in.read(reinterpret_cast<char*>(&byte), sizeof(byte))) {
			break;
		}
		write_huffman_code(bout, hcs[byte].code);
		if (!out) {
			return false;
		}
	}

	bit_count = bout.position() - bit_count;
	bout.flush();
	out.seekp(write_start_pos, ios::beg);
	out.write(reinterpret_cast<const char*>(&bit_count), sizeof(bit_count));
	out.seekp(0,ios::end);
	if (out) {
		return true;
	} else {
		return false;
	}
}

bool huffman_zip(const char *in_filename, const char *out_filename)
{
	HuffmanTree ht;
	TokenList tokens;
	HuffmanCodes hcs;
	ifstream in;
	ofstream out;
	bool r=false;

	in.open(in_filename, ios_base :: in | ios_base :: binary);
	if (!in) {
		clog<<"无法打开输入文件："<<in_filename<<endl;
		return false;
	}
	tokens = collect_word_list(in);
	if (tokens.size() == 0) {
		clog<<"文件为空："<<in_filename<<endl;
		return false;
	}
	in.clear();
	in.seekg(0,ios::beg);
	if (!in) {
		clog<<"无法移动输入文件指针："<<in_filename<<endl;
		return false;
	}

	create_huffman_tree(ht, tokens);
	create_huffman_codes(ht, tokens, hcs);
	cout<<"是否打印编码表？1.是 / 0.否"<<endl;
	int key;
	cin>>key;
	if (key == 1) {
		cout<<"编码表："<<endl;
		print_huffman_codes(hcs, tokens);
	}

	out.open(out_filename, ios_base :: out | ios_base :: binary);
	if (!out) {
		clog<<"无法打开输出文件："<<out_filename<<endl;
		return false;
	}
	if (write_huffman_zip_header(out) == false) {
		clog<<"无法写输出文件："<<out_filename<<endl;
		return false;
	}
	if (write_huffman_tree(out,ht,tokens) == false) {
		clog<<"无法写输出文件："<<out_filename<<endl;
		return false;
	}
	if (huffman_data_encode(in,out,hcs) == false) {
		clog<<"无法写输出文件："<<out_filename<<endl;
		return false;
	}
	in.close();
	out.close();
	return true;
}

void compress (string in_filename) {
	double in_size, out_size;

	FILE *in = fopen(in_filename.c_str(), "rb");
	if (in) {
		in_size = filelength(fileno(in));
		fclose(in);
	}

	string zip_filename = in_filename + ".huf";
	cout<<"正在压缩……"<<endl;
	if (huffman_zip(in_filename.c_str(), zip_filename.c_str())) {
		cout<<"压缩完成，输出文件："<<zip_filename<<endl;
	} else {
		cout<<"压缩失败！"<<endl;
		system("pause");
	}

	FILE *out = fopen(zip_filename.c_str(), "rb");
	if (out) {
		out_size = filelength(fileno(out));
		fclose(out);
	}
	cout<<"压缩率:"<<(100.0*out_size/in_size)<<"%%"<<endl;
}
