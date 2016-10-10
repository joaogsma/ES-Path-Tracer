#ifndef __GUARD_KD_TREE_H__
#define __GUARD_KD_TREE_H__

#include "geometry/triangle.h"
#include "geometry/vector3.h"

#include <vector>

/*	The Region struct represents the region of the plane defined by each node 
	in a kd-tree. */
struct Region { double min_x, max_x, min_y, max_y, min_z, max_z; };


// Abstract node class
class KD_Node {
public:
	KD_Node() {}
	virtual ~KD_Node() {}
};


/*	KD_Middle_Node objects correspond to non-leaf nodes in the kd-tree. They 
	contain the median value based on which they split the data, pointers to 
	the	left and right subtrees and a bound offset. The bound offset is the 
	ammount	by which the bounding box of the KD_Middle_Node's tree goes 
	beyond the plane defined by it's parent median_value. */
class KD_Middle_Node : public KD_Node {
public:
    const double split_value;
	const double left_bound_offset, right_bound_offset;

    const KD_Node * const left;
    const KD_Node * const right;
    
    KD_Middle_Node(double median_value, double left_bound_offset, 
		double right_bound_offset, const KD_Node * const left_child, 
		const KD_Node * const right_child);

	~KD_Middle_Node();
};


/*	KD_Leaf objects correspond to leaf nodes in the kd-tree. They contain the
	corresponding triangle but no bounding box, as it's faster to intersect a
	ray with a triangle than with a box. */
class KD_Leaf : public KD_Node {
public:
    const Triangle * const tri;

    KD_Leaf(const Triangle * const triangle);

	~KD_Leaf();
};


/*	KD_Tree objects correspond to an enture kd-tree. They are created throught 
	the build_tree function, and can be queries through the search function,
	which receives an intesection test function. */
class KD_Tree {
public:
    static KD_Tree* build_tree(const std::vector<const Triangle*>& triangles);

	~KD_Tree();

	void search(std::vector<const Triangle*>& intersected,
		bool contains_region(const Region&),
		bool intersects_region(const Region&),
		bool intersects_triangle(const Triangle&)) const;

private:
    const KD_Node* const root;
	const Region bounding_box;
	unsigned int dim_counter;

    KD_Tree(KD_Node* root, Region bounding_box);

	//static double median(const std::vector<const Triangle*>& values, int dimension);
	
	static bool compare_by_x(const Triangle* a, const Triangle* b);

	static bool compare_by_y(const Triangle* a, const Triangle* b);

	static bool compare_by_z(const Triangle* a, const Triangle* b);

	static bool at_left_half_space(const Triangle* tri, int dimension, double split_value);

	static void KD_Tree::sort_triangles(const std::vector<const Triangle*>& triangles,
		std::vector<std::vector<const Triangle*> >& sorted_triangles);

	static KD_Node* build_tree(std::vector<std::vector<const Triangle*> >& triangles, 
		int dimension, Region& bounding_box);

	void search( const KD_Node* const current, Region &region, 
		int dimension, std::vector<const Triangle*>& intersected, 
		bool contains_region(const Region&),
		bool intersects_region(const Region&),
		bool intersects_triangle(const Triangle&) ) const;

	void report_subtree( const KD_Node* const root, 
		std::vector<const Triangle*>& intersected ) const;

};

#endif