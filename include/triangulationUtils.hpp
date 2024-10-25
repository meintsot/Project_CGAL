#pragma once

#include "triangulation.hpp"

class TriangulationUtils {
public:
    static bool isObtuseTriangle(const Point&, const Point&, const Point&);

    static bool isObtuseTriangle(const Triangle&);

    static int countObtuseTriangles(const CDT&);

    static FT squaredDistance(const Point& p1, const Point& p2);

    static Point quadrilateralCentroid(const Point& A, const Point& B, const Point& C, const Point& D);
};
