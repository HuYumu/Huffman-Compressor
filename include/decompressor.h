#ifndef __DECOMPRESSOR__
#define __DECOMPRESSOR__

#include "htnode.h"

#include <fstream>
#include <bitset>


class Decompressor {
public:
    Decompressor(const char *zipfile_path, const char *unzipfile_file);
    ~Decompressor();
    void decompress();

private:
    long ch_cnt = 0;
    long node_cnt = 0;
    
    const char *zipfile_path, *unzipfile_path;
    HTNode* htree;

    void write_decompress_res();
};

#endif