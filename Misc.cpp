#include <iostream>
#include <cassert>
#include <vector>

using namespace std;

// Disjoint Sets
class DSNode{
public:
    int size;
    int rank;
    DSNode* parent;
    DSNode(){
        size = 1;
        rank = 0;
        parent = this;
    }
};

DSNode* findRoot(DSNode* node){
    while (node != node->parent) node = node->parent;
    return node;
}

void makeUnion(int a, int b, vector<DSNode*> nodes){
    DSNode* aroot = findRoot(nodes[a]);
    DSNode* broot = findRoot(nodes[b]);
    if (aroot == broot) return;
    if (aroot->rank >= broot->rank){
        broot->parent = aroot;
        aroot->size += broot->size;
    } else {
        aroot->parent = broot;
        broot->size += aroot->size;
    }
    if (aroot->rank == broot->rank) aroot->rank++;
}


// Tests
int main() {
    vector<DSNode*> DS;
    for (int i = 0; i < 8; i++) DS.push_back(new DSNode());
    makeUnion(0, 7, DS);
    makeUnion(1, 5, DS);
    makeUnion(4, 0, DS);
    makeUnion(5, 7, DS);
    makeUnion(2, 3, DS);
    assert(findRoot(DS[0])->size == 5);
    assert(findRoot(DS[1])->size == 5);
    assert(findRoot(DS[2])->size == 2);
    assert(findRoot(DS[3])->size == 2);
    assert(findRoot(DS[4])->size == 5);
    assert(findRoot(DS[5])->size == 5);
    assert(findRoot(DS[6])->size == 1);
    assert(findRoot(DS[7])->size == 5);

    return 0;
}
