#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/draw_triangulation_2.h>
#include <vector>
#include <cmath>


typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Constrained_Delaunay_triangulation_2<K> CDT;
typedef CDT::Point Point;
typedef K::Triangle_2 Triangle;
typedef CDT::Face_handle Face_handle;
// Define a pair of indices representing an edge
typedef std::pair<CDT::Vertex_handle, CDT::Vertex_handle> Edge;


bool is_obtuse(const Triangle& triangle) {
    Point p1 = triangle[0];
    Point p2 = triangle[1];
    Point p3 = triangle[2];

    double a2 = CGAL::squared_distance(p2, p3);
    double b2 = CGAL::squared_distance(p1, p3);
    double c2 = CGAL::squared_distance(p1, p2);

    return (a2 + b2 < c2) || (b2 + c2 < a2) || (a2 + c2 < b2);
}

int count_obtuse_triangles(const CDT& cdt) {
    int obtuse_count = 0;
    
    // Iterate over all finite faces (triangles)
    for (auto face = cdt.finite_faces_begin(); face != cdt.finite_faces_end(); ++face) {
        // Get the triangle corresponding to the face
        Triangle triangle = cdt.triangle(face);
        
        // Check if the triangle is obtuse
        if (is_obtuse(triangle)) {
            ++obtuse_count;
        }
    }
    
    return obtuse_count;
}



//final

// Function to compute the squared distance between two points
double squared_distance(const Point& p1, const Point& p2) {
    return CGAL::squared_distance(p1, p2);
}


// Function to insert the circumcenter of an obtuse triangle into the triangulation
void insert_circumcenter(CDT& cdt, Face_handle face) {
    // Retrieve the vertices of the triangle
    Point p1 = face->vertex(0)->point();
    Point p2 = face->vertex(1)->point();
    Point p3 = face->vertex(2)->point();

    // Calculate the circumcenter of the triangle
    Point circumcenter = CGAL::circumcenter(p1, p2, p3);

    // Insert the circumcenter into the triangulation
    cdt.insert(circumcenter);
}





// Function to insert the circumcenter of an obtuse triangle into the triangulation
int trully_insert_circumcenter_if_beneficial(CDT& cdt, Face_handle face) {
    // Step 1: Count obtuse triangles before insertion
    int initial_obtuse_count = count_obtuse_triangles(cdt);

    // Step 2: Compute circumcenter
    Point p1 = face->vertex(0)->point();
    Point p2 = face->vertex(1)->point();
    Point p3 = face->vertex(2)->point();
    Point circumcenter = CGAL::circumcenter(p1, p2, p3);

    // int obtuse_triangle_count = count_obtuse_triangles(cdt);
    // std::cout << " PRIN Number of obtuse triangles: " << obtuse_triangle_count << std::endl;


    // Step 3: Insert circumcenter and update triangulation
    CDT::Vertex_handle new_vertex = cdt.insert(circumcenter);

    // obtuse_triangle_count = count_obtuse_triangles(cdt);
    // std::cout << " META Number of obtuse triangles: " << obtuse_triangle_count << std::endl;


    // Step 4: Count obtuse triangles after insertion
    int new_obtuse_count = count_obtuse_triangles(cdt);

    // Step 5: Compare counts and decide whether to keep or remove the point
    if (new_obtuse_count > initial_obtuse_count) {
        // Undo the insertion by removing the point
        cdt.remove(new_vertex);

        // obtuse_triangle_count = count_obtuse_triangles(cdt);
        // std::cout << " TELIKA Number of obtuse triangles: " << obtuse_triangle_count << std::endl;
        // sleep(2);

        return 0;
    }

    // obtuse_triangle_count = count_obtuse_triangles(cdt);
    // std::cout << " TELIKA Number of obtuse triangles: " << obtuse_triangle_count << std::endl;
    // sleep(2);

    return 1;

}




// Function to insert the circumcenter of an obtuse triangle into the triangulation
int insert_circumcenter_if_beneficial(CDT cdt, Face_handle face) {
    // Step 1: Count obtuse triangles before insertion
    int initial_obtuse_count = count_obtuse_triangles(cdt);

    // Step 2: Compute circumcenter
    Point p1 = face->vertex(0)->point();
    Point p2 = face->vertex(1)->point();
    Point p3 = face->vertex(2)->point();
    Point circumcenter = CGAL::circumcenter(p1, p2, p3);

    // int obtuse_triangle_count = count_obtuse_triangles(cdt);
    // std::cout << " PRIN Number of obtuse triangles: " << obtuse_triangle_count << std::endl;


    // Step 3: Insert circumcenter and update triangulation
    CDT::Vertex_handle new_vertex = cdt.insert(circumcenter);

    // obtuse_triangle_count = count_obtuse_triangles(cdt);
    // std::cout << " META Number of obtuse triangles: " << obtuse_triangle_count << std::endl;


    // Step 4: Count obtuse triangles after insertion
    int new_obtuse_count = count_obtuse_triangles(cdt);

    // Step 5: Compare counts and decide whether to keep or remove the point
    if (new_obtuse_count > initial_obtuse_count) {
        // Undo the insertion by removing the point
        cdt.remove(new_vertex);

        // obtuse_triangle_count = count_obtuse_triangles(cdt);
        // std::cout << " TELIKA Number of obtuse triangles: " << obtuse_triangle_count << std::endl;
        // sleep(2);

        return 0;
    }

    // obtuse_triangle_count = count_obtuse_triangles(cdt);
    // std::cout << " TELIKA Number of obtuse triangles: " << obtuse_triangle_count << std::endl;
    // sleep(2);

    return 1;

}

// Main function to refine the CDT and reduce obtuse triangles using circumcenter insertion
void refine_obtuse_triangles_with_circumcenters(CDT& cdt) {
    bool done = false;
    int count = 0;

    while (!done) {
        done = true; // Assume we're done unless we find an obtuse triangle
        if (count++ == 100000000)
            break;

        for (auto face = cdt.finite_faces_begin(); face != cdt.finite_faces_end(); ++face) {
            
            Triangle triangle = cdt.triangle(face);
            if (is_obtuse(triangle)) {
                // Insert the circumcenter and re-triangulate
                if ( insert_circumcenter_if_beneficial(cdt, face) == 1 ){ // New point inserted
                    trully_insert_circumcenter_if_beneficial(cdt, face);
                    done = false; // Keep iterating since we inserted a new point
                    break; // Rebuild the triangulation and start checking again
                }
            }

        }
    }
}

// void refine_obtuse_triangles_with_circumcenters2(CDT& cdt) {
//     bool done = false;
//     int count = 0;


//     for (auto face = cdt.finite_faces_begin(); face != cdt.finite_faces_end(); ++face) {
            
//             Triangle triangle = cdt.triangle(face);
//             if (is_obtuse(triangle)) {
//                 // Insert the circumcenter and re-triangulate
//                 insert_circumcenter(cdt, face);
//                 break;
//             }

//         }
    
// }

//final



int main() 
{
    // Initialize the Constrained Delaunay Triangulation (CDT)
    CDT cdt;

    // Define the points from the PSLG (x, y coordinates)
    std::vector<Point> points = {
        Point(632, 1588), Point(1330, 1097), Point(3051, 470), Point(5040, 1077),
        Point(5883, 2766), Point(8130, 3629), Point(9280, 2836), Point(9613, 4963),
        Point(9422, 6363), Point(8996, 7327), Point(8020, 7611), Point(8467, 9720),
        Point(6735, 9183), Point(4674, 7865), Point(2519, 7692), Point(973, 9797),
        Point(1205, 6005), Point(1929, 5812), Point(3203, 6301), Point(5345, 2923)
    };

    // Insert points into the triangulation
    for (const Point& p : points) {
        cdt.insert(p);
    }

    // Define and add the constrained edges (from additional constraints)
    std::vector<std::pair<int, int>> constraints = {
        {3, 4}, {5, 6}, {9, 10}, {10, 11}, {11, 12}, {12, 13}, {13, 14},
        {14, 15}, {15, 16}, {18, 19}, {19, 0}
    };

    // Insert constrained edges based on the provided indices
    for (const auto& constraint : constraints) {
        cdt.insert_constraint(points[constraint.first], points[constraint.second]);
    }

    int obtuse_triangle_count = count_obtuse_triangles(cdt);
    std::cout << "Number of obtuse triangles: " << obtuse_triangle_count << std::endl;

    // Draw the triangulation using CGAL's draw function
    CGAL::draw(cdt);

    refine_obtuse_triangles_with_circumcenters(cdt);

    obtuse_triangle_count = count_obtuse_triangles(cdt);
    std::cout << "Number of obtuse triangles: " << obtuse_triangle_count << std::endl;

    // Draw the triangulation using CGAL's draw function
    CGAL::draw(cdt);

    return 0;
}
