#include "http_response.hpp"
#include <sstream>
#include <iostream>

std::string HttpResponse::to_string(){
    std::ostringstream ss;
    ss << "HTTP/1.1" << ' ';
    ss << static_cast<int>(getStatus()) << ' ';
    ss << status_to_string(getStatus()) << "\r\n";
    for (const auto& [k,v] : getHeaders())
        ss << k << ": " << v << "\r\n";
    ss << "\r\n";
    ss << getContent();

    return ss.str();
}