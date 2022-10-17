#pragma once

#include "http_standards.hpp"
#include "http_message.hpp"
#include "utils.hpp"

class HttpResponse: public HttpMessage {
public:
    HttpResponse(): mStatus(HttpStatus::INVALID){}
    HttpResponse(HttpStatus status) : mStatus(status){}
    HttpResponse(std::string response); // Not implemented as parsing a response is not required for a server

    ~HttpResponse() = default;

    void setStatus(HttpStatus status) { mStatus = status; }

    HttpStatus getStatus(){ return mStatus; }

    std::string to_string();

private:
    HttpStatus mStatus;
};