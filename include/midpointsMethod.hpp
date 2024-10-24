#pragma once

#include "triangulation.hpp"
#include "triangulationMethod.hpp"

class MidpointsMethod : public TriangulationMethod {
public:
    void execute(CDT& cdt, std::vector<Point>& steiner_points) override;
};