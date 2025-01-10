#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <cstdlib>
#include <ctime>


#include "triangulation.hpp"
#include "jsonUtils.hpp"
#include "triangulationUtils.hpp"
#include "triangulationMethod.hpp"
#include "circumCenterMethod.hpp"
#include "midpointMethod.hpp"
#include "centroidMethod.hpp"
#include "oneCentroid.hpp"
#include "projectionMethod.hpp"

// LOCAL SEARCH

int find_best_method(CDT cdt, Face_handle face){

    std::vector<Point> dummy_points; // Temporary vector for Steiner points
    int initial_obtuse_count = TriangulationUtils::countObtuseTriangles(cdt);

    CDT starting_cdt = cdt;
    CDT test_cdt;

    int best_method = 6; // Default to 6 if none improves
    int best_obtuse_count = initial_obtuse_count;

    ProjectionMethod* method1;
    method1 = new ProjectionMethod();
    method1->insertProjection(cdt, face, dummy_points);
    delete method1;
    int projection_obtuse_count = TriangulationUtils::countObtuseTriangles(cdt);
    cdt = starting_cdt;
    if (projection_obtuse_count < best_obtuse_count) {
        best_obtuse_count = projection_obtuse_count;
        best_method = 1;
    }

    MidpointMethod* method2;
    method2 = new MidpointMethod();
    method2->insertMidpoint(cdt, face, dummy_points);
    delete method2;
    int midpoint_obtuse_count = TriangulationUtils::countObtuseTriangles(cdt);
    cdt = starting_cdt;
    if (midpoint_obtuse_count < best_obtuse_count) {
        best_obtuse_count = midpoint_obtuse_count;
        best_method = 2;
    }

    CentroidMethod* method3;
    method3 = new CentroidMethod();
    method3->insertCentroid(cdt, face, dummy_points);
    delete method3;
    int centroid_obtuse_count = TriangulationUtils::countObtuseTriangles(cdt);
    cdt = starting_cdt;
    if (centroid_obtuse_count < best_obtuse_count) {
        best_obtuse_count = centroid_obtuse_count;
        best_method = 3;
    }

    oneCentroidMethod* method4;
    method4 = new oneCentroidMethod();
    method4->insertoneCentroid(cdt, face, dummy_points);
    delete method4;
    int onecentroid_obtuse_count = TriangulationUtils::countObtuseTriangles(cdt);
    cdt = starting_cdt;
    if (onecentroid_obtuse_count < best_obtuse_count) {
        best_obtuse_count = onecentroid_obtuse_count;
        best_method = 4;
    }

    CircumCenterMethod* method5;
    method5 = new CircumCenterMethod();
    method5->insertCircumcenter(cdt, face, dummy_points);
    delete method5;
    int circumcenter_obtuse_count = TriangulationUtils::countObtuseTriangles(cdt);
    cdt = starting_cdt;
    if (circumcenter_obtuse_count < best_obtuse_count) {
        best_obtuse_count = circumcenter_obtuse_count;
        best_method = 5;
    }

    return best_method;

}

void local_search(CDT& cdt, std::vector<Point>& steinerPoints, int L) {
    TriangulationMethod* method = nullptr;
    bool done = false;
    int stopping_criterion = 0;

    double p_sum = 0.0; // Sum for p(n)
    int obtuse_previous = TriangulationUtils::countObtuseTriangles(cdt); // Initial obtuse triangle count
    bool randomized = false;

    while (!done) {
        done = true; 
        if (stopping_criterion++ == L) break;

        for (auto face = cdt.finite_faces_begin(); face != cdt.finite_faces_end(); ++face) {
            Triangle triangle = cdt.triangle(face);
            auto is_obtuse = TriangulationUtils::isObtuseTriangle(triangle);
            if (is_obtuse) {
                int best_method = find_best_method(cdt, face);
                switch (best_method) {
                    case 1: method = new ProjectionMethod(); break;
                    case 2: method = new MidpointMethod(); break;
                    case 3: method = new CentroidMethod(); break;
                    case 4: method = new oneCentroidMethod(); break;
                    case 5: method = new CircumCenterMethod(); break;
                    case 6: method = nullptr; break; // No improvement possible
                }

                if (method != nullptr) {
                    method->execute(cdt, face, steinerPoints);
                    delete method;
                    done = false; // Continue iterating
                    break;
                }
            }
        }

        if (done) { // If no improvement was possible, apply randomization
            Point randomPoint = generateRandomPoint(cdt);
            steinerPoints.push_back(randomPoint);
            cdt.insert(randomPoint);
            randomized = true;
            done = false; // Rebuild triangulation and continue
        }

        int obtuse_current = TriangulationUtils::countObtuseTriangles(cdt);
        if (obtuse_previous > 0 && obtuse_current > 0) {
            double p_n = std::log(static_cast<double>(obtuse_current) / obtuse_previous) /
                         std::log(static_cast<double>(stopping_criterion + 1) / stopping_criterion);
            p_sum += p_n;
        }
        obtuse_previous = obtuse_current; // Update for next iteration
    }

    double average_p = p_sum / (stopping_criterion - 1);
    std::cout << "Local Search Average Convergence Rate (p): " << average_p << std::endl;
    if (randomized) {
        std::cout << "Randomization was applied in Local Search." << std::endl;
    }
}


// SA

// Function to calculate the energy of a triangulation
double calculateEnergy(const CDT& cdt, double a, double b, const std::vector<Point> steinerPoints) {
    int obtuseCount = TriangulationUtils::countObtuseTriangles(cdt);
    int steinerCount = steinerPoints.size();
    return a * obtuseCount + b * steinerCount;
}

double randomProbability() {
    return static_cast<double>(std::rand()) / RAND_MAX;
}

void simulated_annealing(CDT& cdt, std::vector<Point>& steinerPoints, double a, double b, int L) {
    TriangulationMethod* method = nullptr;
    std::vector<Point> dummy_points;
    double energy = calculateEnergy(cdt, a, b, steinerPoints); // Initial energy
    double T = 1.0;
    int counter = 0;
    bool randomized = false;
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    double p_sum = 0.0; // Sum for p(n)
    int obtuse_previous = TriangulationUtils::countObtuseTriangles(cdt);

    while (T > 0) {
        counter++;
        bool improved = false;

        for (auto face = cdt.finite_faces_begin(); face != cdt.finite_faces_end(); ++face) {
            Triangle triangle = cdt.triangle(face);
            auto is_obtuse = TriangulationUtils::isObtuseTriangle(triangle);
            if (is_obtuse) {
                int method_option = std::rand() % 5;
                switch (method_option) {
                    case 0: method = new ProjectionMethod(); break;
                    case 1: method = new MidpointMethod(); break;
                    case 2: method = new CentroidMethod(); break;
                    case 3: method = new CircumCenterMethod(); break;
                    case 4: method = new oneCentroidMethod(); break;
                }

                CDT test_cdt = cdt;
                dummy_points = steinerPoints;
                method->execute(test_cdt, face, dummy_points);

                double newEnergy = calculateEnergy(test_cdt, a, b, dummy_points);
                double DE = newEnergy - energy;

                if (DE < 0 || std::exp(-DE / T) >= randomProbability()) {
                    cdt = test_cdt;
                    steinerPoints = dummy_points;
                    energy = newEnergy;
                    improved = true;

                    int obtuse_current = TriangulationUtils::countObtuseTriangles(cdt);
                    if (obtuse_previous > 0 && obtuse_current > 0) {
                        double p_n = std::log(static_cast<double>(obtuse_current) / obtuse_previous) /
                                     std::log(static_cast<double>(counter + 1) / counter);
                        p_sum += p_n;
                    }
                    obtuse_previous = obtuse_current;
                    break;
                }
            }
        }

        if (!improved) { // Apply randomization if no improvement
            Point randomPoint = generateRandomPoint(cdt);
            steinerPoints.push_back(randomPoint);
            cdt.insert(randomPoint);
            randomized = true;
        }

        T -= 1.0 / L;
    }

    double average_p = p_sum / (counter - 1);
    std::cout << "Simulated Annealing Average Convergence Rate (p): " << average_p << std::endl;
    if (randomized) {
        std::cout << "Randomization was applied in Simulated Annealing." << std::endl;
    }
}


// Ant Colonies
void evaluate_method(TriangulationMethod* method, double a, double b, int obtuseCountOld, int obtuseCountNew, std::vector<Point>& steinerPoints, double previousEnergy, CDT& newCdt) {
    double energyDelta = calculateEnergy(newCdt, a, b, steinerPoints) - previousEnergy;
    if (energyDelta < 0)
    {
        int steinerCount = steinerPoints.size();
        double pheromonesDelta = obtuseCountOld == obtuseCountNew ? 0 : 1 / (1 + a * obtuseCountNew + b * steinerCount);
        method->setAntColonyCdt(newCdt);
        method->setAntColonySteinerPoints(steinerPoints);
        method->setPheromonesDelta(pheromonesDelta);
        method->setEnergyDelta(energyDelta);
    }
}

void update_pheromones(TriangulationMethod* method, double pheromonesEvaporation) {
    double pheromones = method->getPheromones();
    double pheromonesDelta = method->getPheromonesDelta();
    pheromones = (1 - pheromonesEvaporation) * pheromones + pheromonesDelta;
    method->setPheromones(pheromones);
}

void ant_colonies(CDT& cdt, std::vector<Point>& steinerPoints, double a, double b, double x, double y, double lambda, double kappa, int L) {
    int K = kappa;
    bool randomized = false;
    double p_sum = 0.0; // Sum for p(n)
    int obtuse_previous = TriangulationUtils::countObtuseTriangles(cdt);

    for (int c = 0; c < L; c++) { // For each cycle
        bool improved = false;

        for (int ant = 0; ant < K; ant++) {
            auto obtuseTriangle = TriangulationUtils::getRandomObtuseTriangle(cdt);
            if (!obtuseTriangle) continue;

            TriangulationMethod* selectedMethod = ...; // Select method based on probabilities
            CDT newCdt = cdt;
            std::vector<Point> newSteinerPoints = steinerPoints;
            selectedMethod->execute(newCdt, obtuseTriangle, newSteinerPoints);

            int obtuse_current = TriangulationUtils::countObtuseTriangles(newCdt);
            if (obtuse_current < obtuse_previous) {
                cdt = newCdt;
                steinerPoints = newSteinerPoints;
                obtuse_previous = obtuse_current;
                improved = true;

                double p_n = ...; // Calculate p(n)
                p_sum += p_n;
                break;
            }
        }

        if (!improved) { // Apply randomization if no improvement
            Point randomPoint = generateRandomPoint(cdt);
            steinerPoints.push_back(randomPoint);
            cdt.insert(randomPoint);
            randomized = true;
        }
    }

    double average_p = p_sum / (L - 1);
    std::cout << "Ant Colony Optimization Average Convergence Rate (p): " << average_p << std::endl;
    if (randomized) {
        std::cout << "Randomization was applied in Ant Colony Optimization." << std::endl;
    }
}
