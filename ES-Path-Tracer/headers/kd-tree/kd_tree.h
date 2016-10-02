#ifndef __GUARD_KD_TREE_H__
#define __GUARD_KD_TREE_H__

#include "geometry/triangle.h"
#include "geometry/vector3.h"

#include <memory>
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

    const std::shared_ptr<const KD_Node> left;
    const std::shared_ptr<const KD_Node> right;
    
    KD_Middle_Node(int bound_offset, int median_value, 
		const std::shared_ptr<const KD_Node> &left_child, 
		const std::shared_ptr<const KD_Node> &right_child);
};


// Leaves in the KD-Tree
class KD_Leaf : KD_Node {
public:
    const std::shared_ptr<const Triangle> tri;

    KD_Leaf(const std::shared_ptr<const Triangle> &triangle);
};


class KD_Tree {
public:
    static std::shared_ptr<KD_Tree> build_tree(std::vector<Triangle>& triangles);

    void search( /* TODO... */ );

private:
    const std::shared_ptr<const KD_Node> root;
	unsigned int dim_counter;

    KD_Tree(const std::shared_ptr<const KD_Node> &root);
};

#endif