#include <iostream>
#include "projectionMethod.hpp"
#include "triangulationUtils.hpp"

ProjectionMethod::ProjectionMethod() {
    std::cout << "ProjectionMethod executed" << std::endl;
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
    bool done = false;
    int counter = 0;
    // while (!done) {
    //     done = true; // Assume no more insertions
    //     // std::cout << "counter = " << counter << "\n";
    //     // if (counter++ == 4)
    //     //     break;
    //     for (auto face = cdt.finite_faces_begin(); face != cdt.finite_faces_end(); ++face) {
    //         Triangle triangle = cdt.triangle(face);
    //         if (TriangulationUtils::isObtuseTriangle(triangle)) {
    //             // Check if the projection reduces obtuse triangles
    //             if (isProjectionBeneficial(cdt, face)) {
                     insertProjection(cdt, face, steiner_points);
    //                 done = false; // Continue iterating after an insertion
    //                 break; // Restart iteration due to triangulation update
    //             }
    //         }
    //     }
    // }
}
