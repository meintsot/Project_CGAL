#pragma once

#include "triangulation.hpp"

class TriangulationMethod {
protected:
// State management for ant colonies algorithm
    double pheromones;
    double pheromonesDelta;
    double energyDelta;
    CDT antColonyCdt;
    std::vector<Point> antColonySteinerPoints;
public:
    // Getters
    inline double getPheromones() const { return pheromones; }
    inline double getPheromonesDelta() const { return pheromonesDelta; }
    inline double getEnergyDelta() const { return energyDelta; }
    inline const CDT& getAntColonyCdt() const { return antColonyCdt; }
    inline const std::vector<Point>& getAntColonySteinerPoints() const { return antColonySteinerPoints; }

    // Setters
    inline void setPheromones(double value) { pheromones = value; }
    inline void setPheromonesDelta(double value) { pheromonesDelta = value; }
    inline void setEnergyDelta(double value) { energyDelta = value; }
    inline void setAntColonyCdt(const CDT& cdt) { antColonyCdt = cdt; }
    inline void setAntColonySteinerPoints(const std::vector<Point>& points) { antColonySteinerPoints = points; }

    virtual void execute(CDT& cdt, Face_handle face, std::vector<Point>& steiner_points) = 0;
    virtual double antColoniesHeuristic(CDT& cdt, Face_handle face, FT radiusToHeightRatio) = 0;
};