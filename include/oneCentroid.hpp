#pragma once

#include "triangulation.hpp"
#include "triangulationMethod.hpp"

class oneCentroidMethod : public TriangulationMethod {
public:
    oneCentroidMethod();
    
    void execute(CDT& cdt, std::vector<Point>& steiner_points) override;

    // Function to insert the oneCentroid of an obtuse triangle into the triangulation
    void insertoneCentroid(CDT&, Face_handle, std::vector<Point>&);

    // Function to insert the oneCentroid of an obtuse triangle into the triangulation
    bool isoneCentroidBeneficial(CDT, Face_handle);
};