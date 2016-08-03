//============================================================================
// Name : huffman.c
// Author : lancer
// Version : 1.0    2015-12-29 08:52:31
// Copyright : made by lancer
// Description : Ansi-style
// Last Modified time : 2016-08-03 19:52:12
//============================================================================

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <windows.h>
#include "huffman.h"
#include "compress.h"
#include "decompress.h"

int main(int argc, char const *argv[]){
    int key;
    char filename[24];
    do {
        printf("+------------+\n");
        printf("| 1.ѹ���ļ� |\n");
        printf("| 2.��ѹ�ļ� |\n");
        printf("+------------+\n");
        scanf("%d", &key);
        getchar();
        printf("�������ļ�����·����");
        scanf("%s",filename);
        switch (key) {
            case 1: compress(filename, NULL); break;
            case 2: decompress(filename, NULL); break;
            default: break;
        }
    } while (key != 0);
    system("pause");
    return 0;
}
