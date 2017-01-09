#include "geometry/plane.h"
#include "geometry/point3.h"
#include "geometry/triangle.h"
#include "geometry/vector3.h"
#include "kd-tree/kd_tree.h"

#include <climits>
#include <stdexcept>
#include <typeinfo>
#include <vector>


namespace kd_tree {
    KD_Node::~KD_Node() {}

    const double KD_Tree::EPSILON = 1e-6;

    KD_Tree::KD_Tree(KD_Node* root, Region bounding_box) : dim_counter(0),
        root(root), bounding_box(bounding_box) {}

    KD_Tree::~KD_Tree() { delete root; }

    bool KD_Tree::compare_by_x(const Triangle* a, const Triangle* b)
    {
        double barycenter_a_x = (a->v1->x + a->v2->x + a->v3->x) / 3;
        double barycenter_b_x = (b->v1->x + b->v2->x + b->v3->x) / 3;

        return barycenter_a_x < barycenter_b_x;
    }

    bool KD_Tree::compare_by_y(const Triangle* a, const Triangle* b)
    {
        double barycenter_a_y = (a->v1->y + a->v2->y + a->v3->y) / 3;
        double barycenter_b_y = (b->v1->y + b->v2->y + b->v3->y) / 3;

        return barycenter_a_y < barycenter_b_y;
    }

    bool KD_Tree::compare_by_z(const Triangle* a, const Triangle* b)
    {
        double barycenter_a_z = (a->v1->z + a->v2->z + a->v3->z) / 3;
        double barycenter_b_z = (b->v1->z + b->v2->z + b->v3->z) / 3;

        return barycenter_a_z < barycenter_b_z;
    }

    KD_Node* KD_Tree::build_tree(std::vector<std::vector<const Triangle*> >& sorted_triangles, int axis)
    {
        // The triangle vector for the axis in which this node splits the space
        std::vector<const Triangle*>& triangles = sorted_triangles[axis];

        // If there is only one triangle, the returned node will be a KD_Leaf
        if (triangles.size() == 1)
            return new KD_Leaf(triangles.front());

        /*	There is more than one triangle, so the returned node will be a KD_Middle_Node */

        // ===== Compute the node's split value (median) =====
        std::vector<const Triangle*>::size_type mid = triangles.size() / 2;
        const Triangle* mid_triangle = triangles[mid];
        
        // Coordinate of mid triangle's barycenter
        double barycentric_mid_tri = ((*mid_triangle->v1)[axis] + (*mid_triangle->v2)[axis]
            + (*mid_triangle->v3)[axis]) / 3;

        double split_value;

        /*	Even number of triangles. The median is the average of the coordinates
            of the barycenters */
        if (triangles.size() % 2 == 0)
        {
            const Triangle* mid_prev_triangle = triangles[mid - 1];

            // Coordinate of mid prev triangle's barycenter
            double barycentric_mid_prev_tri = ( (*mid_prev_triangle->v1)[axis] +
                (*mid_prev_triangle->v2)[axis] + (*mid_prev_triangle->v3)[axis] ) / 3;

            split_value = (barycentric_mid_tri + barycentric_mid_prev_tri) / 2;
        }
        // Odd number of triangles
        else
        {
            split_value = barycentric_mid_tri;
        }
        // ===================================================


        // ===== Divide the sorted triangle lists by the split value =====
        std::vector<std::vector<const Triangle*> > left_sorted_triangles(sorted_triangles.size());
        std::vector<std::vector<const Triangle*> > right_sorted_triangles(sorted_triangles.size());

        for (unsigned int axis_idx = 0; axis_idx < sorted_triangles.size(); axis_idx++)
        {
            for (std::vector<const Triangle*>::const_iterator tri_it = sorted_triangles[axis_idx].begin();
                tri_it != sorted_triangles[axis_idx].end(); tri_it++)
            {
                const Surface_Point& v1 = *(*tri_it)->v1;
                const Surface_Point& v2 = *(*tri_it)->v2;
                const Surface_Point& v3 = *(*tri_it)->v3;

                if ( abs(v1[axis] - split_value) < KD_Tree::EPSILON || 
                    abs(v2[axis] - split_value) < KD_Tree::EPSILON ||
                    abs(v3[axis] - split_value) < KD_Tree::EPSILON )        // One of the vertices intersects the split plane
                {
                    left_sorted_triangles[axis_idx].push_back(*tri_it);
                    right_sorted_triangles[axis_idx].push_back(*tri_it);
                }
                else if ( v1[axis] < split_value && 
                    v2[axis] < split_value && v3[axis] < split_value )      // Triangle entirely in the left side
                {
                    left_sorted_triangles[axis_idx].push_back(*tri_it);
                }
                else if ( v1[axis] > split_value && 
                    v2[axis] > split_value && v3[axis] > split_value )      // Triangle entirely in the right side
                {
                    right_sorted_triangles[axis_idx].push_back(*tri_it);
                }
                else                                                        // Triangle crosses the split plane
                {
                    left_sorted_triangles[axis_idx].push_back(*tri_it);
                    right_sorted_triangles[axis_idx].push_back(*tri_it);
                }
            }
        }
        // ===============================================================

        // Recursively compute the left and right subtrees
        KD_Node* left_child = build_tree(left_sorted_triangles, (axis + 1) % 3);
        KD_Node* right_child = build_tree(right_sorted_triangles, (axis + 1) % 3);

        // Compute the split plane
        Vector3 split_plane_normal( (axis == 0), (axis == 1), (axis == 2) );
        Point3 split_plane_point = split_plane_normal * split_value;
        Plane split_plane(split_plane_point, split_plane_normal);

        return new KD_Middle_Node(split_plane, left_child, right_child);
    }

    /*	Public version of the search function. It sets the initial parameters for
        the private version */
    void KD_Tree::search(std::vector<const Triangle*>& intersected,
        bool contains_region(const Region&),
        bool intersects_region(const Region&),
        bool intersects_triangle(const Triangle&)) const
    {
        Region space;
        space.min_x = space.min_y = space.min_z = INT_MIN;
        space.max_x = space.max_y = space.max_z = INT_MAX;

        search(root, space, 0, intersected, contains_region, intersects_region,
            intersects_triangle);
    }

    // Private version of the search function. It recursively searches the kd-tree
    void KD_Tree::search(const KD_Node* const current, Region& region,
        int axis, std::vector<const Triangle*>& intersected,
        bool contains_region(const Region&),
        bool intersects_region(const Region&),
        bool intersects_triangle(const Triangle&)) const
    {
        const KD_Leaf* const current_as_leaf = dynamic_cast<const KD_Leaf*>(current);

        if (current_as_leaf)
        {
            /*	Check if the triangle contained in this leaf intersects the query region.
                If so, add it to the return list */
            if (intersects_triangle(*current_as_leaf->tri))
                intersected.push_back(current_as_leaf->tri);
        }
        else
        {
            /*	Check if the query range intersects the region corresponding to this
                subtree. This check is done here, instead of in the parent node, because
                then there is no need to check intersections with leaf bounding boxes, as
                we only need to check for intersections with the triangle contained in the
                leaf (see previous if clause) */
            if (!intersects_region(region))
                return;

            const KD_Middle_Node* const current_as_middle_node =
                static_cast<const KD_Middle_Node*>(current);

            Region left_subregion = region;
            Region right_subregion = region;

            // ===== Compute the left and right subregions =====
            const Plane& split_plane = current_as_middle_node->split_plane;
            
            switch (axis)
            {
            case 0: left_subregion.max_x = right_subregion.min_x = split_plane.point.x; break;
            case 1: left_subregion.max_y = right_subregion.min_y = split_plane.point.y; break;
            case 2: left_subregion.max_z = right_subregion.min_z = split_plane.point.z; break;
            default: throw std::logic_error("Unknown axis");
            }

            // =================================================

            // ===== Processing of left subtree =====
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
                        (axis + 1) % 3, intersected, contains_region,
                        intersects_region, intersects_triangle);
                }
            }
            // ======================================

            // ===== Processing of right subtree =====
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
                        (axis + 1) % 3, intersected, contains_region,
                        intersects_region, intersects_triangle);
                }
            }
            // =======================================
        }
    }

    void KD_Tree::report_subtree(const KD_Node* const current,
        std::vector<const Triangle*>& intersected) const
    {
        /*	If the current node is a leaf, add its triangle to the reported list.
            Otherwise, recursively continue through the left and right subtrees */

        const KD_Leaf* const current_as_leaf = dynamic_cast<const KD_Leaf*>(current);

        if (current_as_leaf)
            intersected.push_back(current_as_leaf->tri);
        else
        {
            const KD_Middle_Node* const current_as_middle_node =
                dynamic_cast<const KD_Middle_Node*>(current);

            report_subtree(current_as_middle_node->left, intersected);
            report_subtree(current_as_middle_node->right, intersected);
        }

    }
}