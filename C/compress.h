//============================================================================
// Name : compress.h
// Author : lancer
// Version : 1.0    2015-12-29 08:52:31
// Copyright : made by lancer
// Description : Ansi-style
// Last Modified time : 2015-12-30 09:14:45
//============================================================================

int initial_files(char *source_filename, FILE **pin, char *obj_filename, FILE **pout){
    char *temp;

    if (fopen(source_filename, "rb+")==NULL){
        return -1;
    }

    if (!obj_filename){
        if ((obj_filename = (char*)malloc(256*sizeof(char))) == NULL){
            return -1;
        }

        if (!(temp = strrchr(source_filename, '.'))){
            strcpy(obj_filename, source_filename);
            strcat(obj_filename, ".huf");
        } else {
            strncpy(obj_filename, source_filename, temp-source_filename);
            obj_filename[temp-source_filename]='\0';
            strcat(obj_filename, ".huf");
        }
    }

    if (strcmp(source_filename, obj_filename) == 0){
        return -1;
    }

    if ((*pout=fopen(obj_filename, "wb"))==NULL){
        return -1;
    }

    if ((*pin=fopen(source_filename, "rb"))==NULL){
        return -1;
    }

    free(obj_filename);
    return 0;
}

long frequency_data(FILE *in, long frequency[]){
    unsigned char buf[256];
    int     i, read_len;
    long    filesize;

    for (i = 0; i < 256; i++){
        frequency[i] = 0;
    }

    fseek(in, 0L, SEEK_SET);
    read_len = 256;

    while (read_len == 256){
        read_len = fread(buf, 1, 256, in);
        for (i = 0; i < read_len; i++){
            frequency[*(buf+i)]++;
        }
    }

    filesize = 0;
    for (i = 0; i < 256; i++){
        filesize += frequency[i];
    }

    return filesize;
}

unsigned char chars_to_bits(const unsigned char chars[8]){
    int i;
    unsigned char bits = 0;
    bits |= chars[0];
    for (i = 1; i < 8; ++i){
        bits <<= 1;
        bits |= chars[i];
    }
    return bits;
}

int write_compress_file(FILE *in, FILE *out, HuffmanTree ht, HuffmanCode hc[], char* source_filename, long source_filesize){
    HuffmanCode     *cur_HuffmanCode;
    unsigned int    i, read_counter, write_counter, zip_head=0xFFFFFFFF;
    unsigned char   write_char_counter, code_char_counter, copy_char_counter, read_buf[256], write_buf[256], write_chars[8], filename_size=strlen(source_filename);

    fseek(in, 0L, SEEK_SET);
    fseek(out, 0L, SEEK_SET);
    fwrite(&zip_head, sizeof(unsigned int), 1, out);
    fwrite(&filename_size, sizeof(unsigned char), 1, out);
    fwrite(source_filename, sizeof(char), filename_size, out);
    fwrite(&source_filesize, sizeof(long), 1, out);

    for (i = 256; i < 256*2-1; i++){
        fwrite(&(ht[i].lchild), sizeof(ht[i].lchild), 1, out);
        fwrite(&(ht[i].rchild), sizeof(ht[i].rchild), 1, out);
    }

    write_counter = write_char_counter = 0;
    read_counter = 256;

    while (read_counter == 256){
        read_counter = fread(read_buf, 1, 256, in);
        for (i = 0; i < read_counter; i++){
            cur_HuffmanCode = &hc[read_buf[i]];
            code_char_counter = 0;
            while (code_char_counter != cur_HuffmanCode->codelen){
                copy_char_counter = (8-write_char_counter > cur_HuffmanCode->codelen-code_char_counter ?
                    cur_HuffmanCode->codelen-code_char_counter : 8-write_char_counter);
                memcpy(write_chars+write_char_counter, cur_HuffmanCode->codestr+code_char_counter, copy_char_counter);
                write_char_counter += copy_char_counter;
                code_char_counter += copy_char_counter;
                if (write_char_counter == 8){
                    write_char_counter = 0;
                    write_buf[write_counter++] = chars_to_bits(write_chars);
                    if (write_counter == 256){
                        fwrite(write_buf, 1, 256, out);
                        write_counter = 0;
                    }
                }
            }
        }

    }

    fwrite(write_buf, 1, write_counter, out);
    if (write_char_counter != 0){
        write_char_counter = chars_to_bits(write_chars);
        fwrite(&write_char_counter, 1, 1, out);
    }
    return 0;
}

int compress(char *source_filename, char *obj_filename){
    FILE        *in, *out;
    HuffmanCode hc[256];
    HuffmanNode ht[256*2-1];
    int     error_code, i, j, key;
    long    frequency[256], source_filesize, obj_filesize=0;
    float   compress_rate;
    char    error_info[256] = "";
    char source_filename_[24];

    strcpy(source_filename_, source_filename);

    FILE *fp = fopen(strcat(source_filename_, ".log"), "w");

    printf("正在压缩……\n");
    fprintf(fp, "正在压缩……\n");

    error_code = initial_files(source_filename, &in, obj_filename, &out);
    if  (error_code){
        printf("无法打开输入文件：%s\n", source_filename);
        fprintf(fp, "无法打开输入文件：%s\n", source_filename);
        return error_code;
    }

    source_filesize = frequency_data(in, frequency);

    error_code = create_hftree(ht, frequency, 256);
    if (error_code){
        printf("无法创建哈夫曼树！\n");
        fprintf(fp, "%s", "无法创建哈夫曼树！\n");
        return error_code;
    }

    error_code=encode_hftree(ht, hc, 256);
    if (error_code){
        printf("%s\n", "无法创建哈夫曼编码！\n");
        fprintf(fp, "无法创建哈夫曼编码\n");
        return error_code;
    }

    for (i = 0; i < 256; i++){
        obj_filesize += frequency[i] * hc[i].codelen;
    }
    obj_filesize = (obj_filesize%8 == 0 ? obj_filesize/8 : obj_filesize/8+1);
    for (i = 0; i < 256-1; i++){
        obj_filesize += 2*sizeof(short);
    }
    obj_filesize += strlen(source_filename)+1+sizeof(long)+sizeof(unsigned int);

    compress_rate=(float)obj_filesize/source_filesize;
    printf("压缩率 %.2lf%%\n", compress_rate*100);
    fprintf(fp, "压缩率 %lf%%\n", compress_rate*100);

    error_code = write_compress_file(in, out, ht, hc, source_filename, source_filesize);
    if (error_code){
        printf("无法写输出文件！\n");
        fprintf(fp, "%s", "无法写输出文件！\n");
        return error_code;
    }

    fclose(in);
    fclose(out);
    for (i = 0; i < 256; i++){
        free(hc[i].codestr);
    }
    // printf("是否打印编码表？1.是 / 0.否\n");
    // scanf("%d", &key);
    key = 1;
    if (key == 1) {
        printf("编码表：\n");
        fprintf(fp, "%s", "编码表：\n");
        for(i = 0; i < 256; i++){
            if(frequency[i] == 0){
                i++;
            } else {
                printf("%c\t%d\t",i,frequency[i]);
                fprintf(fp, "%c\t%d\t", i, frequency[i]);
                for(j = 0; j < hc[i].codelen; j++){
                    printf(" %d",hc[i].codestr[j]);
                    fprintf(fp, " %d", hc[i].codestr[j]);
                }
                printf("\n");
                fprintf(fp, "\n");
            }
        }
    }
    printf("压缩完成！\n");
    fprintf(fp, "%s", "压缩完成！");
    fclose(fp);
    return error_code;
}
