#include "geometry/triangle.h"
#include "kd-tree/kd_tree.h"

#include <cfloat>
#include <vector>

using std::vector;

// ============================================================================
// ============================== KD_MIDDLE_NODE ==============================
// ============================================================================

KD_Middle_Node::KD_Middle_Node(double bound_offset, int median_value, 
	const KD_Node * const left_child, 
	const KD_Node * const right_child) :
	KD_Node(bound_offset), median_value(median_value),
	left(left_child), right(right_child) {}

KD_Middle_Node::~KD_Middle_Node() {	delete left; delete right; }

// ============================================================================



// ============================================================================
// ================================= KD_LEAF ==================================
// ============================================================================

KD_Leaf::KD_Leaf(double bound_offset, const Triangle* const triangle)
	: KD_Node(0), tri(triangle) {}

KD_Leaf::~KD_Leaf() { delete tri; }

// ============================================================================



// ============================================================================
// ================================= KD_TREE ==================================
// ============================================================================

static const int X = 0;
static const int Y = 1;
static const int Z = 2;

KD_Tree::KD_Tree(const KD_Node * const root) : dim_counter(0), root(root) {}

KD_Tree::~KD_Tree() { delete root; }

KD_Tree* KD_Tree::build_tree(std::vector<Triangle>& triangles)
{
	// TODO...
	// Remember to compute bound_offsets of KD_Leaf objects as well
	return nullptr;
}


/*	Public version of the search function. It sets the initial parameters for
	the private version */
void KD_Tree::search(vector<const Triangle*>& intersected,
	bool contains_region(const Region&),
	bool intersects_region(const Region&),
	bool intersects_triangle(const Triangle&)) const
{
	Region space;
	space.min_x = DBL_MIN;
	space.min_y = DBL_MIN;
	space.min_z = DBL_MIN;
	space.max_x = DBL_MAX;
	space.max_y = DBL_MAX;
	space.max_z = DBL_MAX;

	search( root, space, X, intersected, contains_region, intersects_region, 
		intersects_triangle );
}


// Private version of the search function. It recursively searches the kd-tree
void KD_Tree::search(const KD_Node* const current, Region& region,
	int dimension, vector<const Triangle*>& intersected,
	bool contains_region(const Region&),
	bool intersects_region(const Region&),
	bool intersects_triangle(const Triangle&) ) const
{
	const KD_Leaf* const current_as_leaf = dynamic_cast<const KD_Leaf*>(current);

	if (current_as_leaf)
	{
		/*	Check if the triangle contained in this leaf intersects the query region. 
			If so, add it to the return list */
		if ( intersects_triangle(*current_as_leaf->tri) )
			intersected.push_back(current_as_leaf->tri);
	}
	else
	{
		const KD_Middle_Node* const current_as_middle_node = 
			static_cast<const KD_Middle_Node*>(current);

		Region left_subregion = region;
		Region right_subregion = region;
		
		double *left_max_dim = nullptr, *right_min_dim = nullptr;
		switch (dimension)
		{
		case X:
			left_max_dim = &left_subregion.max_x;
			right_min_dim = &right_subregion.min_x;
			break;
		
		case Y:
			left_max_dim = &left_subregion.max_y;
			right_min_dim = &right_subregion.min_y;
			break;

		case Z:
			left_max_dim = &left_subregion.max_z;
			right_min_dim = &right_subregion.min_z;
			break;
		}

		// Adjust the reagion boundaries of the left and right subregions
		*left_max_dim = current_as_middle_node->median_value 
			+ current_as_middle_node->left->bound_offset;

		*right_min_dim = current_as_middle_node->median_value
			- current_as_middle_node->right->bound_offset;

		// ===== Processing of left subtree =====
		// Report the entire left subtree if it is fully contained in the query region
		if (contains_region(left_subregion))
			report_subtree(current_as_middle_node->left, intersected);

		/*	Recursively continue through the left subtree if the left subregion
			intersects the query subregion */
		else if (intersects_region(left_subregion))
		{
			search( current_as_middle_node->left, left_subregion, 
				(dimension + 1) % 3, intersected, contains_region, 
				intersects_region, intersects_triangle );
		}
		// ======================================
		
		// ===== Processing of right subtree =====
		/*	Report the entire right subtree if it is fully contained in the 
			query region */
		if (contains_region(right_subregion))
			report_subtree(current_as_middle_node->right, intersected);

		/*	Recursively continue through the right subtree if the right subregion
			intersects the query subregion */
		else if (intersects_region(right_subregion))
		{
			search(current_as_middle_node->right, right_subregion,
				(dimension + 1) % 3, intersected, contains_region,
				intersects_region, intersects_triangle);
		}
		// =======================================
	}
}


void KD_Tree::report_subtree(const KD_Node* const current,
	vector<const Triangle*>& intersected) const
{
	/*	If the current node is a leaf, add its triangle to the reported list. 
		Otherwise, recursively continue through the left and right subtrees */
	
	const KD_Leaf* const current_as_leaf = dynamic_cast<const KD_Leaf*>(current);

	if (current_as_leaf)
		intersected.push_back( current_as_leaf->tri );
	else
	{
		const KD_Middle_Node* const current_as_middle_node = 
			dynamic_cast<const KD_Middle_Node*>(current);

		report_subtree( current_as_middle_node->left, intersected );
		report_subtree( current_as_middle_node->right, intersected );
	}
	
}

// ============================================================================


