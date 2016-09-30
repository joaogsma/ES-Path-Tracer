#include "kd_tree.h"
#include "../primitives/triangle.h"

KD_Middle_Node::KD_Middle_Node(int bound_offset, int median_value, 
            KD_Node* left_child, KD_Node* right_child) : 
            bound_offset(bound_offset), median_value(median_value),
            left(left_child), right(right_child) {}

KD_Middle_Node::~KD_Middle_Node() {
    delete left;
    delete right;
}

KD_Leaf_Node::KD_Leaf_Node(Triangle triangle) : tri(triangle) {}