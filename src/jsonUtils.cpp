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

    return input_data;
}

void JsonUtils::writeOutputJson(const std::string& filename, const OutputData& output_data) {
    json output_json;
    output_json["content_type"] = output_data.content_type;
    output_json["instance_uid"] = output_data.instance_uid;
    output_json["steiner_points_x"] = output_data.steiner_points_x;
    output_json["steiner_points_y"] = output_data.steiner_points_y;
    output_json["edges"] = output_data.edges;

    std::ofstream output_file(filename);
    output_file << output_json.dump(4);
}