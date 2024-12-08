#pragma once

#include "triangulation.hpp"
#include "triangulationMethod.hpp"

class CircumCenterMethod : public TriangulationMethod {
public:
    CircumCenterMethod();
    
    void execute(CDT& cdt,Face_handle face , std::vector<Point>& steiner_points) override;

    double antColoniesHeuristic(CDT& cdt, Face_handle face, FT radiusToHeightRatio) override;

    // Function to insert the circumcenter of an obtuse triangle into the triangulation
    void insertCircumcenter(CDT&, Face_handle, std::vector<Point>&);

    // Function to insert the circumcenter of an obtuse triangle into the triangulation
    bool isCircumcenterBeneficial(CDT, Face_handle);
};