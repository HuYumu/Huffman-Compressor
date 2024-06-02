#include<iostream>
#include<fstream>
#include<algorithm>
#include<queue> 
#include<vector>
#include<string.h>
#include<bitset>

using namespace std;

#define MAX_SIZE 256
#define MAX_WEIGHT 1000000


struct HTNode {
    unsigned char ch;
    long freq;
    vector<char> code;
    int parent;
    int lchild;
    int rchild;
//    bool operator< (const HTNode& ht) {
//	    return this->freq < ht->freq;
//	}
};

struct HTNodeCmp {
	bool operator()(const HTNode* a, const HTNode* b) const {
        return a->freq > b->freq;
    }
}; 


void read_count(unsigned char chs[], long freq[], long& ch_cnt, long& ch_type_cnt) {
    long ch_freq_dict[MAX_SIZE] = {0};

    FILE* input = fopen("./bin/file_path/text.txt","rb");
	if(!input) {
		std::cerr << "Error opening file_path." << std::endl;
		return;
	}

    unsigned char ch = fgetc(input);
    while(!feof(input)) {
        ++ch_freq_dict[ch];
        ++ch_cnt;
        ch = fgetc(input);
    }
    fclose(input);
    
    for(int i = 0; i < MAX_SIZE; ++i) {
        if(ch_freq_dict[i] != 0) {
            chs[ch_type_cnt] = i;
            freq[ch_type_cnt] = ch_freq_dict[i];
            ++ch_type_cnt;
        }
    }
}


HTNode* create_htree(long node_cnt, long ch_type_cnt, unsigned char chs[], long freq[]) {
    HTNode* htree = (HTNode*)calloc(node_cnt,sizeof(HTNode));

    for(int i = 0; i < node_cnt; ++i) {
        htree[i].lchild = -1;
        htree[i].rchild = -1;
        htree[i].parent = -1;
    }

    priority_queue<HTNode*,vector<HTNode*>,HTNodeCmp> pq;

    for(int i = 0; i < ch_type_cnt; ++i) {
        htree[i].ch = chs[i];
        htree[i].freq = freq[i];
        pq.push(htree+i);
    }

    int lid = 0, rid = 0;
    for(int i = ch_type_cnt; i < node_cnt; ++i) {
        lid = pq.top() - htree;
        pq.pop(); 
        rid = pq.top() - htree;
        pq.pop();
        htree[i].freq = htree[lid].freq + htree[rid].freq;
        htree[i].lchild = lid;
        htree[i].rchild = rid;
        htree[lid].parent = i;
        htree[rid].parent = i;
        pq.push(htree+i);
    }
    return htree;
}


void create_hftree_code(long cur, HTNode* htree, vector<char>& path) {
    if(htree[cur].lchild == -1 && htree[cur].rchild == -1) {
        htree[cur].code = vector<char>(path);
        return;
    }
    path.push_back('0');
    create_hftree_code(htree[cur].lchild,htree,path);
    path.pop_back();
    path.push_back('1');
    create_hftree_code(htree[cur].rchild,htree,path);
    path.pop_back();
    return;
}


void compress(long ch_cnt, long node_cnt, long& compress_ch_cnt, HTNode *htree) {
    long ch_index[MAX_SIZE];
    for(int i = 0; i < node_cnt; ++i) {
        ch_index[htree[i].ch] = i;
    }

    FILE* infile = fopen("./bin/file_path/text.txt","rb");
    FILE* outfile = fopen("./bin/file_path/huff_zip.abc","wb");

	if(!infile) {
		std::cerr << "Error opening infile." << std::endl;
		return;
	}
	
	if(!outfile) {
		std::cerr << "Error opening outfile." << std::endl;
		return;
	}

    fwrite(&node_cnt,sizeof(long),1,outfile);
    fwrite(&ch_cnt,sizeof(long),1,outfile);

    for(int k = 0; k < node_cnt; ++k) {
        fwrite(&htree[k].ch,sizeof(unsigned char),1,outfile);
        fwrite(&htree[k].lchild,sizeof(int),1,outfile);
        fwrite(&htree[k].rchild,sizeof(int),1,outfile);
    }

    unsigned char tmp = 0; 
    int bit_cnt = 0; 

    char ch = fgetc(infile);
    while(!feof(infile)) {
        vector<char> code = htree[ch_index[ch]].code;
        for(int i = 0; i < code.size(); ++i) {
            ++bit_cnt;
            tmp = code[i] == '0'? (tmp << 1) : (tmp << 1) | 1;
            if(bit_cnt >= 8) {
                fwrite(&tmp,sizeof(unsigned char),1,outfile);
                ++compress_ch_cnt;
                bit_cnt = 0;
                tmp = 0;
            }
        }
        ch = fgetc(infile);
    }
    
    if(bit_cnt > 0) {
        for(int i = 0; i < 8-bit_cnt; ++i) {
            tmp << 1;
        }
        fwrite(&tmp,sizeof(unsigned char),1,outfile);
        ++compress_ch_cnt;
    }
    fclose(infile);
    fclose(outfile);
    return;
}


void decompress() {
    FILE* infile = fopen("./bin/file_path/huff_zip.abc","rb");
    FILE* outfile = fopen("./bin/file_path/huff_unzip.txt","wb");
    
    long written_ch_cnt = 0;
    long node_cnt = 0;
    long ch_cnt = 0;

    fread(&node_cnt,sizeof(long),1,infile);
    fread(&ch_cnt,sizeof(long),1,infile);

    HTNode* htree = (HTNode*)calloc(node_cnt,sizeof(HTNode));

    for(int k = 0; k < node_cnt; ++k) {
        fread(&htree[k].ch,sizeof(unsigned char),1,infile);
        fread(&htree[k].lchild,sizeof(int),1,infile);
        fread(&htree[k].rchild,sizeof(int),1,infile);
    }
    
    unsigned char ch = fgetc(infile), ans;
    long cur_node_id = node_cnt-1;

    cout << node_cnt << " " << ch_cnt << endl;

    while(!feof(infile)) {
        bitset<8> tmp(ch);
        for(int i = 7; i >= 0; --i) {
            cur_node_id = tmp[i] == 0? htree[cur_node_id].lchild : htree[cur_node_id].rchild;
            if(htree[cur_node_id].lchild == -1 && htree[cur_node_id].rchild == -1) {
                ans = htree[cur_node_id].ch;
                fwrite(&ans,sizeof(unsigned char),1,outfile);
                ++written_ch_cnt;
                cur_node_id = node_cnt-1;
                if(written_ch_cnt == ch_cnt) {
                    break;
                }
            }
        }
        if(written_ch_cnt == ch_cnt) {
            break;
        }
        ch = fgetc(infile);
    }
    fclose(infile);
    fclose(outfile);
}


int main() {
    long ch_cnt = 0;
    long ch_type_cnt = 0;
    long compress_ch_cnt = 0;
    unsigned char chs[MAX_SIZE];
    long freq[MAX_SIZE];
    
    read_count(chs, freq, ch_cnt, ch_type_cnt);
    long node_cnt = ch_type_cnt*2 - 1;
	
    HTNode* htree = create_htree(node_cnt,ch_type_cnt,chs,freq);
    vector<char> path;
    create_hftree_code(node_cnt-1,htree,path);
    
    compress(ch_cnt,node_cnt,compress_ch_cnt,htree);
    cout << "compressed word count:" << compress_ch_cnt << endl;
    cout << "compressed:" << 1 - (float)compress_ch_cnt/ch_cnt << endl;
    cout << "=======================" << endl;
    
    decompress();
    cout << "finished!" << endl;
    
    free(htree);
}
