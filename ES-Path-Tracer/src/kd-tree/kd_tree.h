#ifndef __GUARD_KD_TREE_H__
#define __GUARD_KD_TREE_H__

#include "../geometry/vector3.h"
#include "../geometry/triangle.h"
#include <vector>
#include <utility>

struct Region { double min_x, max_x, min_y, max_y, min_z, max_z; };

// Abstract node class
class KD_Node {
public:
	KD_Node() {}
};


// Non-leaf nodes in the KD-Tree
class KD_Middle_Node : KD_Node {
public:
    const int bound_offset;
    const double median_value;

    KD_Node* const left;
    KD_Node* const right;
    
    KD_Middle_Node(int bound_offset, int median_value, KD_Node* left_child, 
			KD_Node* right_child);

    ~KD_Middle_Node();
};


// Leaves in the KD-Tree
class KD_Leaf : KD_Node {
public:
    const Triangle tri;

    KD_Leaf(Triangle triangle);
};


class KD_Tree {
public:
    ~KD_Tree();

    static KD_Tree* build_tree(std::vector<Triangle>& triangles);

    void search( /* TODO... */ );

private:
    KD_Node* const root;
	unsigned int dim_counter;

    KD_Tree(KD_Node* root);
};

#endif