// main.cpp

#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

#include "triangulation.hpp"

using json = nlohmann::json;

InputData parse_input_json(const std::string& filename) {
    InputData input_data;
    std::ifstream input_file(filename);
    json input_json;
    input_file >> input_json;

    input_data.instance_uid = input_json["instance_uid"];
    input_data.num_points = input_json["num_points"];

    auto points_x_json = input_json["points_x"];
    auto points_y_json = input_json["points_y"];

    for (const auto& x_val : points_x_json) {
        input_data.points_x.push_back(FT(x_val.get<int>()));
    }

    for (const auto& y_val : points_y_json) {
        input_data.points_y.push_back(FT(y_val.get<int>()));
    }

    input_data.region_boundary = input_json["region_boundary"].get<std::vector<int>>();
    input_data.num_constraints = input_json["num_constraints"];
    input_data.additional_constraints = input_json["additional_constraints"].get<std::vector<std::vector<int>>>();

    return input_data;
}

void write_output_json(const std::string& filename, const OutputData& output_data) {
    json output_json;
    output_json["content_type"] = output_data.content_type;
    output_json["instance_uid"] = output_data.instance_uid;
    output_json["steiner_points_x"] = output_data.steiner_points_x;
    output_json["steiner_points_y"] = output_data.steiner_points_y;
    output_json["edges"] = output_data.edges;

    std::ofstream output_file(filename);
    output_file << output_json.dump(4);
}

bool is_obtuse_triangle(const Point& a, const Point& b, const Point& c) {
    // Compute the squared lengths of the sides
    FT ab2 = CGAL::squared_distance(a, b);
    FT bc2 = CGAL::squared_distance(b, c);
    FT ca2 = CGAL::squared_distance(c, a);

    // Use the Law of Cosines to check for obtuse angles
    if (ab2 + bc2 < ca2) return true;
    if (bc2 + ca2 < ab2) return true;
    if (ca2 + ab2 < bc2) return true;

    return false;
}

void perform_triangulation(const InputData& input_data, OutputData& output_data) {
    CDT cdt;

    std::vector<Point> points;
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
    int rb_size = input_data.region_boundary.size();
    for (int i = 0; i < rb_size; ++i) {
        int idx1 = input_data.region_boundary[i];
        int idx2 = input_data.region_boundary[(i + 1) % rb_size];
        cdt.insert_constraint(vertex_handles[idx1], vertex_handles[idx2]);
    }

    // Insert additional constraints
    for (const auto& constraint : input_data.additional_constraints) {
        int idx1 = constraint[0];
        int idx2 = constraint[1];
        cdt.insert_constraint(vertex_handles[idx1], vertex_handles[idx2]);
    }

    // Set to keep track of Steiner points to avoid duplicates
    std::set<Point> steiner_points_set;

    bool has_obtuse_triangles = true;
    while (has_obtuse_triangles) {
        has_obtuse_triangles = false;
        std::vector<Point> new_steiner_points;

        // Iterate over all finite faces to find obtuse triangles
        for (auto fit = cdt.finite_faces_begin(); fit != cdt.finite_faces_end(); ++fit) {
            Point a = fit->vertex(0)->point();
            Point b = fit->vertex(1)->point();
            Point c = fit->vertex(2)->point();

            if (is_obtuse_triangle(a, b, c)) {
                has_obtuse_triangles = true;

                // Find the longest edge
                std::array<std::pair<Point, Point>, 3> edges = {
                    std::make_pair(a, b),
                    std::make_pair(b, c),
                    std::make_pair(c, a)
                };

                double max_length = 0.0;
                Point p1, p2;
                for (const auto& edge : edges) {
                    double length = CGAL::to_double(CGAL::squared_distance(edge.first, edge.second));
                    if (length > max_length) {
                        max_length = length;
                        p1 = edge.first;
                        p2 = edge.second;
                    }
                }

                // Compute the midpoint of the longest edge
                Point midpoint = CGAL::midpoint(p1, p2);

                // Check if the midpoint is not already inserted
                if (steiner_points_set.find(midpoint) == steiner_points_set.end()) {
                    new_steiner_points.push_back(midpoint);
                    steiner_points_set.insert(midpoint);
                }
            }
        }

        // Insert new Steiner points
        if (!new_steiner_points.empty()) {
            for (const auto& sp : new_steiner_points) {
                Vertex_handle vh = cdt.insert(sp);
                points.push_back(sp);
                vertex_handles.push_back(vh);
                point_indices[sp] = static_cast<int>(points.size()) - 1;
            }
        } else {
            // No new Steiner points added; break the loop
            break;
        }
    }

    // Collect Steiner points
    std::vector<Point> steiner_points;
    for (auto vit = cdt.finite_vertices_begin(); vit != cdt.finite_vertices_end(); ++vit) {
        Point p = vit->point();
        if (point_indices.find(p) == point_indices.end()) {
            steiner_points.push_back(p);
            point_indices[p] = static_cast<int>(points.size());
            points.push_back(p);
        }
    }

    // Prepare output data
    output_data.content_type = "CG_SHOP_2025_Solution";
    output_data.instance_uid = input_data.instance_uid;

    // Steiner points x and y coordinates
    for (const auto& p : steiner_points_set) {
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
    InputData input_data = parse_input_json(input_filename);

    // Prepare output data
    OutputData output_data;

    // Perform triangulation
    perform_triangulation(input_data, output_data);

    // Write output JSON
    write_output_json(output_filename, output_data);

    std::cout << "Triangulation completed. Output written to " << output_filename << std::endl;

    return 0;
}
