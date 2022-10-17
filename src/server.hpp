#pragma once

#include "http_response.hpp"
#include "http_request.hpp"

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <stdexcept>
#include <unistd.h>
#include <iostream>
#include <arpa/inet.h>
#include <thread>
#include <stdio.h>
#include <string.h>
#include <sstream>
#include <functional>
#include <map>
#include <utility>



constexpr size_t maxHttpBufferSize = 4096;
using Request_Handler_t = std::function<HttpResponse (const HttpRequest&)>;

struct HttpData {
    HttpData(int fd) : fd(fd), ptr(0), length(0){}
    HttpData() : fd(0), ptr(0), length(0){}
    int fd;
    int ptr;
    int length;
    char buffer[maxHttpBufferSize];
};


class Server{
public:
    explicit Server(const std::string& host, std::uint16_t port) : mHost(host), mPort(port), mSocketFd(0){
        mServerAddr.sin_family = AF_INET; // Only support IPv4
        mServerAddr.sin_port = htons(port);
        mServerAddr.sin_addr.s_addr = INADDR_ANY;
        inet_pton(PF_INET, mHost.c_str(), &(mServerAddr.sin_addr.s_addr));
    }
    ~Server() = default;
    Server() = default;

    void start();
    void stop();
    void registerRequestHandler(std::string path, HttpMethod method, Request_Handler_t callback){
        URI uri(path);
        mRequestHanlders[uri].insert({method, callback});
    }

private:
    static constexpr int nThreads = 10;
    static constexpr int maxEvents = 10000;
    static constexpr int maxBackLogQueue = 4096; // Maximum number in Linux Kenel 5.4

    std::string mHost;
    std::uint16_t mPort;
    int mSocketFd;
    bool isRunning = false;
    sockaddr_in mServerAddr;
    std::map<URI, std::map<HttpMethod, Request_Handler_t>> mRequestHanlders; // Only support HTTP1.1 Method GET
    // Use map rather than hash_map because comparing string is faster than hashing it

    std::thread mListener;
    std::thread mBackends[nThreads];
    int mEpollFds[nThreads];
    epoll_event mThreadEvents[nThreads][maxEvents];


    void openSocket();
    void setupSocket();
    void bindSocket(sockaddr* server_addr);
    void listenSocket();
    void setupEpoll();
    void acceptNewConnections();
    void tickEpollEvents(int thread_id);
    void addEpollInstance(int epfd, int fd, std::uint32_t event, void* data);
    void delEpollInstance(int epfd, int fd);
    void modifyEpollInstance(int epfd, int fd, std::uint32_t event, void* data);
    void handleRequest(int epfd, HttpData* request);
    void handleHttpMessage(HttpData& request, HttpData* response);
    HttpResponse prepareResponse(HttpRequest& request);
    void handleResponse(int epfd, HttpData* request);


    void HandleEpollEvent(int epoll_fd, HttpData *data,
                                  std::uint32_t events);

};