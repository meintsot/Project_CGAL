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




// CIRCUMCENTER



// Function to insert the circumcenter of an obtuse triangle into the triangulation
void insert_circumcenter(CDT& cdt, Face_handle face, std::vector<Point>& steiner_points) {

    // Find circumcenter
    Point p1 = face->vertex(0)->point();
    Point p2 = face->vertex(1)->point();
    Point p3 = face->vertex(2)->point();
    Point circumcenter = CGAL::circumcenter(p1, p2, p3);


    // Insert circumcenter and update triangulation
    CDT::Vertex_handle new_vertex = cdt.insert(circumcenter);
    steiner_points.push_back(circumcenter);

}

// Function to insert the circumcenter of an obtuse triangle into the triangulation
int is_circumcenter_beneficial(CDT cdt, Face_handle face) {
    
    // Keep track of obtuses triangles in the start to see if the insertion will reduce the number of obtuse triangles
    int initial_obtuse_count = count_obtuse_triangles(cdt);

    // Find circumcenter
    Point p1 = face->vertex(0)->point();
    Point p2 = face->vertex(1)->point();
    Point p3 = face->vertex(2)->point();
    Point circumcenter = CGAL::circumcenter(p1, p2, p3);


    // Insert circumcenter and update triangulation
    CDT::Vertex_handle new_vertex = cdt.insert(circumcenter);

   


    int new_obtuse_count = count_obtuse_triangles(cdt);

    // Compare counts and decide whether to keep or remove the point
    if (new_obtuse_count < initial_obtuse_count) {
        // Undo the insertion by removing the point
        //cdt.remove(new_vertex);

        
        return 1;
    }

   
    return 0;

}

// Main function to refine the CDT and reduce obtuse triangles using circumcenter insertion
void method_circumcenters(CDT& cdt, std::vector<Point>& steiner_points) {
    bool done = false;
    int count = 0;

    while (!done) {
        done = true; 
        //if (count++ == 10000)
           // break;

        for (auto face = cdt.finite_faces_begin(); face != cdt.finite_faces_end(); ++face) {
            
            Triangle triangle = cdt.triangle(face);
            if (is_obtuse(triangle)) {
                // First check if the insertion would actually reduce the total number of obtuse triangles, and perform it only if it does
                if ( is_circumcenter_beneficial(cdt, face) == 1 ){ 
                    insert_circumcenter(cdt, face, steiner_points);
                    done = false; // Keep iterating since we inserted a new point
                    break; // Rebuild the triangulation and start checking again
                }
            }

        }
        // no more improvement

    }
}







// MIDPOINT


void insert_midpoint(CDT& cdt, Face_handle face, std::vector<Point>& steiner_points) {

    // Find midpoint
    Point p1 = face->vertex(0)->point();
    Point p2 = face->vertex(1)->point();
    Point p3 = face->vertex(2)->point();

    // Find the longest edge
    double d1 = squared_distance(p1, p2);
    double d2 = squared_distance(p2, p3);
    double d3 = squared_distance(p3, p1);

    Point midpoint;
    if (d1 >= d2 && d1 >= d3) {
        // Longest edge is between p1 and p2
        midpoint = CGAL::midpoint(p1, p2);
        cdt.insert(midpoint);
    } else if (d2 >= d1 && d2 >= d3) {
        // Longest edge is between p2 and p3
        midpoint = CGAL::midpoint(p2, p3);
        cdt.insert(midpoint);
    } else {
        // Longest edge is between p3 and p1
        midpoint = CGAL::midpoint(p3, p1);
        cdt.insert(midpoint);
    }
    steiner_points.push_back(midpoint);

}

// Function to insert the circumcenter of an obtuse triangle into the triangulation
int is_midpoint_beneficial(CDT cdt, Face_handle face) {
    
    // Keep track of obtuses triangles in the start to see if the insertion will reduce the number of obtuse triangles
    int initial_obtuse_count = count_obtuse_triangles(cdt);

    // Find midpoint
    Point p1 = face->vertex(0)->point();
    Point p2 = face->vertex(1)->point();
    Point p3 = face->vertex(2)->point();


    // Find the longest edge
    double d1 = squared_distance(p1, p2);
    double d2 = squared_distance(p2, p3);
    double d3 = squared_distance(p3, p1);

    if (d1 >= d2 && d1 >= d3) {
        // Longest edge is between p1 and p2
        Point midpoint = CGAL::midpoint(p1, p2);
        cdt.insert(midpoint);
    } else if (d2 >= d1 && d2 >= d3) {
        // Longest edge is between p2 and p3
        Point midpoint = CGAL::midpoint(p2, p3);
        cdt.insert(midpoint);
    } else {
        // Longest edge is between p3 and p1
        Point midpoint = CGAL::midpoint(p3, p1);
        cdt.insert(midpoint);
    }

    int new_obtuse_count = count_obtuse_triangles(cdt);

    // Compare counts and decide whether to keep or remove the point
    if (new_obtuse_count < initial_obtuse_count) {
        // Undo the insertion by removing the point
        //cdt.remove(new_vertex);

        // obtuse_triangle_count = count_obtuse_triangles(cdt);
        // std::cout << " TELIKA Number of obtuse triangles: " << obtuse_triangle_count << std::endl;
        // sleep(2);

        return 1;
    }

    // obtuse_triangle_count = count_obtuse_triangles(cdt);
    // std::cout << " TELIKA Number of obtuse triangles: " << obtuse_triangle_count << std::endl;
    // sleep(2);

    return 0;

}


void method_midpoints(CDT& cdt, std::vector<Point>& steiner_points) {
    bool done = false;
    int count = 0;

    while (!done) {
        done = true; 
        //if (count++ == 10000)
            //break;

        for (auto face = cdt.finite_faces_begin(); face != cdt.finite_faces_end(); ++face) {
            
            Triangle triangle = cdt.triangle(face);
            if (is_obtuse(triangle)) {
                // First check if the insertion would actually reduce the total number of obtuse triangles, and perform it only if it does
                if ( is_midpoint_beneficial(cdt, face) == 1 ){ 
                    insert_midpoint(cdt, face, steiner_points);
                    done = false; // Keep iterating since we inserted a new point
                    break; // Rebuild the triangulation and start checking again
                }
            }

        }
    }
}


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


    std::vector<Point> steiner_points;


    int obtuse_triangle_count = count_obtuse_triangles(cdt);
    std::cout << "Number of obtuse triangles: " << obtuse_triangle_count << std::endl;

    // Draw the triangulation using CGAL's draw function
    CGAL::draw(cdt);


    method_circumcenters(cdt, steiner_points);
    method_midpoints(cdt, steiner_points);

    obtuse_triangle_count = count_obtuse_triangles(cdt);
    std::cout << "Number of obtuse triangles: " << obtuse_triangle_count << std::endl;

    // Draw the triangulation using CGAL's draw function
    CGAL::draw(cdt);

    std::cout << "Steiner points inserted:\n";
    for (const auto& sp : steiner_points) {
        std::cout << "Point(" << sp.x() << ", " << sp.y() << ")\n";
    }
    
    std::cout << "Number of Steiner points: " << steiner_points.size() << "\n";



    return 0;
}