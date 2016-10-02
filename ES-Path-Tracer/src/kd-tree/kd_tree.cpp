#include "geometry/triangle.h"
#include "kd-tree/kd_tree.h"

#include <memory>

using std::shared_ptr;

// ============================================================================
// ============================== KD_MIDDLE_NODE ==============================
// ============================================================================

KD_Middle_Node::KD_Middle_Node(int bound_offset, int median_value, 
	const shared_ptr<const KD_Node> &left_child, 
	const shared_ptr<const KD_Node> &right_child) :
	bound_offset(bound_offset), median_value(median_value),
	left(left_child), right(right_child) {}

// ============================================================================



// ============================================================================
// ================================= KD_LEAF ==================================
// ============================================================================

KD_Leaf::KD_Leaf(const shared_ptr<const Triangle> &triangle) : tri(triangle) {}

// ============================================================================



// ============================================================================
// ================================= KD_TREE ==================================
// ============================================================================

KD_Tree::KD_Tree(const shared_ptr<const KD_Node> &root) : dim_counter(0), root(root) {}

std::shared_ptr<KD_Tree> KD_Tree::build_tree(std::vector<Triangle>& triangles)
{
	return nullptr;
}

void KD_Tree::search()
{
	
}

// ============================================================================