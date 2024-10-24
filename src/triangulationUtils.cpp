#include "triangulation.hpp"
#include "triangulationUtils.hpp"

bool TriangulationUtils::isObtuseTriangle(const Point& a, const Point& b, const Point& c) {
    // Compute the squared lengths of the sides
    FT ab2 = TriangulationUtils::squaredDistance(a, b);
    FT bc2 = TriangulationUtils::squaredDistance(b, c);
    FT ca2 = TriangulationUtils::squaredDistance(c, a);

    // Use the Law of Cosines to check for obtuse angles
    if (ab2 + bc2 < ca2) return true;
    if (bc2 + ca2 < ab2) return true;
    if (ca2 + ab2 < bc2) return true;

    return false;
}

bool TriangulationUtils::isObtuseTriangle(const Triangle& triangle) {
    Point p1 = triangle[0];
    Point p2 = triangle[1];
    Point p3 = triangle[2];

    return TriangulationUtils::isObtuseTriangle(p1, p2, p3);
}

int TriangulationUtils::countObtuseTriangles(const CDT& cdt) {
    int obtuse_count = 0;
    
    // Iterate over all finite faces (triangles)
    for (auto face = cdt.finite_faces_begin(); face != cdt.finite_faces_end(); ++face) {
        // Get the triangle corresponding to the face
        Triangle triangle = cdt.triangle(face);
        
        // Check if the triangle is obtuse
        auto i_sobtuse = TriangulationUtils::isObtuseTriangle(triangle);
        if (i_sobtuse) {
            ++obtuse_count;
        }
    }
    
    return obtuse_count;
}

// Function to compute the squared distance between two points
FT TriangulationUtils::squaredDistance(const Point& p1, const Point& p2) {
    return CGAL::squared_distance(p1, p2);
}