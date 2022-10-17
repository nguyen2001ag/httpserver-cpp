#pragma once

#include "http_message.hpp"
#include "uri.hpp"
#include "utils.hpp"

class HttpRequest: public HttpMessage {
public:
    HttpRequest() = default;
    HttpRequest(const std::string& request);
    ~HttpRequest() = default;


    void setMethod(HttpMethod method){
        mMethod = method;
    }

    HttpMethod getMethod(){
        return mMethod;
    }

    void setURI(const std::string& path){
        uri.updatePath(path);
    }

    std::string getPath(){
        return uri.getPath();
    }

    URI getURI(){
        return uri;
    }

    std::string to_string();


private:
    HttpMethod mMethod;
    URI uri;
    std::string version;
};