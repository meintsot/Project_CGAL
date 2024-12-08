#pragma once

#include "triangulation.hpp"

class TriangulationUtils {
public:
    static bool isObtuseTriangle(const Point&, const Point&, const Point&);

    static bool isObtuseTriangle(const Triangle&);

    static int countObtuseTriangles(const CDT&);

    static FT squaredDistance(const Point& p1, const Point& p2);

    static Point quadrilateralCentroid(const Point& A, const Point& B, const Point& C, const Point& D);
    
    static Point computeCentroid(const Point& p1, const Point& p2, const Point& p3);
    
    static int findObtuseAngle(const Point& p1, const Point& p2, const Point& p3);

    static bool is_point_inside_convex_hull(const CDT& cdt, const Point& point);
    
    static FT computeCircumradius(const Triangle& triangle);

    static FT computeHeight(const Triangle& triangle);

    static FT computeRadiusToHeightRatio(const Triangle& triangle);

    static Face_handle getRandomObtuseTriangle(const CDT& cdt);
};
