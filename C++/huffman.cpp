//============================================================================
// Name : huffman.cpp
// Author : lancer
// Version : 1.0    2015-12-30 08:52:31
// Copyright : made by lancer
// Description : Ansi-style
// Last Modified time : 2015-12-30 12:00:35
//============================================================================

#include <io.h>
#include <iostream>
#include <fstream>
#include "Bitstream.imp.h"

#include "huffman.h"
#include "compress.h"
#include "decompress.h"

#define FILE_HEADER "huffman made by lancer"

using namespace std;
using namespace Costella;

int main(int argc, char* argv[]) {
	int key;
	do {
		cout<<"+------------+"<<endl;
		cout<<"| 1.Ñ¹ËõÎÄ¼þ |"<<endl;
		cout<<"| 2.½âÑ¹ÎÄ¼þ |"<<endl;
		cout<<"+------------+"<<endl;
		cin>>key;
		getchar();
		string in_filename;
		cout<<"ÇëÊäÈëÎÄ¼þÍêÕûÂ·¾¶£º"<<endl;
		getline(cin, in_filename);
		switch (key) {
			case 1: compress(in_filename); break;
			case 2: decompress(in_filename); break;
			default: break;
		}
	} while (key != 0);

	system("pause");
	return 0;
}

