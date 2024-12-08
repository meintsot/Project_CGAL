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

    std::cout << "BEST METHOD = " << best_method << "\n";

    return best_method;

}

void local_search( CDT& cdt, std::vector<Point>& steinerPoints){

    TriangulationMethod* method = nullptr;
    bool done = false;
    int count = 0;
    int best_method;
    int stopping_criterion; // parameter

    while(!done){
        done = true; 
        if ( stopping_criterion++ == 5000 ) break;

        for (auto face = cdt.finite_faces_begin(); face != cdt.finite_faces_end(); ++face) {
            Triangle triangle = cdt.triangle(face);
            auto is_obtuse = TriangulationUtils::isObtuseTriangle(triangle);
            if (is_obtuse) {
                best_method = find_best_method(cdt, face);
                switch (best_method)
                {
                    case 1:
                        method = new ProjectionMethod();
                        break;
                    case 2:
                        method = new MidpointMethod();
                        break;
                    case 3:
                        method = new CentroidMethod();
                        break;
                    case 4:
                        method = new oneCentroidMethod();
                        break;
                    case 5:
                        method = new CircumCenterMethod();
                        break;
                    case 6:
                        method = nullptr; // can't improve more
                        break;
                    default:
                        break;
                }
                // found best triangulation
                // now perform

                if ( method != nullptr ){
                    method->execute(cdt, face, steinerPoints);
                    delete method;
                    done = false; // Keep iterating since we inserted a new point
                    break; // Rebuild the triangulation and start checking again
                }
            }
        }
        // if we reached here, it means that couldn't reduce the obtuse triangles no matter the method        
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

void simulated_annealing(CDT& cdt, std::vector<Point>& steinerPoints){

    TriangulationMethod* method = nullptr;
    std::vector<Point> dummy_points;
    double a,b;
    a = 3.0;
    b = 0.8;
    int L = 500;
    double energy = calculateEnergy(cdt, a, b, steinerPoints); // Initial energy
    double T = 1.0;
    int method_option;
    int counter = 0;
    std::srand(static_cast<unsigned>(std::time(nullptr)));
          
    while ( T > 0 ){
        counter++;

        for (auto face = cdt.finite_faces_begin(); face != cdt.finite_faces_end(); ++face) {
            Triangle triangle = cdt.triangle(face);
            auto is_obtuse = TriangulationUtils::isObtuseTriangle(triangle);
            if (is_obtuse) {
                method_option = std::rand() % 5;
                //std::cout << "option = " << method_option << "\n";
                switch (method_option)
                {
                    case 0: method = new ProjectionMethod(); break;
                    case 1: method = new MidpointMethod(); break;
                    case 2: method = new CentroidMethod(); break;
                    case 3: method = new oneCentroidMethod(); break;
                    case 4: method = new CircumCenterMethod(); break;
                }

                CDT test_cdt = cdt;
                dummy_points = steinerPoints;
                method->execute(test_cdt, face, dummy_points);

                double newEnergy = calculateEnergy(test_cdt, a, b, dummy_points);
                double DE = newEnergy - energy;

                if ( DE < 0 || std::exp( -DE / T ) >= randomProbability() ){
                    method->execute(cdt, face, steinerPoints); // update cdt and steiner points
                    energy = newEnergy;
                    delete method;
                    break;

                }
            }
        }

        T -= 1.0 / L;
        //T
    std::cout << "fores = " << counter << "\n";
    }

}









void perform_triangulation(int methodType, const InputData& input_data, OutputData& output_data) {
    CDT cdt;

    std::vector<Point> points;
    std::vector<Point> steinerPoints;
    std::map<Point, int> point_indices;
    std::vector<Vertex_handle> vertex_handles;

    // Insert initial points
    for (int i = 0; i < input_data.points_x.size(); ++i) {
        Point p(input_data.points_x[i], input_data.points_y[i]);
        Vertex_handle vh = cdt.insert(p);
        points.push_back(p);
        vertex_handles.push_back(vh);
        point_indices[p] = static_cast<int>(i);
    }

    // Insert region boundary constraints
    // int rb_size = input_data.region_boundary.size();
    //     for (int i = 0; i < rb_size; ++i) {
    //      int idx1 = input_data.region_boundary[i];
    //        int idx2 = input_data.region_boundary[(i + 1) % rb_size];
    //          cdt.insert_constraint(vertex_handles[idx1], vertex_handles[idx2]);
    //        }

    // Insert additional constraints
    for (const auto& constraint : input_data.additional_constraints) {
        int idx1 = constraint[0];
        int idx2 = constraint[1];
        cdt.insert_constraint(vertex_handles[idx1], vertex_handles[idx2]);
    }

    // before
    CGAL::draw(cdt);
    int obtuse_triangle_count = TriangulationUtils::countObtuseTriangles(cdt);
    std::cout << "Number of obtuse triangles: " << obtuse_triangle_count << std::endl;

    TriangulationMethod* method = nullptr;

    // switch (methodType)
    // {
    // case 1:
    //     method = new CircumCenterMethod();
    //     break;
    // case 2:
    //     method = new MidpointMethod();
    //     break;
    // case 3:
    //     method = new CentroidMethod();
    //     break;
    // case 4:
    //     method = new oneCentroidMethod();
    //     break;
    // case 5:
    //     method = new ProjectionMethod();
    //     break;
    // default:
    //     break;
    // }
    

    // local search

    //local_search(cdt, steinerPoints);

    simulated_annealing(cdt, steinerPoints);


    // after
    CGAL::draw(cdt);
    obtuse_triangle_count = TriangulationUtils::countObtuseTriangles(cdt);
    std::cout << "Number of obtuse triangles: " << obtuse_triangle_count << std::endl;

    // Prepare output data
    output_data.content_type = "CG_SHOP_2025_Solution";
    output_data.instance_uid = input_data.instance_uid;

    // Steiner points x and y coordinates
    for (const auto& p : steinerPoints) {
        std::stringstream ss_x, ss_y;

        // fix ergasia 1 mistake
        auto exact_x = CGAL::exact(p.x());
        ss_x << exact_x.get_num() << "/" << exact_x.get_den();
        output_data.steiner_points_x.push_back(ss_x.str());

        auto exact_y = CGAL::exact(p.y());
        ss_y << exact_y.get_num() << "/" << exact_y.get_den();
        output_data.steiner_points_y.push_back(ss_y.str());
        // ss_x << CGAL::to_double(p.x());
        // ss_y << CGAL::to_double(p.y());
        // output_data.steiner_points_x.push_back(ss_x.str());
        // output_data.steiner_points_y.push_back(ss_y.str());
    }

    // Edges
    for (auto eit = cdt.finite_edges_begin(); eit != cdt.finite_edges_end(); ++eit) {
        auto face = eit->first;
        int index = eit->second;

        auto vh1 = face->vertex((index + 1) % 3);
        auto vh2 = face->vertex((index + 2) % 3);

        Point p1 = vh1->point();
        Point p2 = vh2->point();

        int idx1 = point_indices[p1];
        int idx2 = point_indices[p2];

        output_data.edges.push_back({ idx1, idx2 });
    }
}

int main(int argc, const char* argv[]) {
    if (argc != 4) {
        std::cout << "Usage: " << argv[0] << " <inputFile> <outputFile> <method>" << std::endl;
        return 1;
    }
    // Input and output file paths
    std::string input_filename = argv[1];
    std::string output_filename = argv[2];
    int method = std::stoi(argv[3]);

    // Parse input JSON
    InputData input_data = JsonUtils::parseInputJson(input_filename);

    // Prepare output data
    OutputData output_data;

    // Perform triangulation
    perform_triangulation(method, input_data, output_data);

    // Write output JSON
    JsonUtils::writeOutputJson(output_filename, output_data);

    std::cout << "Triangulation completed. Output written to " << output_filename << std::endl;

    return 0;
}
