#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

#include "triangulation.hpp"
#include "jsonUtils.hpp"
#include "triangulationUtils.hpp"
#include "triangulationMethod.hpp"
#include "circumCenterMethod.hpp"

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

    // Insert region boundary constraints
    // int rb_size = input_data.region_boundary.size();
    // for (int i = 0; i < rb_size; ++i) {
    //     int idx1 = input_data.region_boundary[i];
    //     int idx2 = input_data.region_boundary[(i + 1) % rb_size];
    //     cdt.insert_constraint(vertex_handles[idx1], vertex_handles[idx2]);
    // }

    // Insert additional constraints
    for (const auto& constraint : input_data.additional_constraints) {
        int idx1 = constraint[0];
        int idx2 = constraint[1];
        cdt.insert_constraint(vertex_handles[idx1], vertex_handles[idx2]);
    }
    int obtuse_triangle_count = TriangulationUtils::countObtuseTriangles(cdt);
    std::cout << "Number of obtuse triangles: " << obtuse_triangle_count << std::endl;
    TriangulationMethod* method = new CircumCenterMethod();
    method->execute(cdt, steinerPoints);
    delete method;
    obtuse_triangle_count = TriangulationUtils::countObtuseTriangles(cdt);
    std::cout << "Number of obtuse triangles: " << obtuse_triangle_count << std::endl;

    // Prepare output data
    output_data.content_type = "CG_SHOP_2025_Solution";
    output_data.instance_uid = input_data.instance_uid;

    // Steiner points x and y coordinates
    for (const auto& p : steinerPoints) {
        std::stringstream ss_x, ss_y;
        ss_x << CGAL::to_double(p.x());
        ss_y << CGAL::to_double(p.y());
        output_data.steiner_points_x.push_back(ss_x.str());
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

int main() {
    // Input and output file paths
    std::string input_filename = "data/input.json";
    std::string output_filename = "data/output.json";

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
