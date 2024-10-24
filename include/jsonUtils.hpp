#pragma once

#include "./triangulation.hpp"

class JsonUtils {
public:
    static InputData parseInputJson(const std::string&);

    static void writeOutputJson(const std::string&, const OutputData&);
};
