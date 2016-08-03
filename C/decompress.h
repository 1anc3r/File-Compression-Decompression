//============================================================================
// Name : decompress.h
// Author : lancer
// Version : 1.0    2015-12-29 08:52:31
// Copyright : made by lancer
// Description : Ansi-style
// Last Modified time : 2015-12-30 09:14:45
//============================================================================

int de_initial_files(char *source_filename, FILE **pin, char *obj_filename, FILE **pout){
    unsigned int    zip_head;
    unsigned char   filename_size;

    if ((*pin = fopen(source_filename, "rb")) == NULL){
        return -1;
    }

    fread(&zip_head, sizeof(unsigned int), 1, *pin);
    if (zip_head!=0xFFFFFFFF){
        return -1;
    }
    if (!obj_filename){
        if ((obj_filename=(char*)malloc(256*sizeof(char)))==NULL){
            return -1;
        }
        fread(&filename_size, sizeof(unsigned char), 1, *pin);
        fread(obj_filename, sizeof(char), filename_size, *pin);
        obj_filename[filename_size]='\0';
    }
    else{
        fread(&filename_size, sizeof(unsigned char), 1, *pin);
        fseek(*pin, filename_size, SEEK_CUR);
    }

    if ((*pout=fopen(obj_filename, "wb"))==NULL){
        return -1;
    }

    free(obj_filename);
    return 0;
}

void get_mini_huffmantree(FILE* in, short mini_ht[][2]){
    int i;
    for (i = 0; i < 256; i++){
        mini_ht[i][0] = mini_ht[i][1] = -1;
    }
    fread(mini_ht[i], sizeof(short), 2*(256-1), in);
}

int write_decompress_file(FILE *in, FILE* out, short mini_ht[][2], long bits_pos, long obj_filesize){
    long            cur_size;
    unsigned char   read_buf[256], write_buf[256], convert_bit;
    unsigned int    read_counter, write_counter, cur_pos;


    fseek(in, bits_pos, SEEK_SET);
    fseek(out, 0L, SEEK_SET);
    read_counter = 256-1;
    cur_size = write_counter = 0;
    cur_pos = 256*2-2;

    while (cur_size != obj_filesize){
        if (++read_counter == 256){
            fread(read_buf, 1, 256, in);
            read_counter = 0;
        }
        for (convert_bit = 128; convert_bit !=0 ; convert_bit >>= 1){
            cur_pos = ((read_buf[read_counter]&convert_bit) == 0 ? mini_ht[cur_pos][0] : mini_ht[cur_pos][1]);
            if (cur_pos<256){
                write_buf[write_counter] = (unsigned char)cur_pos;
                if (++write_counter == 256){
                    fwrite(write_buf, 1, 256, out);
                    write_counter = 0;
                }
                cur_pos = 256*2-2;
                if (++cur_size == obj_filesize){
                    break;
                }
            }
        }

    }
    fwrite(write_buf, 1, write_counter, out);
    return 0;
}

int decompress(char *source_filename, char *obj_filename){
    int     error_code;
    FILE    *in, *out;
    short   mini_ht[256*2-1][2];
    long    obj_filesize;

    error_code = de_initial_files(source_filename, &in, obj_filename, &out);
    if (error_code){
        printf("无法打开输入文件：%s\n", source_filename);
        return error_code;
    }

    fread(&obj_filesize, sizeof(long), 1, in);

    get_mini_huffmantree(in, mini_ht);
    error_code = write_decompress_file(in, out, mini_ht, ftell(in), obj_filesize);
    if (error_code){
        printf("解压失败！\n");
        return error_code;
    }

    printf("解压完成！\n");
    fclose(in);
    fclose(out);
    return 0;
}
