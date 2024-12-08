#include <iostream>
#include "oneCentroid.hpp"
#include "triangulationUtils.hpp"

oneCentroidMethod::oneCentroidMethod() {
    std::cout << "oneCentroidMethod executed" << std::endl;
}

// Function to insert the centroid of an obtuse triangle into the triangulation
void oneCentroidMethod::insertoneCentroid(CDT& cdt, Face_handle face, std::vector<Point>& steiner_points) {

    // Find centroid
    Point p1 = face->vertex(0)->point();
    Point p2 = face->vertex(1)->point();
    Point p3 = face->vertex(2)->point();

    Point centroid = TriangulationUtils::computeCentroid(p1, p2, p3);


    // Insert centroid and update triangulation
    CDT::Vertex_handle new_vertex = cdt.insert(centroid);
    steiner_points.push_back(centroid);

}

// Function to insert the centroid of an obtuse triangle into the triangulation
bool oneCentroidMethod::isoneCentroidBeneficial(CDT cdt, Face_handle face) {
    
    // Keep track of obtuses triangles in the start to see if the insertion will reduce the number of obtuse triangles
    int initial_obtuse_count = TriangulationUtils::countObtuseTriangles(cdt);

    // Find centroid
    Point p1 = face->vertex(0)->point();
    Point p2 = face->vertex(1)->point();
    Point p3 = face->vertex(2)->point();
    Point centroid = TriangulationUtils::computeCentroid(p1, p2, p3);


    // Insert centroid and update triangulation
    CDT::Vertex_handle new_vertex = cdt.insert(centroid);

    int new_obtuse_count = TriangulationUtils::countObtuseTriangles(cdt);

    // Compare counts and decide whether to keep or remove the point
    return new_obtuse_count < initial_obtuse_count;
}

void oneCentroidMethod::execute(CDT& cdt,Face_handle face , std::vector<Point>& steiner_points) {
    bool done = false;
    int count = 0;

    // while (!done) {
    //     done = true; 

    //     for (auto face = cdt.finite_faces_begin(); face != cdt.finite_faces_end(); ++face) {
            
    //         Triangle triangle = cdt.triangle(face);
    //         auto is_obtuse = TriangulationUtils::isObtuseTriangle(triangle);
    //         if (is_obtuse) {
    //             // First check if the insertion would actually reduce the total number of obtuse triangles, and perform it only if it does
    //             if (this->isoneCentroidBeneficial(cdt, face) == 1){ 
                     this->insertoneCentroid(cdt, face, steiner_points);
    //                 done = false; // Keep iterating since we inserted a new point
    //                 break; // Rebuild the triangulation and start checking again
    //             }
    //         }

    //     }
    // }
}
