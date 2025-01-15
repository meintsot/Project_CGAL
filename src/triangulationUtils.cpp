#include "triangulation.hpp"
#include "triangulationUtils.hpp"
#include <CGAL/draw_triangulation_2.h>
#include <CGAL/convex_hull_2.h>
#include <CGAL/Polygon_2.h>

#define PI 3.14159265358979323846

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

FT TriangulationUtils::computeCircumradius(const Triangle& triangle) {
    FT a = CGAL::approximate_sqrt(CGAL::squared_distance(triangle[0], triangle[1]));
    FT b = CGAL::approximate_sqrt(CGAL::squared_distance(triangle[1], triangle[2]));
    FT c = CGAL::approximate_sqrt(CGAL::squared_distance(triangle[2], triangle[0]));
    
    FT s = (a + b + c) / 2.0; // Semi-perimeter
    FT area = CGAL::approximate_sqrt(s * (s - a) * (s - b) * (s - c)); // Heron's formula

    return (a * b * c) / (4.0 * area);
}

FT TriangulationUtils::computeHeight(const Triangle& triangle) {
    FT a = CGAL::approximate_sqrt(CGAL::squared_distance(triangle[0], triangle[1]));
    FT b = CGAL::approximate_sqrt(CGAL::squared_distance(triangle[1], triangle[2]));
    FT c = CGAL::approximate_sqrt(CGAL::squared_distance(triangle[2], triangle[0]));
    
    FT area = CGAL::area(triangle[0], triangle[1], triangle[2]);
    FT longest_side = std::max({a, b, c});

    return (2.0 * area) / longest_side;
}

Face_handle TriangulationUtils::getRandomObtuseTriangle(const CDT& cdt) {
    // Collect all obtuse triangles
    std::vector<Face_handle> obtuseTriangles;
    for (auto face = cdt.finite_faces_begin(); face != cdt.finite_faces_end(); ++face) {
        Triangle triangle = cdt.triangle(face);
        if (TriangulationUtils::isObtuseTriangle(triangle)) {
            obtuseTriangles.push_back(face);
        }
    }

    // If no obtuse triangles are found, return an empty triangle (or handle as desired)
    if (obtuseTriangles.empty()) {
        throw std::runtime_error("No obtuse triangles found in the CDT");
    }

    // Choose a random obtuse triangle
    static std::random_device rd;  // Seed
    static std::mt19937 gen(rd()); // Random number generator
    std::uniform_int_distribution<> dis(0, obtuseTriangles.size() - 1);

    return obtuseTriangles[dis(gen)];
}

bool TriangulationUtils::isConvexBoundary(const std::vector<Point>& boundary) {
    Polygon_2 polygon(boundary.begin(), boundary.end());
    return polygon.is_simple() && polygon.is_convex();
}

bool TriangulationUtils::areConstraintsClosed(const std::vector<std::pair<Point, Point>>& constraints) {
    std::unordered_map<Point, int, PointHash> vertex_degree;

    for (const auto& constraint : constraints) {
        vertex_degree[constraint.first]++;
        vertex_degree[constraint.second]++;
    }

    for (const auto& [vertex, degree] : vertex_degree) {
        if (degree != 2) {
            return false;
        }
    }
    return true;
}

bool TriangulationUtils::isAxisParallel(const std::vector<Point>& boundary) {
    for (size_t i = 0; i < boundary.size(); ++i) {
        const auto& p1 = boundary[i];
        const auto& p2 = boundary[(i + 1) % boundary.size()];
        if (p1.x() != p2.x() && p1.y() != p2.y()) {
            return false;
        }
    }
    return true;
}

std::string TriangulationUtils::classifyInput(const std::vector<Point>& boundary, const std::vector<std::pair<Point, Point>>& constraints) {
    bool isConvex = TriangulationUtils::isConvexBoundary(boundary);
    bool isAxisParallel = TriangulationUtils::isAxisParallel(boundary);
    bool hasClosedConstraints = TriangulationUtils::areConstraintsClosed(constraints);

    if (isConvex) {
        if (constraints.empty()) {
            //std::cout << "Convex boundary, no constraints " << std::endl;
            return "ls"; // Convex boundary, no constraints
        } else if (!hasClosedConstraints) {
            //std::cout << "Convex boundary, open constraints " << std::endl;
            return "ls"; // Convex boundary, open constraints
        } else {
            //std::cout << "Convex boundary, closed constraints " << std::endl;
            return "sa"; // Convex boundary, closed constraints
        }
    } else {
        if (isAxisParallel) {
            //std::cout << "Non-convex boundary, axis-parallel segments " << std::endl;
            return "sa"; // Non-convex boundary, axis-parallel segments
        } else {
            //std::cout << "Irregular non-convex boundary " << std::endl;
            return "ant"; // Irregular non-convex boundary
        }
    }
}

std::string TriangulationUtils::getCategory(const std::vector<Point>& boundary, const std::vector<std::pair<Point, Point>>& constraints) {
    bool isConvex = TriangulationUtils::isConvexBoundary(boundary);
    bool isAxisParallel = TriangulationUtils::isAxisParallel(boundary);
    bool hasClosedConstraints = TriangulationUtils::areConstraintsClosed(constraints);

    if (isConvex) {
        if (constraints.empty()) {
            //std::cout << "Convex boundary, no constraints " << std::endl;
            return "Convex_No_Constraints"; // Convex boundary, no constraints
        } else if (!hasClosedConstraints) {
            //std::cout << "Convex boundary, open constraints " << std::endl;
            return "Convex_Open_Constraints"; // Convex boundary, open constraints
        } else {
            //std::cout << "Convex boundary, closed constraints " << std::endl;
            return "Convex_Closed_Constraints"; // Convex boundary, closed constraints
        }
    } else {
        if (isAxisParallel) {
            //td::cout << "Non-convex boundary, axis-parallel segments " << std::endl;
            return "Non_Convex_Parallel"; // Non-convex boundary, axis-parallel segments
        } else {
            //std::cout << "Irregular non-convex boundary " << std::endl;
            return "Non_Convex_Irregular"; // Irregular non-convex boundary
        }
    }
}