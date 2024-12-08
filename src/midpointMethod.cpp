#include <iostream>
#include "midpointMethod.hpp"
#include "triangulationUtils.hpp"

MidpointMethod::MidpointMethod() {
    std::cout << "MidpointMethod executed" << std::endl;
}

void MidpointMethod::insertMidpoint(CDT& cdt, Face_handle face, std::vector<Point>& steiner_points) {

    // Find midpoint
    Point p1 = face->vertex(0)->point();
    Point p2 = face->vertex(1)->point();
    Point p3 = face->vertex(2)->point();

    // Find the longest edge
    FT d1 = TriangulationUtils::squaredDistance(p1, p2);
    FT d2 = TriangulationUtils::squaredDistance(p2, p3);
    FT d3 = TriangulationUtils::squaredDistance(p3, p1);

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
bool MidpointMethod::isMidpointBeneficial(CDT cdt, Face_handle face) {
    
    // Keep track of obtuses triangles in the start to see if the insertion will reduce the number of obtuse triangles
    int initial_obtuse_count = TriangulationUtils::countObtuseTriangles(cdt);

    // Find midpoint
    Point p1 = face->vertex(0)->point();
    Point p2 = face->vertex(1)->point();
    Point p3 = face->vertex(2)->point();


    // Find the longest edge
    FT d1 = TriangulationUtils::squaredDistance(p1, p2);
    FT d2 = TriangulationUtils::squaredDistance(p2, p3);
    FT d3 = TriangulationUtils::squaredDistance(p3, p1);

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

    int new_obtuse_count = TriangulationUtils::countObtuseTriangles(cdt);

    // Compare counts and decide whether to keep or remove the point
    return new_obtuse_count < initial_obtuse_count;
}

void MidpointMethod::execute(CDT& cdt,Face_handle face , std::vector<Point>& steiner_points) {
    bool done = false;
    int count = 0;

    // while (!done) {
    //     done = true; 

    //     for (auto face = cdt.finite_faces_begin(); face != cdt.finite_faces_end(); ++face) {
            
    //         Triangle triangle = cdt.triangle(face);
    //         auto is_obtuse = TriangulationUtils::isObtuseTriangle(triangle);
    //         if (is_obtuse) {
    //             // First check if the insertion would actually reduce the total number of obtuse triangles, and perform it only if it does
    //             auto is_midpoint_beneficial = this->isMidpointBeneficial(cdt, face);
    //             if (is_midpoint_beneficial){ 
                     this->insertMidpoint(cdt, face, steiner_points);
    //                 done = false; // Keep iterating since we inserted a new point
    //                 break; // Rebuild the triangulation and start checking again
    //             }
    //         }
    //     }
    // }
}
