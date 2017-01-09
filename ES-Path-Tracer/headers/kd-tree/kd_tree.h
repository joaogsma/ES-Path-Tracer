#ifndef __GUARD_KD_TREE_H__
#define __GUARD_KD_TREE_H__

#include "geometry/plane.h"
#include "geometry/triangle.h"

#include <iterator>
#include <vector>
#include <type_traits>

namespace kd_tree {
    /*	The Region struct represents the region of the plane defined by each node 
	    in a kd-tree. */
    struct Region { double min_x, max_x, min_y, max_y, min_z, max_z; };


    // Abstract node class
    class KD_Node {
    public:
        virtual ~KD_Node() = 0;
    };


    /*	KD_Middle_Node objects correspond to non-leaf nodes in the kd-tree. They 
	    contain the median value based on which they split the data, pointers to 
	    the	left and right subtrees and a bound offset. The bound offset is the 
	    ammount	by which the bounding box of the KD_Middle_Node's tree goes 
	    beyond the plane defined by it's parent median_value. */
    class KD_Middle_Node : public KD_Node {
    public:
        const Plane split_plane;
	    
        const KD_Node * const left;
        const KD_Node * const right;
    
        KD_Middle_Node(Plane split_plane, const KD_Node * const left_child, 
		    const KD_Node * const right_child)
		    : split_plane(split_plane), left(left_child), right(right_child) {}

	    ~KD_Middle_Node() { delete left; delete right; }
    };


    /*	KD_Leaf objects correspond to leaf nodes in the kd-tree. They contain the
	    corresponding triangle but no bounding box, as it's faster to intersect a
	    ray with a triangle than with a box. */
    class KD_Leaf : public KD_Node {
    public:
        Triangle const * const tri;

	    KD_Leaf(Triangle const * const triangle) : tri(triangle) {}

        ~KD_Leaf() {}
    };


    /*	KD_Tree objects correspond to an enture kd-tree. They are created throught 
	    the build_tree function, and can be queries through the search function,
	    which receives an intesection test function. */
    class KD_Tree {
    public:
        template<class Input_It>
        static KD_Tree* build_tree(Input_It triangles_begin, Input_It triangles_end);

	    ~KD_Tree();

	    void search(std::vector<const Triangle*>& intersected,
		    bool contains_region(const Region&),
		    bool intersects_region(const Region&),
		    bool intersects_triangle(const Triangle&)) const;

    private:
        const KD_Node* const root;
	    const Region bounding_box;
	    unsigned int dim_counter;
        static const double EPSILON;

        KD_Tree(KD_Node* root, Region bounding_box);

	    static bool compare_by_x(const Triangle* a, const Triangle* b);

	    static bool compare_by_y(const Triangle* a, const Triangle* b);

	    static bool compare_by_z(const Triangle* a, const Triangle* b);

	    template<class Input_It>
        static void sort_triangles(Input_It begin, Input_It end,
            std::vector<std::vector<const Triangle*> >& sorted_triangles);

	    static KD_Node* build_tree(
            std::vector<std::vector<const Triangle*> >& sorted_triangles, int axis);

	    void search( const KD_Node* const current, Region &region, 
		    int dimension, std::vector<const Triangle*>& intersected, 
		    bool contains_region(const Region&),
		    bool intersects_region(const Region&),
		    bool intersects_triangle(const Triangle&) ) const;

	    void report_subtree( const KD_Node* const root, 
		    std::vector<const Triangle*>& intersected ) const;
    };

    template<class Input_It>
    void KD_Tree::sort_triangles(Input_It begin, Input_It end,
        std::vector<std::vector<const Triangle*> >& sorted_triangles)
    {
        sorted_triangles.clear();

        // Insert 3 copies of all the triangles, one for each axis-ordered vector
        sorted_triangles.push_back(std::vector<const Triangle*>());
        sorted_triangles.front().insert(sorted_triangles.front().end(), begin, end);
        sorted_triangles.push_back(sorted_triangles.front());
        sorted_triangles.push_back(sorted_triangles.front());

        // Sort each axis-ordered vector by the corresponding axis values
        sort(sorted_triangles[0].begin(), sorted_triangles[0].end(), compare_by_x);
        sort(sorted_triangles[1].begin(), sorted_triangles[1].end(), compare_by_y);
        sort(sorted_triangles[2].begin(), sorted_triangles[2].end(), compare_by_z);
    }

    template<class Input_It>
    KD_Tree* KD_Tree::build_tree(Input_It triangles_begin, Input_It triangles_end)
    {
        // Check if Input_It is an iterator to [const] Triangle*
        static_assert( 
            std::is_convertible<std::iterator_traits<Input_It>::value_type, const Triangle*>::value,
            "Must be Input Iterators to [const] Triangle*");

        if (triangles_end == triangles_begin)
            return nullptr;

        /*	Sort the triangles by coordinates of their barycenters, in three vectors.
        Each vector corresponds to a sorting done based in coordinates from a
        different axis (X, Y, Z) */
        vector<vector<const Triangle*> > sorted_triangles;
        sort_triangles(triangles_begin, triangles_end, sorted_triangles);

        Region inf_box;
        inf_box.max_x = inf_box.max_y = inf_box.max_z = INT_MAX;
        inf_box.min_x = inf_box.min_y = inf_box.min_z = INT_MIN;

        KD_Node* root = build_tree(sorted_triangles, 0);

        return new KD_Tree(root, inf_box);
    }
}
#endif