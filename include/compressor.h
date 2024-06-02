#ifndef __COMPRESSOR__
#define __COMPRESSOR__
#include "htnode.h"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <queue>
#include <vector>
#include <string.h>


#define MAX_SIZE 256
#define MAX_WEIGHT 1000000

class Compressor {
public:
    Compressor(const char *file_path, const char *zipfile_file);
    ~Compressor();
    void compress();

private:
    long ch_cnt = 0;
    long ch_type_cnt = 0;
    long compress_ch_cnt = 0;
    long node_cnt = 0;
    unsigned char chs[MAX_SIZE];
    long freq[MAX_SIZE];

    const char *file_path, *zipfile_path;

    HTNode* htree;

    void read_count();
    void create_htree();
    void create_hftree_code(long cur,std::vector<char>& path);
    void write_compress_res();
};
#endif