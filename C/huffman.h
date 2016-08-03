typedef struct HuffmanNode{
	long    weight;
	short   parent, lchild, rchild;
}HuffmanNode, *HuffmanTree;

typedef struct HuffmanCode{
	unsigned char codelen;
	unsigned char *codestr;
}HuffmanCode;

// int create_hftree(HuffmanNode ht[], long weight[], int n);
// int encode_hftree(HuffmanNode ht[], HuffmanCode hc[], int n);

// int create_hftree(HuffmanNode ht[], long weight[], int n){
// 	int i, j;
// 	int minW1, minW2, minX1, minX2;
// 	if (n < 1){
// 		return -1;
// 	}

// 	if (ht == NULL){
// 		return -1;
// 	}
// 	for (i = 0; i < 2*n-1; i++){
// 		if (i  <  n){
// 			ht[i].weight = weight[i];
// 		} else {
// 			ht[i].weight = -1;
// 		}
// 		ht[i].parent = ht[i].lchild = ht[i].rchild = -1;
// 	}
// 	for (i = 0; i < n-1; i++){
// 		minW1 = minW2 = INT_MAX;
// 		minX1 = minX2 = 0;
// 		for (j = i; j < n+i; j++){
// 			if (ht[j].weight < minW1 && ht[j].parent == -1){
// 				minW2 = minW1;
// 				minX2 = minX1;
// 				minW1 = ht[j].weight;
// 				minX1 = j;
// 			} else if (ht[j].weight < minW2 && ht[j].parent == -1){
// 				minW2 = ht[j].weight;
// 				minX2 = j;
// 			}
// 		}
// 		ht[minX1].parent = n+i;
// 		ht[minX2].parent = n+i;
// 		ht[n+i].weight = ht[minX1].weight+ht[minX2].weight;
// 		ht[n+i].lchild = minX1;
// 		ht[n+i].rchild = minX2;
// 	}
// 	return 0;
// }

// int encode_hftree(HuffmanNode ht[], HuffmanCode hc[], int n){
// 	int i, j;
// 	int child, parent, len;
// 	unsigned char *code = (unsigned char*)malloc(n*sizeof(unsigned char));
// 	if (code == NULL){
// 		return -1;
// 	}
// 	for (i = 0; i < n; i++){
// 		len = 0;
// 		child = i;
// 		parent = ht[child].parent;
// 		while (parent != 2*n-2){
// 			code[len] = (ht[ht[parent].parent].lchild == parent ? 0:1);
// 			len++;
// 			child = parent;
// 			parent = ht[child].parent;
// 		}
// 		if ( (hc[i].codestr = (unsigned char *)malloc((len)*sizeof(unsigned char))) == NULL){
// 			return -1;
// 		}
// 		hc[i].codelen = len;
// 		for (j = 0; j < len; j++){
// 			hc[i].codestr[j] = code[len-j-1];
// 		}
// 	}
// 	free(code);
// 	return 0;
// }

int search_set(HuffmanTree ht,int n,int *s1, int *s2){
    int i,x;
    long minValue = 1000000,min = 0;
    for(x=0;x<n;x++){
        if(ht[x].parent==-1)  break;

    }
    for(i=0;i<n;i++){
        if(ht[i].parent==-1 && ht[i].weight < minValue){
            minValue = ht[i].weight;
            min=i;
        }
    }
    *s1 = min;

    minValue = 1000000,min = 0;
    for(i=0;i<n;i++){
        if(ht[i].parent==-1 && ht[i].weight < minValue && i != *s1){
            minValue = ht[i].weight;
            min=i;
        }
    }

    *s2 = min;
    return 1;
}

int create_hftree(HuffmanNode ht[],long w[],int n){
    int m,i,s1,s2;

    if (n<1)    return -1;
    m=2*n-1;
    if (ht==NULL)   return -1;
    for(i=0;i<n;i++){
        ht[i].weight=w[i];
        ht[i].parent=ht[i].lchild=ht[i].rchild=-1;
    }
    for(;i<m;i++){
        ht[i].weight=ht[i].parent=ht[i].lchild=ht[i].rchild=-1;
    }

    for(i=n;i<m;i++){
        search_set(ht,i,&s1,&s2);

        ht[s1].parent = ht[s2].parent = i;
        ht[i].lchild  = s1;
        ht[i].rchild = s2;
        ht[i].weight  = ht[s1].weight + ht[s2].weight;
    }
    return 0;
}

int encode_hftree(HuffmanTree htp,HuffmanCode hc[],int n){
    int i,j,p,codelen;
    unsigned char *code=(unsigned char*)malloc(n*sizeof(unsigned char));

    if (code==NULL) return -1;
    for(i=0;i<n;i++){
        for(p=i,codelen=0;p!=2*n-2;p=htp[p].parent,codelen++){
            code[codelen]=(htp[htp[p].parent].lchild==p?0:1);
        }
        if((hc[i].codestr=(unsigned char *)malloc((codelen)*sizeof(unsigned char)))==NULL){
            return -1;
        }
        hc[i].codelen=codelen;
        for(j=0;j<codelen;j++){
            hc[i].codestr[j]=code[codelen-j-1];
        }
    }
    free(code);
    return 0;
}
