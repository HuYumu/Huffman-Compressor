#include "decompressor.h"


Decompressor::Decompressor(const char* zipfile_path, const char* unzipfile_file) {
    this->zipfile_path = zipfile_path;
    this->unzipfile_path = unzipfile_file;
}

void Decompressor::decompress() {
    write_decompress_res();
}


void Decompressor::write_decompress_res() {
    std::ifstream zipfile;
    zipfile.open(this->zipfile_path, std::ios::in | std::ios::binary);

    std::ofstream unzipfile;
    unzipfile.open(this->unzipfile_path, std::ios::out | std::ios::binary);

    zipfile.read(reinterpret_cast<char*>(&this->ch_cnt),sizeof(this->ch_cnt));
    zipfile.read(reinterpret_cast<char*>(&this->node_cnt),sizeof(this->node_cnt));

    this->htree = (HTNode*)calloc(sizeof(HTNode),this->node_cnt);

    for(int k = 0; k < this->node_cnt; ++k) {
        zipfile.read(&this->htree[k].ch,sizeof(this->htree[k].ch));
        zipfile.read(reinterpret_cast<char*>(&this->htree[k].lchild),sizeof(this->htree[k].lchild));
        zipfile.read(reinterpret_cast<char*>(&this->htree[k].rchild),sizeof(this->htree[k].rchild));
    }

    long written_ch_cnt = 0;

    long cur_node_id = this->node_cnt-1;
    
    char ch, ans;
    while(!zipfile.eof()) {
        zipfile.read(&ch,sizeof(ch));
        std::bitset<8> tmp(ch);
        for(int i = 7; i >= 0; --i) {
            cur_node_id = tmp[i] == 0? this->htree[cur_node_id].lchild : this->htree[cur_node_id].rchild;
            if(this->htree[cur_node_id].lchild == -1 && this->htree[cur_node_id].rchild == -1) {
                ans = this->htree[cur_node_id].ch;
                unzipfile.write(&ans,sizeof(ans));
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
    }

    zipfile.close();
    unzipfile.close();
}

Decompressor::~Decompressor() {
    free(this->htree);
}
