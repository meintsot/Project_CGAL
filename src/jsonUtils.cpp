#include "triangulation.hpp"
#include "jsonUtils.hpp"

using json = nlohmann::json;

InputData JsonUtils::parseInputJson(const std::string& filename) {
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

    // if (input_json.contains("method")) {
    //         input_data.method = input_json["method"].get<std::string>();
    //     } else {
    //         input_data.method = "auto";
    // }   
    //input_data.delaunay = input_json["delaunay"];

    input_data.method = input_json.value("method", "auto");      // Default: "auto"
    input_data.delaunay = input_json.value("delaunay", true);   // Default: false


    input_data.parameters = input_json.value("parameters", json::object()); // Default: empty object

    //if (input_data.method == "ls") {
        input_data.L = input_data.parameters.value("L", 500);  // Default: 500
    //} else if (input_data.method == "sa") {
        //input_data.alpha = input_data.parameters.value("alpha", 0.0); // Default: 0.0
        //input_data.beta = input_data.parameters.value("beta", 0.0);
        //input_data.L = input_data.parameters.value("L", 0);
    //} else if (input_data.method == "ant") {
        input_data.alpha = input_data.parameters.value("alpha", 3.5);
        input_data.beta = input_data.parameters.value("beta", 1.0);
        input_data.xi = input_data.parameters.value("xi", 1.0);
        input_data.psi = input_data.parameters.value("psi", 2.0);
        input_data.lambda = input_data.parameters.value("lambda", 0.5);
        input_data.kappa = input_data.parameters.value("kappa", 10);
        //input_data.L = input_data.parameters.value("L", 0);
    //}

    

    return input_data;
}

void JsonUtils::writeOutputJson(const std::string& filename, const OutputData& output_data) {
    json output_json;
    output_json["content_type"] = output_data.content_type;
    output_json["instance_uid"] = output_data.instance_uid;
    output_json["steiner_points_x"] = output_data.steiner_points_x;
    output_json["steiner_points_y"] = output_data.steiner_points_y;
    output_json["edges"] = output_data.edges;
    output_json["obtuse_count"] = output_data.obtuse_triangle_count;
    output_json["method"] = output_data.method;
    output_json["parameters"] = output_data.parameters;

    std::ofstream output_file(filename);
    output_file << output_json.dump(4);
}