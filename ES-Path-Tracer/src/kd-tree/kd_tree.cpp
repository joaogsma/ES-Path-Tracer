#include "geometry/ray.h"
#include "geometry/plane.h"
#include "geometry/point3.h"
#include "geometry/triangle.h"
#include "geometry/vector3.h"
#include "kd-tree/kd_tree.h"

#include <algorithm>
#include <cmath>
#include <climits>
#include <stack>
#include <utility>
#include <vector>

namespace kd_tree
{    
    static const double epsilon = 10e-6;


    KD_Node::~KD_Node() {}


    // ============================================================================================
    // ====================================== KD-TREE SEARCH ======================================
    // ============================================================================================

    struct Stack_Element {
        KD_Node const *node;
        double entry_t, exit_t;

        Stack_Element(const KD_Node *node, double entry_t, double exit_t)
            : node(node), entry_t(entry_t), exit_t(exit_t) {}
    };

    const Triangle* KD_Tree::intersect(Ray ray) const
    {
        // Add a small epsilon to zero coordinates in the ray normalize
        for (Vector3::iterator it = ray.direction.begin(); it != ray.direction.end(); ++it)
            if (*it == 0) 
                *it += 1e-8;
        
        double entry_t, exit_t;
        if ( !ray.intersect(bounding_box, entry_t, exit_t) )
            return nullptr;    // Return false if ray does not intersect tree's AABB

        std::stack<Stack_Element> traversal_stack;
        traversal_stack.push( Stack_Element(root, entry_t, exit_t) );

        while ( !traversal_stack.empty() )
        {
            const Stack_Element &elem = traversal_stack.top();

            // Update iteration variables
            const KD_Node *current_node = elem.node;
            const KD_Middle_Node *current_middle_node;
            entry_t = elem.entry_t;
            exit_t = elem.exit_t;

            // Remove top element from the stack
            traversal_stack.pop();

            while ( current_middle_node = dynamic_cast<const KD_Middle_Node*>(current_node) )
            {
                /*  The tree is build with splitting plane's normal being 
                    either [1, 0, 0], [0, 1, 0] or [0, 0, 1] */
                int axis = (int) dot_prod(current_middle_node->split_plane.normal, Vector3(0, 1, 2));
                double plane_pos = current_middle_node->split_plane.point[axis];

                /*  Special cases for t:
                        * + or - Inf for normalize 0 in this axis
                        * NaN for ray contained in the plane (avoided by adding epsilon before) */
                double invdir = ray.direction[axis];
                double t = (plane_pos - ray.origin[axis]) / invdir;

                // Classify children as near and far
                const KD_Node *near, *far;
                if (invdir >= 0)
                {
                    near = current_middle_node->left;
                    far = current_middle_node->right;
                }
                else
                {
                    near = current_middle_node->right;
                    far = current_middle_node->left;
                }

                // ===== Handle t =====
                if ( t >= exit_t )
                    current_node = near;   // Skip the far node, the ray does not intersect it
                else if ( t <= entry_t )
                    current_node = far;    // Skip the near this node, the ray does not intersect it
                else    // Both nodes need to be visited
                {
                    traversal_stack.push( Stack_Element(far, t, exit_t) );
                    current_node = near;
                    exit_t = t;
                }
                // ====================
            }

            const std::vector<const Triangle*> &triangles = ((const KD_Leaf*)current_node)->triangles;

            double intersection_t = INFINITY;
            const Triangle *intersection_tri = nullptr;

            // Intersect ray with each object
            for (std::vector<const Triangle*>::const_iterator it = triangles.begin();
                it != triangles.end(); ++it)
            {
                const Triangle &current_tri = **it;
                double current_tri_t;
                std::vector<double> current_tri_bar_weights;
                
                // Check if the current triangle is intersect and if so, keep the best one
                if ( ray.intersect(current_tri, current_tri_t, current_tri_bar_weights) && 
                    current_tri_t < intersection_t )
                {
                    intersection_t = current_tri_t;
                    intersection_tri = &current_tri;
                }
            }

            // Return an intersection, if found
            if (intersection_t < INFINITY)
                return intersection_tri;
        }

        return nullptr;
    }

    // ============================================================================================



    // ============================================================================================
    // =================================== KD-TTREE BUILD EVENT ===================================
    // ============================================================================================

    class KD_Tree_Build_Event {
    public:
        enum Type { END = 0, PLANE = 1, BEGIN = 2 };

        Triangle const *tri;
        double position;
        Type type;

        KD_Tree_Build_Event(const Triangle* triangle, double event_position, Type event_type)
            : tri(triangle), position(event_position), type(event_type) {}
    };

    bool operator<(const KD_Tree_Build_Event& e1, const KD_Tree_Build_Event& e2)
    {
        if (std::abs(e1.position - e2.position) < epsilon && e1.type < e2.type)
            return true;

        if (e1.position < e2.position)
            return true;

        return false;
    }

    // ============================================================================================



    // ============================================================================================
    // ================================ KD-TREE BUILD DEFINITIONS =================================
    // ============================================================================================

    const int KD_Tree::TRAVERSAL_COST = 1;                  // Dummy value
    const int KD_Tree::TRIANGLE_INTERSECTION_COST = 3;      // Dummy value
    const double KD_Tree::COST_FUNCTION_BIAS = 0.8;

    inline double KD_Tree::cost_bias(size_t num_triangles_left, size_t num_triangles_right)
    {
        if (num_triangles_left == 0 || num_triangles_right == 0)
            return 0.8;
        return 1;
    }

    inline double KD_Tree::cost(double prob_left, double prob_right, size_t num_triangles_left,
        size_t num_triangles_right)
    {
        double estimated_cost = KD_Tree::TRAVERSAL_COST + KD_Tree::TRIANGLE_INTERSECTION_COST *
            (prob_left * num_triangles_left + prob_right * num_triangles_right);
        return cost_bias(num_triangles_left, num_triangles_right) * estimated_cost;
    }

    inline double KD_Tree::surface_area(const AAB& region)
    {
        double delta_x = region.max_x - region.min_x;
        double delta_y = region.max_y - region.min_y;
        double delta_z = region.max_z - region.min_z;

        // Number os sides orthogonal to the Z axis
        int z_num_sides = (1 + (region.max_z - region.min_z >= epsilon));
        // Number os sides orthogonal to the Y axis
        int y_num_sides = (1 + (region.max_y - region.min_y >= epsilon));
        // Number os sides orthogonal to the X axis
        int x_num_sides = (1 + (region.max_x - region.min_x >= epsilon));

        return (z_num_sides * delta_x * delta_y ) + (y_num_sides * delta_x * delta_z) + 
            (x_num_sides * delta_y * delta_z);
    }

    std::pair<double, KD_Tree::SIDE> KD_Tree::sah(int split_axis, double split_pos, AAB region,
        size_t num_triangles_left, size_t num_triangles_right, size_t num_triangles_plane)
    {
        const std::pair<AAB, AAB>& subregions = split_region(region, split_axis, split_pos);
        const AAB& left_subregion = subregions.first;
        const AAB& right_subregion = subregions.second;

        const double region_surface_area = surface_area(region);
        const double prob_left = surface_area(left_subregion) / region_surface_area;
        const double prob_right = surface_area(right_subregion) / region_surface_area;

        double cost_left = cost(prob_left, prob_right, num_triangles_left + num_triangles_plane,
            num_triangles_right);
        double cost_right = cost(prob_left, prob_right, num_triangles_left,
            num_triangles_right + num_triangles_plane);

        if (cost_left < cost_right)
            return std::make_pair(cost_left, KD_Tree::SIDE::LEFT);
        else
            return std::make_pair(cost_right, KD_Tree::SIDE::RIGHT);
    }

    AAB KD_Tree::compute_aabb(const std::vector<const Triangle*>& triangles)
    {
        AAB region;
        region.max_x = region.max_y = region.max_z = INT_MIN;
        region.min_x = region.min_y = region.min_z = INT_MAX;

        // Compute bounding box for triangles
        for (std::vector<const Triangle*>::const_iterator tri_it = triangles.begin();
            tri_it != triangles.end(); ++tri_it)
        {
            // Compute current triangle's AABB
            AAB aabb = compute_aabb(**tri_it);

            // Expand AABB for the vector
            region.max_x = std::max(region.max_x, aabb.max_x);
            region.max_y = std::max(region.max_y, aabb.max_y);
            region.max_z = std::max(region.max_z, aabb.max_z);
            region.min_x = std::min(region.min_x, aabb.min_x);
            region.min_y = std::min(region.min_y, aabb.min_y);
            region.min_z = std::min(region.min_z, aabb.min_z);
        }

        return region;
    }

    AAB KD_Tree::compute_aabb(const Triangle& triangle)
    {
        AAB region;
        region.max_x = region.max_y = region.max_z = INT_MIN;
        region.min_x = region.min_y = region.min_z = INT_MAX;

        for (int i = 0; i < 3; ++i)
        {
            // Update max region m_vertices
            region.max_x = std::max(region.max_x, triangle.vertex(i)->x);
            region.max_y = std::max(region.max_y, triangle.vertex(i)->y);
            region.max_z = std::max(region.max_z, triangle.vertex(i)->z);
            // Update min region m_vertices
            region.min_x = std::min(region.min_x, triangle.vertex(i)->x);
            region.min_y = std::min(region.min_y, triangle.vertex(i)->y);
            region.min_z = std::min(region.min_z, triangle.vertex(i)->z);
        }

        return region;
    }

    KD_Node* KD_Tree::rec_build_tree(const std::vector<const Triangle*>& triangles, AAB region)
    {
        if ( triangles.empty() )
            return new KD_Leaf(triangles);

        // ========== Find the best partition plane ==========
        int axis;
        double plane_pos;
        KD_Tree::SIDE plane_side;
        find_plane(triangles, region, axis, plane_pos, plane_side);

        // Dummy initial plane
        Plane partition_plane(Point3(0, 0, 0), Vector3(1, 0, 0));

        // Compute partition plane
        switch (axis)
        {
        case 0: { partition_plane = Plane(Point3(plane_pos, 0, 0), Vector3(1, 0, 0)); break; }
        case 1: { partition_plane = Plane(Point3(0, plane_pos, 0), Vector3(0, 1, 0)); break; }
        case 2: { partition_plane = Plane(Point3(0, 0, plane_pos), Vector3(0, 0, 1)); break; }
        }
        // ===================================================


        // Divide the region into left and right subregions
        const std::pair<AAB, AAB> split_regions = split_region(region, axis, plane_pos);
        const AAB &left_subregion = split_regions.first;
        const AAB &right_subregion = split_regions.second;

        std::vector<const Triangle*> left_triangles, right_triangles, plane_triangles;


        // ========== Classify triangles ==========
        for (std::vector<const Triangle*>::const_iterator tri_it = triangles.begin();
            tri_it != triangles.end(); ++tri_it)
        {
            const Triangle* tri = *tri_it;

            if (on_plane(*tri, partition_plane))    // Triangle list on the plane - perfect split
                plane_triangles.push_back(tri);
            else
            {
                // Add the triangle to the list(s) it intersects and has area
                if (has_area(*tri, left_subregion))
                    left_triangles.push_back(tri);
                if (has_area(*tri, right_subregion))
                    right_triangles.push_back(tri);
            }
        }
        // ========================================


        // Termination criteria
        if (terminate(axis, plane_pos, region, left_triangles.size(),
            right_triangles.size(), plane_triangles.size()))
        {
            return new KD_Leaf(triangles);
        }

        // Add the plane triangles to the best side (less costly)
        if (plane_side == KD_Tree::SIDE::LEFT)
            left_triangles.insert(left_triangles.end(), plane_triangles.begin(), plane_triangles.end());
        else
            right_triangles.insert(right_triangles.end(), plane_triangles.begin(), plane_triangles.end());

        KD_Node *left_child, *right_child;

        // Continue recursion in the left subtree
        if ( left_subregion == region )
            left_child = new KD_Leaf(left_triangles);    // AAB was not changed - create a leaf
        else
            left_child = rec_build_tree(left_triangles, left_subregion);    // Continue recursion

        // Continue recursion in the right subtree
        if ( right_subregion == region )
            right_child = new KD_Leaf(right_triangles);    // AAB was not changed - create a leaf
        else
            right_child = rec_build_tree(right_triangles, right_subregion);    // Continue recursion

        return new KD_Middle_Node(partition_plane, left_child, right_child);
    }

    AAB KD_Tree::clipped_triangle_aabb(const Triangle& triangle, const AAB& region)
    {
        AAB triangle_aabb = compute_aabb(triangle);

        triangle_aabb.max_x = std::min(triangle_aabb.max_x, region.max_x);
        triangle_aabb.min_x = std::max(triangle_aabb.min_x, region.min_x);

        triangle_aabb.max_y = std::min(triangle_aabb.max_y, region.max_y);
        triangle_aabb.min_y = std::max(triangle_aabb.min_y, region.min_y);

        triangle_aabb.max_z = std::min(triangle_aabb.max_z, region.max_z);
        triangle_aabb.min_z = std::max(triangle_aabb.min_z, region.min_z);

        return triangle_aabb;
    }

    void KD_Tree::create_events(const Triangle &tri, const AAB &region,
        std::vector<KD_Tree_Build_Event> &x_event_queue, std::vector<KD_Tree_Build_Event> &y_event_queue,
        std::vector<KD_Tree_Build_Event> &z_event_queue)
    {
        AAB clipped_tri_aabb = clipped_triangle_aabb(tri, region);

        // ===== X axis events =====
        if (clipped_tri_aabb.max_x - clipped_tri_aabb.min_x < epsilon)
        {
            x_event_queue.push_back(KD_Tree_Build_Event(&tri, clipped_tri_aabb.min_x,
                KD_Tree_Build_Event::Type::PLANE));
        }
        else
        {
            x_event_queue.push_back(KD_Tree_Build_Event(&tri, clipped_tri_aabb.min_x,
                KD_Tree_Build_Event::Type::BEGIN));
            x_event_queue.push_back(KD_Tree_Build_Event(&tri, clipped_tri_aabb.max_x,
                KD_Tree_Build_Event::Type::END));
        }
        // =========================


        // ===== Y axis events =====
        if (clipped_tri_aabb.max_y - clipped_tri_aabb.min_y < epsilon)
        {
            y_event_queue.push_back(KD_Tree_Build_Event(&tri, clipped_tri_aabb.min_y,
                KD_Tree_Build_Event::Type::PLANE));
        }
        else
        {
            y_event_queue.push_back(KD_Tree_Build_Event(&tri, clipped_tri_aabb.min_y,
                KD_Tree_Build_Event::Type::BEGIN));
            y_event_queue.push_back(KD_Tree_Build_Event(&tri, clipped_tri_aabb.max_y,
                KD_Tree_Build_Event::Type::END));
        }
        // =========================


        // ===== Z axis events =====
        if (clipped_tri_aabb.max_z - clipped_tri_aabb.min_z < epsilon)
        {
            z_event_queue.push_back(KD_Tree_Build_Event(&tri, clipped_tri_aabb.min_z,
                KD_Tree_Build_Event::Type::PLANE));
        }
        else
        {
            z_event_queue.push_back(KD_Tree_Build_Event(&tri, clipped_tri_aabb.min_z,
                KD_Tree_Build_Event::Type::BEGIN));
            z_event_queue.push_back(KD_Tree_Build_Event(&tri, clipped_tri_aabb.max_z,
                KD_Tree_Build_Event::Type::END));
        }
        // =========================
    }

    void KD_Tree::find_plane(const std::vector<const Triangle*>& triangles, AAB region,
        int &axis, double &plane_pos, KD_Tree::SIDE &plane_side)
    {
        std::vector<KD_Tree_Build_Event> x_event_queue, y_event_queue, z_event_queue;

        // Create events for all triangles
        for (std::vector<const Triangle*>::const_iterator tri_it = triangles.begin();
            tri_it != triangles.end(); ++tri_it)
        {
            create_events(**tri_it, region, x_event_queue, y_event_queue, z_event_queue);
        }

        // Sort events based on their plane position
        std::sort(x_event_queue.begin(), x_event_queue.end());
        std::sort(y_event_queue.begin(), y_event_queue.end());
        std::sort(z_event_queue.begin(), z_event_queue.end());

        // Create vector of references for the comming loop
        std::vector<std::vector<KD_Tree_Build_Event>*> queues = { &x_event_queue, 
            &y_event_queue, &z_event_queue };

        // Best cost found so far
        double best_cost = INT_MAX;

        // Iterate over all event lists, and keep the best cost partitioning plane
        for (int i = 0; i < 3; ++i)
        {
            double cost, plane;
            KD_Tree::SIDE side;
            sweep_plane(*queues[i], i, region, triangles.size(), cost, plane, side);

            if (cost < best_cost)
            {
                axis = i;
                plane_pos = plane;
                plane_side = side;
                best_cost = cost;
            }
        }
    }

    void KD_Tree::sweep_plane(std::vector<KD_Tree_Build_Event> &event_queue, int axis, const AAB &region,
        size_t num_triangles, double &best_cost, double &best_plane, KD_Tree::SIDE &best_side)
    {
        best_cost = INT_MAX;
        // Number of triangles in the left, right and contained in the current plane
        size_t num_tri_left = 0, num_tri_right = num_triangles, num_tri_plane = 0;

        std::vector<KD_Tree_Build_Event>::const_iterator event_it = event_queue.begin();

        while ( event_it != event_queue.end() )
        {
            double plane_pos = event_it->position;

            size_t plane_begin = 0, plane_end = 0, plane_on = 0;

            // Count the triangles that end in this plane
            while (event_it != event_queue.end() && event_it->position == plane_pos &&
                event_it->type == KD_Tree_Build_Event::Type::END)
            {
                ++plane_end;
                ++event_it;
            }

            // Count the triangles that are contained in this plane
            while (event_it != event_queue.end() && event_it->position == plane_pos &&
                event_it->type == KD_Tree_Build_Event::Type::PLANE)
            {
                ++plane_on;
                ++event_it;
            }

            // Count the triangles that begin in this plane
            while (event_it != event_queue.end() && event_it->position == plane_pos &&
                event_it->type == KD_Tree_Build_Event::Type::BEGIN)
            {
                ++plane_begin;
                ++event_it;
            }

            // Update the triangle counters - sweeping plane is at plane_pos
            num_tri_plane = plane_on;
            num_tri_right -= plane_on;
            num_tri_right -= plane_end;

            // Compute the SAH cost of splitting the region by this plane
            std::pair<double, KD_Tree::SIDE> sah_result = sah(axis, plane_pos, region,
                num_tri_left, num_tri_right, num_tri_plane);

            // Update the best found plane if needed
            if (sah_result.first < best_cost)
            {
                best_cost = sah_result.first;
                best_side = sah_result.second;
                best_plane = plane_pos;
            }

            // Update the triangle counters - sweeping plane is over plane_pos
            num_tri_plane = 0;
            num_tri_left += plane_begin;
            num_tri_left += plane_on;
        }
    }

    std::pair<AAB, AAB> KD_Tree::split_region(AAB region, int axis, double pos)
    {
        AAB left = region, right = region;

        if (axis == 0)    // Partitioning in the X axis
            left.max_x = right.min_x = pos;
        else if (axis == 1)    // Partitioning in the Y axis
            left.max_y = right.min_y = pos;
        else    // Partitioning in the Z axis
            left.max_z = right.min_z = pos;

        return std::make_pair(left, right);
    }

    bool KD_Tree::on_plane(const Triangle &tri, const Plane &plane)
    {
        return std::abs(plane.evaluate(*tri.vertex(0))) < epsilon && 
            std::abs(plane.evaluate(*tri.vertex(1))) < epsilon &&
            std::abs(plane.evaluate(*tri.vertex(2))) < epsilon;
    }

    bool KD_Tree::has_area(const Triangle& tri, const AAB& region)
    {
        const AAB &clipped_aabb = clipped_triangle_aabb(tri, region);

        if (clipped_aabb.max_x < clipped_aabb.min_x || clipped_aabb.max_y < clipped_aabb.min_y ||
            clipped_aabb.max_z < clipped_aabb.min_z)
        {
            return false;    // Invalid AABB - the triangle is outside the given region
        }

        int nonzero_dims = 0;

        if (clipped_aabb.max_x - clipped_aabb.min_x > epsilon)
            ++nonzero_dims;
        if (clipped_aabb.max_y - clipped_aabb.min_y > epsilon)
            ++nonzero_dims;
        if (clipped_aabb.max_z - clipped_aabb.min_z > epsilon)
            ++nonzero_dims;

        // Clipped triangle has area if its clipped AABB is neither a line nor a point
        return nonzero_dims >= 2;
    }

    bool KD_Tree::terminate(int split_axis, double split_pos, const AAB &region,
        size_t num_triangles_left, size_t num_triangles_right, size_t num_triangles_plane)
    {
        double partitioning_cost = sah(split_axis, split_pos, region, num_triangles_left,
            num_triangles_right, num_triangles_plane).first;

        size_t all_triangles = num_triangles_left + num_triangles_right + num_triangles_plane;

        return partitioning_cost > KD_Tree::TRIANGLE_INTERSECTION_COST * all_triangles;
    }

    // ============================================================================================
}