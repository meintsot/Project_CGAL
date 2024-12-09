#include <iostream>
#include "projectionMethod.hpp"
#include "triangulationUtils.hpp"

ProjectionMethod::ProjectionMethod() {
}

// Function to insert the projection of an obtuse triangle vertex onto its longest edge
void ProjectionMethod::insertProjection(CDT& cdt, Face_handle face, std::vector<Point>& steiner_points) {
    // Get vertices of the triangle
    Point p1 = face->vertex(0)->point();
    Point p2 = face->vertex(1)->point();
    Point p3 = face->vertex(2)->point();

    // Find the obtuse angle's index
    int obtuse_index = TriangulationUtils::findObtuseAngle(p1, p2, p3);
    if (obtuse_index == -1) return; // No obtuse angle, skip

    //CGAL::Line_2 tline;
    // Determine obtuse vertex and the endpoints of the opposite edge
    Point projection;
    if (obtuse_index == 0) {
        CGAL::Line_2<Kernel> tline(p3, p2);  // Construct the line passing through p2 and p3
        projection = tline.projection(p1);  // Project p1 onto the line p2-p3
    } else if (obtuse_index == 1) {
        CGAL::Line_2<Kernel> tline(p3, p1);
        projection = tline.projection(p2);
    } else {
        CGAL::Line_2<Kernel> tline(p2, p1);
        projection = tline.projection(p3);
    }

    // Insert the projection point and update triangulation
    CDT::Vertex_handle new_vertex = cdt.insert(projection);
    steiner_points.push_back(projection);
}

// Function to determine if the projection is beneficial
bool ProjectionMethod::isProjectionBeneficial(CDT cdt, Face_handle face) {
    // Count the number of obtuse triangles before the projection
    int initial_obtuse_count = TriangulationUtils::countObtuseTriangles(cdt);

    // Perform the projection
    std::vector<Point> dummy_points; // Temporary vector for Steiner points
    insertProjection(cdt, face, dummy_points);

    // Count the number of obtuse triangles after the projection
    int new_obtuse_count = TriangulationUtils::countObtuseTriangles(cdt);

    // Return true if the number of obtuse triangles decreases
    return new_obtuse_count < initial_obtuse_count;
}

// Execute the projection-based refinement
void ProjectionMethod::execute(CDT& cdt, Face_handle face , std::vector<Point>& steiner_points) {
    insertProjection(cdt, face, steiner_points);
}

double ProjectionMethod::antColoniesHeuristic(CDT& cdt, Face_handle face, FT radiusToHeightRatio) {
    auto rho = CGAL::to_double(radiusToHeightRatio);
    auto heuristic = (rho - 1) / rho;

    return heuristic < 0 ? 0 : heuristic;
}
