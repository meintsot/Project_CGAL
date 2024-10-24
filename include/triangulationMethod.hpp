#pragma once

#include "triangulation.hpp"

class TriangulationMethod {
public:
    virtual void execute(CDT& cdt, std::vector<Point>& steiner_points) = 0;
};