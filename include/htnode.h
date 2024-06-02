#ifndef __HTNODE__
#define __HTNODE__

#include<vector>

class HTNode {
public:
    unsigned char ch;
    long freq;
    std::vector<char> code;
    int parent;
    int lchild;
    int rchild;

    bool operator<(const HTNode* b) const {
        return this->freq < b->freq;
    }
};

struct HTNodeCmp {
	bool operator()(const HTNode* a, const HTNode* b) const {
        return a->freq > b->freq;
    }
}; 
#endif