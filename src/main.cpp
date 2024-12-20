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
#include "algorithms.hpp"

std::string classifyInput(const std::vector<Point>& boundary, const std::vector<std::pair<Point, Point>>& constraints) {
    bool isConvex = TriangulationUtils::isConvexBoundary(boundary);
    bool isAxisParallel = TriangulationUtils::isAxisParallel(boundary);
    bool hasClosedConstraints = TriangulationUtils::areConstraintsClosed(constraints);

    if (isConvex) {
        if (constraints.empty()) {
            return "ls"; // Convex boundary, no constraints
        } else if (!hasClosedConstraints) {
            return "ls"; // Convex boundary, open constraints
        } else {
            return "sa"; // Convex boundary, closed constraints
        }
    } else {
        if (isAxisParallel) {
            return "sa"; // Non-convex boundary, axis-parallel segments
        } else {
            return "ant"; // Irregular non-convex boundary
        }
    }
}

std::vector<Point> constructBoundary(const InputData& input_data) {
    std::vector<Point> boundary;

    for (int index : input_data.region_boundary) {
        Point p(input_data.points_x[index], input_data.points_y[index]);
        boundary.push_back(p);
    }

    return boundary;
}


void perform_triangulation(const InputData& input_data, OutputData& output_data) {
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

    auto region_boundary = constructBoundary(input_data);
    std::vector<std::pair<Point, Point>> constraints;
    for (const auto& constraint : input_data.additional_constraints) {
        Point p1(input_data.points_x[constraint[0]], input_data.points_y[constraint[0]]);
        Point p2(input_data.points_x[constraint[1]], input_data.points_y[constraint[1]]);
        constraints.emplace_back(p1, p2);
    }

    auto algorithm = classifyInput(region_boundary, constraints);

    if ( algorithm == "ls" ){
        if ( input_data.delaunay == false )
            local_search(cdt, steinerPoints, input_data.L);
        local_search(cdt, steinerPoints, input_data.L);

    }else if ( algorithm == "sa" ){
        if ( input_data.delaunay == false )
            local_search(cdt, steinerPoints, input_data.L);
        simulated_annealing(cdt, steinerPoints, input_data.alpha, input_data.beta, input_data.L);

    } else if ( algorithm == "ant" ){
        if ( input_data.delaunay == false )
            local_search(cdt, steinerPoints, input_data.L);
        ant_colonies(cdt, steinerPoints, input_data.alpha, input_data.beta, input_data.xi, input_data.psi, input_data.lambda, input_data.kappa,input_data.L);

    }

  
    // after
    CGAL::draw(cdt);
    obtuse_triangle_count = TriangulationUtils::countObtuseTriangles(cdt);
    std::cout << "Number of obtuse triangles: " << obtuse_triangle_count << std::endl;

    // Prepare output data
    output_data.content_type = "CG_SHOP_2025_Solution";
    output_data.instance_uid = input_data.instance_uid;
    output_data.obtuse_triangle_count = obtuse_triangle_count;
    output_data.parameters = input_data.parameters;
    output_data.method = input_data.method;

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
    if (argc != 5) {
        std::cout << "Usage: " << argv[0] << " <inputFile> <outputFile> <method>" << std::endl;
        return 1;
    }
    // Input and output file paths
    std::string input_filename = argv[2];
    std::string output_filename = argv[4];

    // Parse input JSON
    InputData input_data = JsonUtils::parseInputJson(input_filename);

    // Prepare output data
    OutputData output_data;

    // Perform triangulation
    perform_triangulation(input_data, output_data);

    // Write output JSON
    JsonUtils::writeOutputJson(output_filename, output_data);

    std::cout << "Triangulation completed. Output written to " << output_filename << std::endl;

    return 0;
}
