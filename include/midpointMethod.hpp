#pragma once

#include "triangulation.hpp"
#include "triangulationMethod.hpp"

class MidpointMethod : public TriangulationMethod {
public:
    MidpointMethod();

    void execute(CDT& cdt,Face_handle face , std::vector<Point>& steiner_points) override;

    void insertMidpoint(CDT& cdt, Face_handle face, std::vector<Point>& steiner_points);

    bool isMidpointBeneficial(CDT cdt, Face_handle face);
};