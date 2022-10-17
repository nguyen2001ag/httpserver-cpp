#pragma once

#include <string>
#include <map>
#include <utility>
#include "http_standards.hpp"

class HttpMessage{
public:
    HttpMessage() = default;
    ~HttpMessage() = default;

    void addHeader(const std::string & header, const std::string &value){
        mHeaders[header] = std::move(value);
    }

    void removeHeader(const std::string &header){
        mHeaders.erase(header);
    }

    void clearAllHeaders(){
        mHeaders.clear();
    }

    std::string getHttpVersion(){
        return mHttpVersion;
    }

    void addContent(const std::string & content){
        mContent += std::move(content);
        mContentLength += content.length();
        setContentLength();
    }

    void setContent(const std::string & content){
        mContent = std::move(content);
        mContentLength = content.length();
        setContentLength();
    }

    void setContentLength(){
        addHeader("Content-Length", std::to_string(mContentLength));
    }

    std::string getContent(){
        return mContent;
    }
    
    void clearContent(){
        mContent.clear();
        mContentLength = 0;
        setContentLength();
    }

    unsigned int getContentLength(){
        return mContentLength;
    }

    std::string getHeaderPair(const std::string &header) {
        if (mHeaders.find(header) != mHeaders.end())
            return mHeaders[header];
        else{
            return std::string();
        }
    }

    std::map<std::string, std::string> getHeaders (){
        return mHeaders;
    }


protected:
    std::string mHttpVersion = "HTTP/1.1";
    std::map<std::string, std::string> mHeaders;
    std::string mContent;
    unsigned int mContentLength = 0;
};