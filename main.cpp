#include "compressor.h"
#include "decompressor.h"

#include <iostream>

int main(int argc, char *argv[]) {
    const char* file_path = "./file/text.txt";
    const char* zipfile_path = "./file/hs_zip.abc";
    const char* unzip_path = "./file/hs_unzip.txt";

    Compressor compressor(file_path,zipfile_path);
    Decompressor decompressor(zipfile_path,unzip_path);

    compressor.compress();
    std::cout << "compressed!" << std::endl;

    decompressor.decompress();
    std::cout << "decompressed!" << std::endl;
}