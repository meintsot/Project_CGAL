#pragma once

#include "./triangulation.hpp"
#include <CGAL/draw_triangulation_2.h>

class JsonUtils {
public:
    static InputData parseInputJson(const std::string&);

    static void writeOutputJson(const std::string&, const OutputData&);
};
