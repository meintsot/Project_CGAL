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
    this->insertoneCentroid(cdt, face, steiner_points);
}

double oneCentroidMethod::antColoniesHeuristic(CDT& cdt, Face_handle face, FT radiusToHeightRatio) {
    return 0;
}
