#ifndef __GUARD_KD_TREE_H__
#define __GUARD_KD_TREE_H__

#include "../primitives/Vector3.h"
#include <vector>
#include <pair>

struct Region {
    std::vector<std::pair<double, double> > bounds;
};

class KD_Node {
    // TODO...
};

class KD_Leaf : KD_Node {
    // TODO...
};


class KD_Tree {
public:
    KD_Tree(unsigned int dimensions);

    ~KD_Tree();

    void add_point(Vector3 point);

    void remove_point(Vector3 point);

    void search( /* TODO... */ );

private:
    KD_Node* root;
    const unsigned int dimensions;
    unsigned int dim_counter;
};

#endif