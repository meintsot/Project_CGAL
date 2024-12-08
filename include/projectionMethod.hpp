#pragma once

#include "triangulation.hpp"
#include "triangulationMethod.hpp"


class ProjectionMethod : public TriangulationMethod {
public:
    ProjectionMethod();

    void execute(CDT& cdt, Face_handle face , std::vector<Point>& steiner_points) override;

    double antColoniesHeuristic(CDT& cdt, Face_handle face, FT radiusToHeightRatio) override;

    void insertProjection(CDT& cdt, Face_handle face, std::vector<Point>& steiner_points);

    bool isProjectionBeneficial(CDT cdt, Face_handle face);
};