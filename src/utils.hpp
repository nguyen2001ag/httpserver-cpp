#pragma once
#include <string>
#include "http_standards.hpp"
#include <algorithm>

std::string method_to_string(HttpMethod method);
std::string status_to_string(HttpStatus status_code);
HttpMethod string_to_method(const std::string& method_string);


