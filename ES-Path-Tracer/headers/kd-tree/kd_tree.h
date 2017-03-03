#ifndef __GUARD_KD_TREE_H__
#define __GUARD_KD_TREE_H__

#include "geometry/ray.h"
#include "geometry/plane.h"
#include "geometry/triangle.h"

#include <iterator>
#include <vector>
#include <type_traits>

namespace kd_tree {
    /*	The AAB struct represents the region of the plane defined by each node 
	    in a kd-tree. */
    struct AAB { double min_x, max_x, min_y, max_y, min_z, max_z; };

    bool operator==(const AAB &a, const AAB &b);

    // Abstract node class
    class KD_Node {
    public:
        virtual ~KD_Node() = 0;
    };


    // KD_Middle_Node objects correspond to non-leaf nodes in the kd-tree
    class KD_Middle_Node : public KD_Node {
    public:
        const Plane split_plane;

        const KD_Node * const left;
        const KD_Node * const right;

        KD_Middle_Node(Plane split_plane, const KD_Node* left_child,
            const KD_Node* right_child)
            : split_plane(split_plane), left(left_child), right(right_child) {}

        ~KD_Middle_Node() { delete left; delete right; }
    };


    /*	KD_Leaf objects correspond to leaf nodes in the kd-tree. They contain a list of triangles, 
        because recursion stops as soon as a brute-force search is estimated to be more efficient 
        than increasing the tree size. */
    class KD_Leaf : public KD_Node {
    public:
        std::vector<const Triangle*> triangles;

        KD_Leaf(const std::vector<const Triangle*>& triangles) : triangles(triangles) {}

        ~KD_Leaf() {}
    };


    class KD_Tree_Build_Event;

    // KD_Tree objects correspond to an entire kd-tree
    class KD_Tree {
    public:
        ~KD_Tree() { delete root; }

        KD_Tree(const std::vector<const Triangle*>& triangles) :
            root(rec_build_tree(triangles, bounding_box)),
            bounding_box(compute_aabb(triangles)) {}

        bool search(const Ray &ray, Triangle const *&tri, double &t) const;

    private:
        static const int TRAVERSAL_COST;
        static const int TRIANGLE_INTERSECTION_COST;
        static const double COST_FUNCTION_BIAS;
        
        const AAB bounding_box;
        const KD_Node* const root;

        enum SIDE { LEFT, RIGHT };

        KD_Node* rec_build_tree(const std::vector<const Triangle*>& triangles, AAB region);
 
        void KD_Tree::find_plane(const std::vector<const Triangle*>& triangles, AAB region,
            int &axis, double &plane_pos, KD_Tree::SIDE &plane_side);
        
        void create_events(const Triangle &tri, const AAB &region,
            std::vector<KD_Tree_Build_Event> &x_event_queue, std::vector<KD_Tree_Build_Event> &y_event_queue,
            std::vector<KD_Tree_Build_Event> &z_event_queue);

        void sweep_plane(std::vector<KD_Tree_Build_Event> &event_queue, int axis, const AAB &region,
            size_t num_triangles, double &best_cost, double &best_plane, KD_Tree::SIDE &best_side);

        std::pair<AAB, AAB> KD_Tree::split_region(AAB region, int axis, double pos);
        
        AAB compute_aabb(const std::vector<const Triangle*>& triangles);
        AAB compute_aabb(const Triangle& triangle);

        AAB clipped_triangle_aabb(const Triangle& triangle, const AAB& region);
        
        bool has_area(const Triangle &tri, const AAB &region);

        bool on_plane(const Triangle &tri, const Plane &plane);

        bool terminate(int split_axis, double split_pos, const AAB &region, size_t num_triangles_left,
            size_t num_triangles_right, size_t num_triangles_plane);

        double surface_area(const AAB& region);

        double cost(double prob_left, double prob_right, size_t num_triangles_left,
            size_t num_triangles_right);

        double cost_bias(size_t num_triangles_left, size_t num_triangles_right);

        std::pair<double, KD_Tree::SIDE> KD_Tree::sah(int split_axis, double split_pos, AAB region,
            size_t num_triangles_left, size_t num_triangles_right, size_t num_triangles_plane);
    };
}
#endif