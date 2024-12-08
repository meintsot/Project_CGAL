#pragma once
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Line_2.h>
#include <vector>
#include <string>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/number_utils_classes.h>
#include <cmath>
#include <random>

typedef CGAL::Exact_predicates_exact_constructions_kernel Kernel;
typedef Kernel::FT FT;
typedef CGAL::Point_2<Kernel> Point;
typedef CGAL::Segment_2<Kernel> Segment;
typedef CGAL::Constrained_Delaunay_triangulation_2<Kernel> CDT;
typedef CDT::Vertex_handle Vertex_handle;
typedef CDT::Face_handle Face_handle;
typedef CDT::Edge Edge;
typedef CGAL::Polygon_2<Kernel> Polygon_2;
typedef Kernel::Triangle_2 Triangle;
typedef Kernel::Line_2 Line;


struct InputData {
    std::string instance_uid;
    int num_points;
    std::vector<FT> points_x;
    std::vector<FT> points_y;
    std::vector<int> region_boundary;
    int num_constraints;
    std::vector<std::vector<int>> additional_constraints;
};

struct OutputData {
    std::string content_type;
    std::string instance_uid;
    std::vector<std::string> steiner_points_x;
    std::vector<std::string> steiner_points_y;
    std::vector<std::vector<int>> edges;
};
