#include <iostream>
#include "circumCenterMethod.hpp"
#include "triangulationUtils.hpp"

CircumCenterMethod::CircumCenterMethod() {
    std::cout << "CircumCenterMethod executed" << std::endl;
}

// Function to insert the circumcenter of an obtuse triangle into the triangulation
void CircumCenterMethod::insertCircumcenter(CDT& cdt, Face_handle face, std::vector<Point>& steiner_points) {

    // Find circumcenter
    Point p1 = face->vertex(0)->point();
    Point p2 = face->vertex(1)->point();
    Point p3 = face->vertex(2)->point();
    Point circumcenter = CGAL::circumcenter(p1, p2, p3);


    if (  TriangulationUtils::is_point_inside_convex_hull(cdt, circumcenter)  ){
        // Insert circumcenter and update triangulation
        CDT::Vertex_handle new_vertex = cdt.insert(circumcenter);
        steiner_points.push_back(circumcenter);
    }

}

// Function to insert the circumcenter of an obtuse triangle into the triangulation
bool CircumCenterMethod::isCircumcenterBeneficial(CDT cdt, Face_handle face) {
    
    // Keep track of obtuses triangles in the start to see if the insertion will reduce the number of obtuse triangles
    int initial_obtuse_count = TriangulationUtils::countObtuseTriangles(cdt);

    // Find circumcenter
    Point p1 = face->vertex(0)->point();
    Point p2 = face->vertex(1)->point();
    Point p3 = face->vertex(2)->point();
    Point circumcenter = CGAL::circumcenter(p1, p2, p3);


    // Insert circumcenter and update triangulation
    CDT::Vertex_handle new_vertex = cdt.insert(circumcenter);

    int new_obtuse_count = TriangulationUtils::countObtuseTriangles(cdt);

    // Compare counts and decide whether to keep or remove the point
    return new_obtuse_count < initial_obtuse_count;
}

void CircumCenterMethod::execute(CDT& cdt,Face_handle face , std::vector<Point>& steiner_points) {
    this->insertCircumcenter(cdt, face, steiner_points);
}

double CircumCenterMethod::antColoniesHeuristic(CDT& cdt, Face_handle face, FT radiusToHeightRatio) {
    auto rho = CGAL::to_double(radiusToHeightRatio);
    return (2 + rho) / rho;
}
