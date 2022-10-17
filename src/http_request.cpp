#include <iostream>
#include <sstream>

#include "http_request.hpp"
#include "utils.hpp"

HttpRequest::HttpRequest(const std::string& request){
    std::string firstLine;
    std::string method, version, path;
    std::string headers, headerLine, header, value;

    std::string body;

    size_t lptr = 0, rptr = 0;

    rptr = request.find("\r\n", lptr);
    if (rptr == std::string::npos){
        std::cout << "No start line found\n";
        
    }
    firstLine = request.substr(lptr, rptr - lptr);
    // Find ptr between headers and body, body only valid with headers
    lptr = rptr + 2;
    rptr = request.find("\r\n\r\n", lptr);
    if (rptr != std::string::npos){ //Found headers
        headers = request.substr(lptr, rptr - lptr);
        lptr = rptr + 4;
        rptr = request.size();
        if (rptr > lptr){ // Check if there is a body
            body = request.substr(lptr, rptr - lptr);
        }
    }

    std::string token;
    std::istringstream ss(firstLine);
    ss >> method >> path >> version;
    if (!ss.good() && !ss.eof()){
        std::cout << "Unable to read the first line from HTTP Request\n";
        return;
    }

    setMethod(string_to_method(method));
    setURI(path);

    ss.clear();
    ss.str(headers);
    while(std::getline(ss, headerLine)){
        size_t left = 0, right = 0;
        right = headerLine.find(":", left);
        
        if (right == std::string::npos){
            std::cout << "Header is invalid\n";
            continue;
        }
        header = headerLine.substr(left, right - left);

        //Getting value of header
        left = right + 2;
        right = headerLine.size();
        value = headerLine.substr(left, right - left);
    }

    setContent(body);
}

std::string HttpRequest::to_string(){
    std::ostringstream ss;
    ss << method_to_string(getMethod()) << ' ';
    ss << getPath() << ' ';
    ss << "HTTP/1.1" << "\r\n";
    for (const auto& [k,v] : getHeaders())
        ss << k << ": " << v << "\r\n";
    ss << "\r\n";
    ss << getContent();

    return ss.str();
}