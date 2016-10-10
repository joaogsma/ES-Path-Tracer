#include "geometry/point3.h"
#include "geometry/triangle.h"
#include "kd-tree/kd_tree.h"

#include <algorithm>
#include <cfloat>
#include <iterator>
#include <stdexcept>
#include <vector>

using std::min;
using std::max;
using std::sort;
using std::transform;
using std::vector;

// ============================================================================
// ============================== KD_MIDDLE_NODE ==============================
// ============================================================================

KD_Middle_Node::KD_Middle_Node(double median_value, double left_bound_offset,
	double right_bound_offset, const KD_Node * const left_child,
	const KD_Node * const right_child)
	: median_value(median_value), left_bound_offset(left_bound_offset),
	right_bound_offset(right_bound_offset), left(left_child), right(right_child) {}

KD_Middle_Node::~KD_Middle_Node() {	delete left; delete right; }

// ============================================================================



// ============================================================================
// ================================= KD_LEAF ==================================
// ============================================================================

KD_Leaf::KD_Leaf(const Triangle* const triangle) : tri(triangle) {}

KD_Leaf::~KD_Leaf() { delete tri; }

// ============================================================================



// ============================================================================
// ================================= KD_TREE ==================================
// ============================================================================

static const int X = 0;
static const int Y = 1;
static const int Z = 2;

KD_Tree::KD_Tree(KD_Node* root, Region bounding_box) : dim_counter(0), 
	root(root), bounding_box(bounding_box) {}


KD_Tree::~KD_Tree() { delete root; }


double KD_Tree::median(const vector<const Triangle*>& triangles, int dimension)
{
	vector<double> values;
	
	// Extract the dimension values for each triangle barycenter
	for (vector<const Triangle*>::const_iterator it = triangles.begin();
		it != triangles.end(); it++)
	{
		const Triangle* tri = *it;
		Point3 barycenter((tri->v1->x + tri->v2->x + tri->v3->x) / 3,
			(tri->v1->y + tri->v2->y + tri->v3->y) / 3,
			(tri->v1->z + tri->v2->z + tri->v3->z) / 3);

		switch (dimension)
		{
		case X: { values.push_back(barycenter.x); break; }
		case Y: { values.push_back(barycenter.y); break; }
		case Z: { values.push_back(barycenter.z); break; }
		default: throw std::logic_error("Unknown dimension");
		}
	}
	
	// Sort the values
	sort( values.begin(), values.end() );

	// Compute the median
	vector<double>::size_type mid = values.size() / 2;
	if ( values.size() % 2 == 0 )
		return (values[mid] + values[mid - 1]) / 2;
	
	return values[mid];
}


KD_Tree* KD_Tree::build_tree(vector<const Triangle*>& triangles)
{
	Region tree_bounding_box;
	KD_Node* root = build_tree(triangles, X, tree_bounding_box);
	return new KD_Tree(root, tree_bounding_box);
}


KD_Node* KD_Tree::build_tree(vector<const Triangle*>& triangles, int dimension, 
	Region& bounding_box)
{
	if (triangles.empty())
		throw std::logic_error("Empty triangle vector");

	// If there is only one triangle, so the returned node will be a KD_Leaf
	if ( triangles.size() == 1 )
	{
		const Triangle& tri = *triangles[0];
		
		// Set the bounding box values for this leaf
		bounding_box.min_x = min(tri.v1->x, min(tri.v2->x, tri.v3->x));
		bounding_box.min_y = min(tri.v1->y, min(tri.v2->y, tri.v3->y));
		bounding_box.min_z = min(tri.v1->z, min(tri.v2->z, tri.v3->z));

		bounding_box.max_x = max(tri.v1->x, max(tri.v2->x, tri.v3->x));
		bounding_box.max_y = max(tri.v1->y, max(tri.v2->y, tri.v3->y));
		bounding_box.max_z = max(tri.v1->z, max(tri.v2->z, tri.v3->z));

		return new KD_Leaf( triangles[0] );
	}
	
	// There is more than one triangle, so the returned node will be a KD_Middle_Node
	
	// TODO... (see notebook)

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
		/*	Check if the query range intersects the region corresponding to this 
			subtree. This check is done here, instead of in the parent node, because
			then there is no need to check intersections with leaf bounding boxes, as
			we only need to check for intersections with the triangle contained in the 
			leaf (see previous if clause) */
		if ( !intersects_region(region) )
			return;
		
		const KD_Middle_Node* const current_as_middle_node = 
			static_cast<const KD_Middle_Node*>(current);

		Region left_subregion = region;
		Region right_subregion = region;
		
		// ===== Compute the left and right subregions =====
		double *left_max_dim = nullptr, *right_min_dim = nullptr;
		// Find which dimension is the one corresponding to this node
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

		default:
			throw std::logic_error("Unknown dimension");
			break;
		}

		// Adjust the region boundary of the left subregion
		*left_max_dim = current_as_middle_node->median_value 
			+ current_as_middle_node->left_bound_offset;

		// Adjust the region boundary of the right subregion
		*right_min_dim = current_as_middle_node->median_value
			- current_as_middle_node->right_bound_offset;
		// =================================================

		// ===== Processing of left subtree =====
		{
			if (current_as_middle_node->left)
			{
				/*	Report the entire left subtree if it is fully contained in the 
					query region */
				if (contains_region(left_subregion))
				{
					report_subtree(current_as_middle_node->left, intersected);
				}
				/*	The left subtree is not entirely contained in the query region, 
					but they might intersect */
				else
				{
					/*	Recursively continue through the left subtree if the left 
						subregion intersects the query subregion */
					search(current_as_middle_node->left, left_subregion,
						(dimension + 1) % 3, intersected, contains_region,
						intersects_region, intersects_triangle);
				}
			}
		}
		// ======================================
		
		// ===== Processing of right subtree =====
		{
			if ( current_as_middle_node->right )
			{
				/*	Report the entire right subtree if it is fully contained in 
					the query region */
				if (contains_region(right_subregion))
				{
					report_subtree(current_as_middle_node->right, intersected);
				}
				/*	The right subtree is not entirely contained in the query region,
					but they might intersect */
				else
				{
					/*	Recursively continue through the right subtree if the 
						right subregion intersects the query subregion */
					search(current_as_middle_node->right, right_subregion,
						(dimension + 1) % 3, intersected, contains_region,
						intersects_region, intersects_triangle);
				}
			}
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


