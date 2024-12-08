#include "triangulation.hpp"
#include "triangulationUtils.hpp"
#include <CGAL/convex_hull_2.h>
#include <CGAL/Polygon_2.h>



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

// Function to compute the centroid of a triangle
Point TriangulationUtils::computeCentroid(const Point& p1, const Point& p2, const Point& p3) {
    return Point(
        (p1.x() + p2.x() + p3.x()) / 3.0,
        (p1.y() + p2.y() + p3.y()) / 3.0
    );
}

int TriangulationUtils::findObtuseAngle(const Point& p1, const Point& p2, const Point& p3) {
        FT a2 = TriangulationUtils::squaredDistance(p2, p3); // Opposite of p1
        FT b2 = TriangulationUtils::squaredDistance(p1, p3); // Opposite of p2
        FT c2 = TriangulationUtils::squaredDistance(p1, p2); // Opposite of p3

        if (a2 > b2 + c2) return 0; // Angle at p1 is obtuse
        if (b2 > a2 + c2) return 1; // Angle at p2 is obtuse
        if (c2 > a2 + b2) return 2; // Angle at p3 is obtuse
        return -1; // No obtuse angle
    }

bool TriangulationUtils::is_point_inside_convex_hull(const CDT& cdt, const Point& point) {
    std::vector<Point> vertices;
    for (auto vit = cdt.finite_vertices_begin(); vit != cdt.finite_vertices_end(); ++vit) {
        vertices.push_back(vit->point());
    }

    // Compute the convex hull of the vertices
    std::vector<Point> convex_hull;
    CGAL::convex_hull_2(vertices.begin(), vertices.end(), std::back_inserter(convex_hull));

    // We need to convert it to polygon to check
    CGAL::Polygon_2<Kernel> hull_polygon(convex_hull.begin(), convex_hull.end());

    // Check if the point is inside
    return hull_polygon.bounded_side(point) != CGAL::ON_UNBOUNDED_SIDE;
}

// Function to compute the squared distance between two points
FT TriangulationUtils::squaredDistance(const Point& p1, const Point& p2) {
    return CGAL::squared_distance(p1, p2);
}

Point TriangulationUtils::quadrilateralCentroid(const Point& A, const Point& B, const Point& C, const Point& D) {
    return Point((A.x() + B.x() + C.x() + D.x()) / 4, (A.y() + B.y() + C.y() + D.y()) / 4);
}