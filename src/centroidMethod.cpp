#include <iostream>
#include "triangulationUtils.hpp"
#include "centroidMethod.hpp"

CentroidMethod::CentroidMethod() {
    std::cout << "CentroidMethod executed" << std::endl;
}

void CentroidMethod::insertCentroid(CDT& cdt, Face_handle face, std::vector<Point>& steiner_points) {

    if (cdt.is_infinite(face)) return;  // Ignore infinite faces

    for (int i = 0; i < 3; ++i) {
        Face_handle neighbor = face->neighbor(i);
        
        if (cdt.is_infinite(neighbor)) continue;  // Ignore infinite neighbors

        Triangle triangle = cdt.triangle(neighbor);
        auto is_obtuse = TriangulationUtils::isObtuseTriangle(triangle);
        if (is_obtuse) {
            Point A = face->vertex((i + 1) % 3)->point();
            Point B = face->vertex((i + 2) % 3)->point();
            Point C = neighbor->vertex((neighbor->index(face) + 1) % 3)->point();
            Point D = neighbor->vertex((neighbor->index(face) + 2) % 3)->point();

            // Compute centroid of the quadrilateral formed by the two obtuse triangles
            Point centroid = TriangulationUtils::quadrilateralCentroid(A, B, C, D);
            
            // Insert the centroid as a steiner point
            cdt.insert(centroid);
            steiner_points.push_back(centroid);
            break;  // Only the first obtuse neighbor
        }
    }
}

// Function to check if inserting the centroid is beneficial
bool CentroidMethod::isCentroidBeneficial(CDT cdt, Face_handle face) {
    
    if (cdt.is_infinite(face)) return false;  // Cannot be beneficial if the face is infinite

    // Keep track of obtuse triangles before insertion
    int initial_obtuse_count = TriangulationUtils::countObtuseTriangles(cdt);
    bool inserted = false;

    for (int i = 0; i < 3; ++i) {
        Face_handle neighbor = face->neighbor(i);
        
        if (cdt.is_infinite(neighbor)) continue;  // Ignore infinite neighbors

        Triangle triangle = cdt.triangle(neighbor);
        auto is_obtuse = TriangulationUtils::isObtuseTriangle(triangle);
        if (is_obtuse) {
            Point A = face->vertex((i + 1) % 3)->point();
            Point B = face->vertex((i + 2) % 3)->point();
            Point C = neighbor->vertex((neighbor->index(face) + 1) % 3)->point();
            Point D = neighbor->vertex((neighbor->index(face) + 2) % 3)->point();

            // Compute centroid of the quadrilateral formed by the two obtuse triangles
            Point centroid = TriangulationUtils::quadrilateralCentroid(A, B, C, D);
            
            // Insert the centroid into the copy of the triangulation
            cdt.insert(centroid);
            inserted = true;
            break;  // Only the first obtuse neighbor
        }
    }

    if (!inserted) return false;

    // Keep track of obtuse triangles after insertion
    int new_obtuse_count = TriangulationUtils::countObtuseTriangles(cdt);

    // Decide whether the insertion is beneficial
    return new_obtuse_count < initial_obtuse_count;
}

void CentroidMethod::execute(CDT& cdt,Face_handle face , std::vector<Point>& steiner_points) {
    bool done = false;
    int count = 0;

    // while (!done) {
    //     done = true;


    //     for (auto face = cdt.finite_faces_begin(); face != cdt.finite_faces_end(); ++face) {

    //         Triangle triangle = cdt.triangle(face);
    //         auto is_obtuse = TriangulationUtils::isObtuseTriangle(triangle);
    //         if (is_obtuse) {

    //             auto is_centroid_beneficial = this->isCentroidBeneficial(cdt, face);
    //             if (is_centroid_beneficial) {
                     this->insertCentroid(cdt, face, steiner_points);
    //                 done = false;  // Keep iterating since we inserted a new point
    //                 break;  // Rebuild the triangulation and start checking again
    //             }
    //         }
    //     }
    // }
}