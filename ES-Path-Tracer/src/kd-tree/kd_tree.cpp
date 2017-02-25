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

    const double KD_Tree::EPSILON = 10e-6;

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
        size_t mid = triangles.size() / 2;
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
                const Point3& v1 = *(*tri_it)->v1;
                const Point3& v2 = *(*tri_it)->v2;
                const Point3& v3 = *(*tri_it)->v3;

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





// ============================================================================
// ============================ NEW IMPLEMENTATION ============================
// ============================================================================

#include <algorithm>
#include <cmath>
#include <climits>
#include <utility>
#include <vector>

static const double epsilon = 10e-6;

class KD_Tree_Build_Event {
public:
    enum Type { END = 0, PLANE = 1, BEGIN = 2 };

    const Triangle* tri;
    const double position;
    const Type type;

    KD_Tree_Build_Event(const Triangle* triangle, double event_position, Type event_type)
        : tri(triangle), position(event_position), type(event_type) {}
};

bool operator<(const KD_Tree_Build_Event& e1, const KD_Tree_Build_Event& e2)
{
    if ( std::abs(e1.position - e2.position) < epsilon && e1.type < e2.type )
        return true;

    if ( e1.position < e2.position )
        return true;

    return false;
}

struct Region { double min_x, max_x, min_y, max_y, min_z, max_z; };

class KD_Node {
public:
    virtual ~KD_Node() = 0;
};

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

class KD_Leaf : public KD_Node {
public:
    std::vector<const Triangle*> triangles;

    KD_Leaf(const std::vector<const Triangle*>& triangles) : triangles(triangles) {}

    ~KD_Leaf() {}
};

class KD_Tree {
public:
    // Dummy values
    static const int TRAVERSAL_COST;
    static const int TRIANGLE_INTERSECTION_COST;
    static const double COST_FUNCTION_BIAS;

    ~KD_Tree();

    KD_Tree( const std::vector<const Triangle*>& triangles ) : 
        root( rec_build_tree(triangles, bounding_box) ), 
        bounding_box( compute_aabb(triangles) ) {}

private:
    const Region bounding_box;
    const KD_Node* const root;
    
    enum SIDE { LEFT, RIGHT };

    KD_Node* rec_build_tree( const std::vector<const Triangle*>& triangles, Region region );
    
    void sweep_plane(std::vector<KD_Tree_Build_Event> &event_queue, int axis, const Region &region, 
        size_t num_triangles, double &best_cost, Plane &best_plane, KD_Tree::SIDE &best_side);

    Region compute_aabb( const std::vector<const Triangle*>& triangles );
    Region compute_aabb( const Triangle& triangle);

    void create_events( const Triangle &tri, const Region &region, 
        std::vector<KD_Tree_Build_Event> &x_event_queue, std::vector<KD_Tree_Build_Event> &y_event_queue,
        std::vector<KD_Tree_Build_Event> &z_event_queue );

    Plane find_plane( const std::vector<const Triangle*>& triangles, Region region );

    std::pair<Region, Region> split_region( Region region, Plane partition_plane );

    bool intersects( const Triangle& tri, const Region& region );

    bool terminate( const std::vector<const Triangle*>& triangles, Region region );
    
    Region clipped_triangle_aabb( const Triangle& triangle, const Region& region );

    double surface_area(const Region& region);

    double cost_bias( size_t num_triangles_left, size_t num_triangles_right );

    double cost( double prob_left, double prob_right, size_t num_triangles_left, 
        size_t num_triangles_right );

    std::pair<double, SIDE> sah( Plane split_plane, Region region, size_t num_triangles_left,
        size_t num_triangles_right, size_t num_triangles_plane );
};

// Dummy values
const int KD_Tree::TRAVERSAL_COST = 1;
const int KD_Tree::TRIANGLE_INTERSECTION_COST = 3;
const double KD_Tree::COST_FUNCTION_BIAS = 0.8;

inline double KD_Tree::cost_bias( size_t num_triangles_left, size_t num_triangles_right)
{
    if (num_triangles_left == 0 || num_triangles_right == 0)
        return 0.8;
    return 1;
}

inline double KD_Tree::cost( double prob_left, double prob_right, size_t num_triangles_left, 
    size_t num_triangles_right )
{
    double estimated_cost = KD_Tree::TRAVERSAL_COST + KD_Tree::TRIANGLE_INTERSECTION_COST * 
        (prob_left * num_triangles_left + prob_right * num_triangles_right);
    return cost_bias(num_triangles_left, num_triangles_right) * estimated_cost;
}

inline double KD_Tree::surface_area(const Region& region)
{
    double delta_x = region.max_x - region.min_x;
    double delta_y = region.max_y - region.min_y;
    double delta_z = region.max_z - region.min_z;

    return delta_x * delta_y * delta_z;
}

std::pair<double, KD_Tree::SIDE> KD_Tree::sah(Plane split_plane, Region region,
    size_t num_triangles_left, size_t num_triangles_right, size_t num_triangles_plane )
{
    const std::pair<Region, Region>& subregions = split_region(region, split_plane);
    const Region& left_subregion = subregions.first;
    const Region& right_subregion = subregions.second;

    const double region_surface_area = surface_area(region);
    const double prob_left = surface_area(left_subregion) / region_surface_area;
    const double prob_right = surface_area(right_subregion) / region_surface_area;

    double cost_left = cost( prob_left, prob_right, num_triangles_left + num_triangles_plane, 
        num_triangles_right );
    double cost_right = cost( prob_left, prob_right, num_triangles_left, 
        num_triangles_right + num_triangles_plane );

    if ( cost_left < cost_right )
        return std::make_pair( cost_left, KD_Tree::SIDE::LEFT );
    else
        return std::make_pair( cost_right, KD_Tree::SIDE::RIGHT );
}

Region KD_Tree::compute_aabb(const std::vector<const Triangle*>& triangles)
{
    Region region;
    region.max_x = region.max_y = region.max_z = INT_MIN;
    region.min_x = region.min_y = region.min_z = INT_MAX;

    // Compute bounding box for triangles
    for (std::vector<const Triangle*>::const_iterator tri_it = triangles.begin();
        tri_it != triangles.end(); ++tri_it)
    {
        // Compute current triangle's AABB
        Region aabb = compute_aabb( **tri_it );
        
        // Expand AABB for the vector
        region.max_x = std::max( region.max_x, aabb.max_x );
        region.max_y = std::max( region.max_y, aabb.max_y );
        region.max_z = std::max( region.max_z, aabb.max_z );
        region.min_x = std::min( region.min_x, aabb.min_x );
        region.min_y = std::min( region.min_y, aabb.min_y );
        region.min_z = std::min( region.min_z, aabb.min_z );
    }
    
    return region;
}

Region KD_Tree::compute_aabb(const Triangle& triangle)
{
    Region region;
    region.max_x = region.max_y = region.max_z = INT_MIN;
    region.min_x = region.min_y = region.min_z = INT_MAX;

    for (Triangle::const_iterator vertex_it = triangle.begin();
        vertex_it != triangle.end(); ++vertex_it)
    {
        // Update max region vertices
        region.max_x = std::max(region.max_x, (*vertex_it)->x);
        region.max_y = std::max(region.max_y, (*vertex_it)->y);
        region.max_z = std::max(region.max_z, (*vertex_it)->z);
        // Update min region vertices
        region.min_x = std::min(region.min_x, (*vertex_it)->x);
        region.min_y = std::min(region.min_y, (*vertex_it)->y);
        region.min_z = std::min(region.min_z, (*vertex_it)->z);
    }
    
    return region;
}

KD_Node* KD_Tree::rec_build_tree(const std::vector<const Triangle*>& triangles, Region region)
{
    if ( terminate(triangles, region) )
        return new KD_Leaf(triangles);

    // Find the best partition plane
    const Plane partition_plane = find_plane(triangles, region);
    
    // Divide the region into left and right subregions
    const std::pair<Region, Region> split_regions = split_region(region, partition_plane);
    const Region& left_subregion = split_regions.first;
    const Region& right_subregion = split_regions.second;

    std::vector<const Triangle*> left_triangles, right_triangles;
    
    // Add the triangles into the left and right sublists
    for (std::vector<const Triangle*>::const_iterator tri_it = triangles.begin();
        tri_it != triangles.end(); ++tri_it)
    {
        const Triangle* tri = *tri_it;

        // Add the triangle to the list(s) it intersects
        if ( intersects(*tri, left_subregion) )
            left_triangles.push_back(tri);
        if ( intersects(*tri, right_subregion) )
            right_triangles.push_back(tri);
    }

    const KD_Node* left_child = rec_build_tree( left_triangles, left_subregion );
    const KD_Node* right_child = rec_build_tree( right_triangles, right_subregion );

    return new KD_Middle_Node( partition_plane, left_child, right_child );
}

Region KD_Tree::clipped_triangle_aabb( const Triangle& triangle, const Region& region )
{
    Region triangle_aabb = compute_aabb(triangle);
    
    triangle_aabb.max_x = std::min(triangle_aabb.max_x, region.max_x);
    triangle_aabb.min_x = std::max(triangle_aabb.min_x, region.min_x);

    triangle_aabb.max_y = std::min(triangle_aabb.max_y, region.max_y);
    triangle_aabb.min_y = std::max(triangle_aabb.min_y, region.min_y);

    triangle_aabb.max_z = std::min(triangle_aabb.max_z, region.max_z);
    triangle_aabb.min_z = std::max(triangle_aabb.min_z, region.min_z);
    
    return triangle_aabb;
}

void KD_Tree::create_events(const Triangle &tri, const Region &region, 
    std::vector<KD_Tree_Build_Event> &x_event_queue, std::vector<KD_Tree_Build_Event> &y_event_queue, 
    std::vector<KD_Tree_Build_Event> &z_event_queue)
{
    Region clipped_tri_aabb = clipped_triangle_aabb( tri, region );

    // ===== X axis events =====
    if (std::abs(clipped_tri_aabb.max_x - clipped_tri_aabb.min_x) < epsilon)
    {
        x_event_queue.push_back( KD_Tree_Build_Event( &tri, clipped_tri_aabb.min_x,
            KD_Tree_Build_Event::Type::PLANE) );
    }
    else
    {
        x_event_queue.push_back( KD_Tree_Build_Event( &tri, clipped_tri_aabb.min_x,
            KD_Tree_Build_Event::Type::BEGIN) );
        x_event_queue.push_back( KD_Tree_Build_Event( &tri, clipped_tri_aabb.max_x,
            KD_Tree_Build_Event::Type::END) );
    }
    // =========================


    // ===== Y axis events =====
    if (std::abs(clipped_tri_aabb.max_y - clipped_tri_aabb.min_y) < epsilon)
    {
        y_event_queue.push_back( KD_Tree_Build_Event( &tri, clipped_tri_aabb.min_y,
            KD_Tree_Build_Event::Type::PLANE) );
    }
    else
    {
        y_event_queue.push_back( KD_Tree_Build_Event( &tri, clipped_tri_aabb.min_y,
            KD_Tree_Build_Event::Type::BEGIN) );
        y_event_queue.push_back( KD_Tree_Build_Event( &tri, clipped_tri_aabb.max_y,
            KD_Tree_Build_Event::Type::END) );
    }
    // =========================


    // ===== Z axis events =====
    if (std::abs(clipped_tri_aabb.max_z - clipped_tri_aabb.min_z) < epsilon)
    {
        z_event_queue.push_back( KD_Tree_Build_Event( &tri, clipped_tri_aabb.min_z,
            KD_Tree_Build_Event::Type::PLANE) );
    }
    else
    {
        z_event_queue.push_back( KD_Tree_Build_Event( &tri, clipped_tri_aabb.min_z,
            KD_Tree_Build_Event::Type::BEGIN) );
        z_event_queue.push_back( KD_Tree_Build_Event( &tri, clipped_tri_aabb.max_z,
            KD_Tree_Build_Event::Type::END) );
    }
    // =========================
}

Plane KD_Tree::find_plane( const std::vector<const Triangle*>& triangles, Region region )
{
    double best_cost = INT_MAX;
    // Dummy initial best plane
    Plane best_plane( Point3(0., 0., 0.), Vector3(0., 0., 1.) );

    std::vector<KD_Tree_Build_Event> x_event_queue, y_event_queue, z_event_queue;
    
    // Create events for all triangles
    for (std::vector<const Triangle*>::const_iterator tri_it = triangles.begin();
        tri_it != triangles.end(); ++tri_it)
    {
        create_events( **tri_it, region, x_event_queue, y_event_queue, z_event_queue );
    }

    // Sort events based on their plane position
    std::sort(x_event_queue.begin(), x_event_queue.end());
    std::sort(y_event_queue.begin(), y_event_queue.end());
    std::sort(z_event_queue.begin(), z_event_queue.end());

}

void KD_Tree::sweep_plane(std::vector<KD_Tree_Build_Event> &event_queue, int axis, const Region &region, 
    size_t num_triangles, double &best_cost, Plane &best_plane, KD_Tree::SIDE &best_side)
{
    best_cost = INT_MAX;
    // Number of triangles in the left, right and contained in the current plane
    size_t num_tri_left = 0, num_tri_right = num_triangles, num_tri_plane = 0;

    for (std::vector<KD_Tree_Build_Event>::const_iterator event_it = event_queue.begin();
        event_it != event_queue.end(); ++event_it)
    {
        double plane_pos = event_it->position;

        size_t plane_begin = 0, plane_end = 0, plane_on = 0;

        // Count the triangles that end in this plane
        while ( event_it != event_queue.end() && event_it->position == plane_pos &&
            event_it->type == KD_Tree_Build_Event::Type::END )
        {
            ++plane_end;
            ++event_it;
        }

        // Count the triangles that are contained in this plane
        while ( event_it != event_queue.end() && event_it->position == plane_pos &&
            event_it->type == KD_Tree_Build_Event::Type::PLANE )
        {
            ++plane_on;
            ++event_it;
        }

        // Count the triangles that begin in this plane
        while ( event_it != event_queue.end() && event_it->position == plane_pos &&
            event_it->type == KD_Tree_Build_Event::Type::BEGIN )
        {
            ++plane_begin;
            ++event_it;
        }

        // Update the triangle counters - sweeping plane is at plane_pos
        num_tri_plane = plane_on;
        num_tri_right -= plane_on;
        num_tri_right -= plane_end;

        // Compute the split plane
        Plane split_plane(Point3(0, 0, 0), Vector3(0, 0, 1));  // Dummy initial value
        switch (axis)
        {
        case 0: { split_plane = Plane(Point3(plane_pos, 0, 0), Vector3(1, 0, 0)); break; }
        case 1: { split_plane = Plane(Point3(0, plane_pos, 0), Vector3(0, 1, 0)); break; }
        case 2: { split_plane = Plane(Point3(0, 0, plane_pos), Vector3(0, 0, 1)); break; }
        }
        
        // Compute the SAH cost of splitting the region by this plane
        std::pair<double, KD_Tree::SIDE> sah_result = sah( split_plane, region, 
            num_tri_left, num_tri_right, num_tri_plane );

        // Update the best found plane if needed
        if (sah_result.first < best_cost)
        {
            best_cost = sah_result.first;
            best_side = sah_result.second;
            best_plane = split_plane;
        }

        // Update the triangle counters - sweeping plane is over plane_pos
        num_tri_plane = 0;
        num_tri_left += plane_begin;
        num_tri_left += plane_on;
    }
}

std::pair<Region, Region> KD_Tree::split_region( Region region, Plane partition_plane )
{

}

bool KD_Tree::intersects( const Triangle& tri, const Region& region )
{

}

bool KD_Tree::terminate( const std::vector<const Triangle*>& triangles, Region region )
{

}