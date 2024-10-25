#pragma once

#include "triangulation.hpp"
#include "triangulationMethod.hpp"

class CentroidMethod : public TriangulationMethod {
public:
    CentroidMethod();

    void execute(CDT& cdt, std::vector<Point>& steiner_points) override;

    void insertCentroid(CDT& cdt, Face_handle face, std::vector<Point>& steiner_points);

    bool isCentroidBeneficial(CDT cdt, Face_handle face);
};