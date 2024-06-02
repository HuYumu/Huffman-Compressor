#include "htnode.h"
#include "compressor.h"

Compressor::Compressor(const char* file_path, const char* zipfile_path) {
    this->file_path = file_path;
    this->zipfile_path = zipfile_path;
}


void Compressor::compress() {
    read_count();
    create_htree();
    std::vector<char> path;
    create_hftree_code(this->node_cnt-1,path);
    for(int k = 0; k < this->node_cnt; ++k) {
        if(this->htree[k].ch == '\n' || this->htree[k].ch == '\r' || this->htree[k].ch == '\r\n') {
            std::cout << k << " " << '_' << " " << this->htree[k].parent << " " << this->htree[k].lchild << " " << this->htree[k].rchild << std::endl;
        } else {
            std::cout << k << " " << this->htree[k].ch << " " << this->htree[k].parent << " " << this->htree[k].lchild << " " << this->htree[k].rchild << " ";
        }
        if(k < this->ch_type_cnt) {
            std::cout << this->htree[k].freq << " ";
            for(int i = 0; i < this->htree[k].code.size(); ++i) {
                std::cout << this->htree[k].code[i];
            }
        }
        std::cout << std::endl;
    }
    write_compress_res();
}


void Compressor::read_count() {
    long ch_freq_dict[MAX_SIZE] = {0};

    FILE* file = fopen(this->file_path,"rb");

    unsigned char ch = fgetc(file);
    while(!feof(file)) {
        ++ch_freq_dict[ch];
        ++this->ch_cnt;
        ch = fgetc(file);
    }
    fclose(file);
    
    for(int i = 0; i < MAX_SIZE; ++i) {
        if(ch_freq_dict[i] != 0) {
            this->chs[ch_type_cnt] = i;
            this->freq[ch_type_cnt] = ch_freq_dict[i];
            ++this->ch_type_cnt;
        }
    }
    this->node_cnt = this->ch_type_cnt*2 - 1;
}


void Compressor::create_htree() {
    this->htree = (HTNode*)calloc(this->node_cnt,sizeof(HTNode));

    for(int i = 0; i < this->node_cnt; ++i) {
        this->htree[i].lchild = -1;
        this->htree[i].rchild = -1;
        this->htree[i].parent = -1;
    }

    std::priority_queue<HTNode*,std::vector<HTNode*>,HTNodeCmp> pq;

    for(int i = 0; i < this->ch_type_cnt; ++i) {
        this->htree[i].ch = this->chs[i];
        this->htree[i].freq = this->freq[i];
        pq.push(this->htree+i);
    }

    int lid = 0, rid = 0;
    for(int i = this->ch_type_cnt; i < this->node_cnt; ++i) {
        lid = pq.top() - this->htree;
        pq.pop(); 
        rid = pq.top() - this->htree;
        pq.pop();
        this->htree[i].freq = this->htree[lid].freq + this->htree[rid].freq;
        this->htree[i].lchild = lid;
        this->htree[i].rchild = rid;
        this->htree[lid].parent = i;
        this->htree[rid].parent = i;
        pq.push(this->htree+i);
    }
}


void Compressor::create_hftree_code(long cur,std::vector<char>& path) {
    if(this->htree[cur].lchild == -1 && this->htree[cur].rchild == -1) {
        this->htree[cur].code = std::vector<char>(path);
        return;
    }
    path.push_back('0');
    create_hftree_code(this->htree[cur].lchild,path);
    path.pop_back();
    path.push_back('1');
    create_hftree_code(this->htree[cur].rchild,path);
    path.pop_back();
}


void Compressor::write_compress_res() {
    FILE* file = fopen(this->file_path,"rb");
    FILE* zipfile = fopen(this->zipfile_path,"wb");

    fwrite(&this->ch_cnt,sizeof(long),1,zipfile);
    fwrite(&this->node_cnt,sizeof(long),1,zipfile);
    for(int k = 0; k < this->node_cnt; ++k) {
        fwrite(&this->htree[k].ch,sizeof(unsigned char),1,zipfile);
        fwrite(&this->htree[k].lchild,sizeof(int),1,zipfile);
        fwrite(&this->htree[k].rchild,sizeof(int),1,zipfile);
    }

    long ch_index[MAX_SIZE];
    for(int i = 0; i < this->ch_type_cnt; ++i) {
        ch_index[this->htree[i].ch] = i;
    }

    unsigned char tmp = 0; 
    int bit_cnt = 0;

    unsigned char ch = fgetc(file);
    while(!feof(file)) {
        std::vector<char> code = this->htree[ch_index[ch]].code;
        for(int i = 0; i < code.size(); ++i) {
            ++bit_cnt;
            tmp = code[i] == '0'? (tmp << 1) : (tmp << 1) | 1;
            if(bit_cnt >= 8) {
                fwrite(&tmp,sizeof(unsigned char),1,zipfile);
                ++this->compress_ch_cnt;
                bit_cnt = 0;
                tmp = 0;
            }
        }
        ch = fgetc(file);
    }
    
    if(bit_cnt > 0) {
        for(int i = 0; i < 8-bit_cnt; ++i) {
            tmp << 1;
        }
        fwrite(&tmp,sizeof(unsigned char),1,zipfile);
        ++this->compress_ch_cnt;
    }

    fclose(file);
    fclose(zipfile);

    std::cout << this->compress_ch_cnt << std::endl;
}


Compressor::~Compressor() {
    free(this->htree);
}











