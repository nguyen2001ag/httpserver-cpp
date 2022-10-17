#include "utils.hpp"

std::string method_to_string(HttpMethod method) {
    switch (method) {
        case HttpMethod::GET:
            return "GET";
        case HttpMethod::HEAD:
            return "HEAD";
        case HttpMethod::POST:
            return "POST";
        case HttpMethod::PUT:
            return "PUT";
        case HttpMethod::DELETE:
            return "DELETE";
        case HttpMethod::CONNECT:
            return "CONNECT";
        case HttpMethod::OPTIONS:
            return "OPTIONS";
        case HttpMethod::TRACE:
            return "TRACE";
        case HttpMethod::PATCH:
            return "PATCH";
        default:
            return std::string();
  }
}


std::string status_to_string(HttpStatus status_code) {
    switch (status_code) {
        case HttpStatus::Continue:
            return "Continue";
        case HttpStatus::Ok:
            return "OK";
        case HttpStatus::Accepted:
            return "Accepted";
        case HttpStatus::MovedPermanently:
            return "Moved Permanently";
        case HttpStatus::Found:
            return "Found";
        case HttpStatus::BadRequest:
            return "Bad Request";
        case HttpStatus::Forbidden:
            return "Forbidden";
        case HttpStatus::NotFound:
            return "Not Found";
        case HttpStatus::MethodNotAllowed:
            return "Method Not Allowed";
        case HttpStatus::ImATeapot:
            return "I'm a Teapot";
        case HttpStatus::InternalServerError:
            return "Internal Server Error";
        case HttpStatus::NotImplemented:
            return "Not Implemented";
        case HttpStatus::BadGateway:
            return "Bad Gateway";
        default:
            return std::string();
  }
}

HttpMethod string_to_method(const std::string& method_string) {
    std::string method_string_uppercase;
    std::transform(method_string.begin(), method_string.end(),
                 std::back_inserter(method_string_uppercase),
                 [](char c) { return toupper(c); });
    if (method_string_uppercase == "GET") {
        return HttpMethod::GET;
    } else if (method_string_uppercase == "HEAD") {
        return HttpMethod::HEAD;
    } else if (method_string_uppercase == "POST") {
        return HttpMethod::POST;
    } else if (method_string_uppercase == "PUT") {
        return HttpMethod::PUT;
    } else if (method_string_uppercase == "DELETE") {
        return HttpMethod::DELETE;
    } else if (method_string_uppercase == "CONNECT") {
        return HttpMethod::CONNECT;
    } else if (method_string_uppercase == "OPTIONS") {
        return HttpMethod::OPTIONS;
    } else if (method_string_uppercase == "TRACE") {
        return HttpMethod::TRACE;
    } else if (method_string_uppercase == "PATCH") {
        return HttpMethod::PATCH;
    } else {
        return HttpMethod::INVALID;
    }
}