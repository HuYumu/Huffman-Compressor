#include "decompressor.h"


Decompressor::Decompressor(const char* zipfile_path, const char* unzipfile_file) {
    this->zipfile_path = zipfile_path;
    this->unzipfile_path = unzipfile_file;
}

void Decompressor::decompress() {
    write_decompress_res();
}


void Decompressor::write_decompress_res() {
    FILE* zipfile = fopen(this->zipfile_path,"rb");
    FILE* unzipfile = fopen(this->unzipfile_path,"wb");

    fread(&this->ch_cnt,sizeof(long),1,zipfile);
    fread(&this->node_cnt,sizeof(long),1,zipfile);

    this->htree = (HTNode*)calloc(sizeof(HTNode),this->node_cnt);

    for(int k = 0; k < this->node_cnt; ++k) {
        fread(&this->htree[k].ch,sizeof(unsigned char),1,zipfile);
        fread(&this->htree[k].lchild,sizeof(int),1,zipfile);
        fread(&this->htree[k].rchild,sizeof(int),1,zipfile);
    }

    long written_ch_cnt = 0;

    long cur_node_id = this->node_cnt-1;
    
    unsigned char ch = fgetc(zipfile), ans;
    
    while(!feof(zipfile)) {
        std::bitset<8> tmp(ch);
        for(int i = 7; i >= 0; --i) {
            cur_node_id = tmp[i] == 0? this->htree[cur_node_id].lchild : this->htree[cur_node_id].rchild;
            if(this->htree[cur_node_id].lchild == -1 && this->htree[cur_node_id].rchild == -1) {
                ans = this->htree[cur_node_id].ch;
                fwrite(&ans,sizeof(unsigned char),1,unzipfile);
                ++written_ch_cnt;
                cur_node_id = this->node_cnt-1;
                if(written_ch_cnt == this->ch_cnt) {
                    break;
                }
            }
        }
        if(written_ch_cnt == this->ch_cnt) {
            break;
        }
        ch = fgetc(zipfile);
    }

    fclose(zipfile);
    fclose(unzipfile);
}

Decompressor::~Decompressor() {
    free(this->htree);
}
