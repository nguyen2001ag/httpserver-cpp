#include "server.hpp"

void Server::start(){
    openSocket();
    setupSocket();
    bindSocket((sockaddr*) &mServerAddr);
    listenSocket();
    setupEpoll();
    isRunning = true;
    mListener = std::thread(&Server::acceptNewConnections, this);
    for (int i = 0; i < nThreads; i++){
        mBackends[i] = std::thread(&Server::tickEpollEvents, this, i);
    }
}

void Server::stop(){
    mListener.join();
    for (int i = 0; i < nThreads; i++) {
        mBackends[i].join();
        close(mEpollFds[i]);
    }
    isRunning = false;
    close(mSocketFd);
}

void Server::openSocket(){
    if ((mSocketFd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)) < 0){
        std::cout << "Failed to setup a socket\n";
    }
    std::cout << "Open socket at socket descriptor: " << mSocketFd << std::endl;
}

void Server::setupSocket(){
    int opt = 1;
    if (setsockopt(mSocketFd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0){
        std::cout << "Failed to setup socket to reusable address and port \n";
    }
}

void Server::bindSocket(sockaddr* server_addr){
    if ( bind(mSocketFd, server_addr, sizeof(*server_addr)) < 0){
        std::cout << "Failed to bind socket\n";
    }
}

void Server::listenSocket(){
    if (listen(mSocketFd, maxBackLogQueue) < 0){
        std::cout << "Failed to setup a listen port\n";
    }
}

void Server::setupEpoll(){
    for (int i = 0 ; i < nThreads; i++){
        if ((mEpollFds[i] = epoll_create1(0)) < 0 ){
            std::cout << "Failed to instantiate a file descriptor for backends" << std::endl;
        }
    }
}

void Server::acceptNewConnections(){
    int thread_id = 0;
    int client_fd;
    HttpData* client_data;
    sockaddr_in client_address;
    socklen_t client_len = sizeof(client_address);

    // accept new connections and distribute tasks to worker threads
    while (isRunning) {
        client_fd = accept4(mSocketFd, (sockaddr *)&client_address, &client_len, SOCK_NONBLOCK);
        if (client_fd < 0) continue;

        client_data = new HttpData(client_fd);
        addEpollInstance(mEpollFds[thread_id], client_fd, EPOLLIN, (void*) client_data);
        thread_id = (thread_id + 1 == nThreads) ? 0 : thread_id + 1;
   }
}

void Server::tickEpollEvents(int thread_id){
    HttpData *raw_data;
    int listen_thread_fd = mEpollFds[thread_id];
    while (isRunning) {
        int nReadyData = epoll_wait(listen_thread_fd, mThreadEvents[thread_id], Server::maxEvents, 0);
        if (nReadyData < 0) continue;

        for (int i = 0; i < nReadyData; i++) {
            const epoll_event &process_event = mThreadEvents[thread_id][i];
            raw_data = reinterpret_cast<HttpData *>(process_event.data.ptr);
            if (process_event.events == EPOLLIN)  {
                handleRequest(listen_thread_fd, raw_data);
            } 
            else if (process_event.events == EPOLLOUT){
                handleResponse(listen_thread_fd, raw_data);
            }
            else {  
                delEpollInstance(listen_thread_fd, raw_data->fd);
                close(raw_data->fd);
                delete raw_data;
            }
        }
  }
}

void Server::handleRequest(int epfd, HttpData* request){
    HttpData *response;
    ssize_t byte_count = recv(request->fd, request->buffer, maxHttpBufferSize, 0);  // Non-blocking read
    int fd = request->fd;
    if (byte_count > 0) {  // Received message
        response = new HttpData(fd);
        handleHttpMessage(*request, response);
        modifyEpollInstance(epfd, response->fd, EPOLLOUT, response);
        delete request;
    } 
    else if (byte_count == 0) {  // client has closed connection
        delEpollInstance(epfd, fd);
        close(fd);
        delete request;
    } 
    else {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {  // Try to read again
            modifyEpollInstance(epfd, fd, EPOLLIN, request);
        } 
        else {  // Close connection if have errors
            delEpollInstance(epfd, fd);
            close(fd);
            delete request;
        }
    }
}

void Server::handleResponse(int epfd, HttpData* response){
    // std::cout << "Im here handling response" << std::endl;
    HttpData* request;
    ssize_t byte_count = send(response->fd, response->buffer + response->ptr, response->length, 0); // Non-blocking send
    if (byte_count >= 0) {
        if (byte_count < response->length) {  // there are still bytes to write
            response->ptr += byte_count;
            response->length -= byte_count;
            modifyEpollInstance(epfd, response->fd, EPOLLOUT, response);
        } 
        else {  //Finish message, keep the connection
            request = new HttpData(response->fd);
            modifyEpollInstance(epfd, request->fd, EPOLLIN, request);
            delete response;
        }
    } 
    else {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {  // retry
            addEpollInstance(epfd, response->fd, EPOLLOUT, response);
        } 
        else {  // other error
            delEpollInstance(epfd, response->fd);
            close(response->fd);
            delete response;
        }
    }

}

// This function fill response with data to be sent to client
void Server::handleHttpMessage(HttpData& request, HttpData* response){  
    HttpRequest requestIns(std::string(request.buffer));
    HttpResponse responseIns;
    std::string response_str;

    responseIns = prepareResponse(requestIns);
    response_str = responseIns.to_string();
    memcpy(response->buffer, response_str.c_str(), maxHttpBufferSize);
    response->length = response_str.size();
}

HttpResponse Server::prepareResponse(HttpRequest& request){
    auto it = mRequestHanlders.find(request.getURI());
    if (it == mRequestHanlders.end()) {  // No path was found for this
        return HttpResponse(HttpStatus::NotFound);
    }
    auto callback = it->second.find(request.getMethod());
    if (callback == it->second.end()) {  // no handler for this method
        return HttpResponse(HttpStatus::MethodNotAllowed);
    }
    return callback->second(request);  // call handler to process the request
}


void Server::addEpollInstance(int epfd, int fd, std::uint32_t event, void* data){
    epoll_event ev;
    ev.events = event;
    ev.data.ptr = data;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) < 0){
        std::cout << "Failed to add epoll event to interest list\n";
        std::cout << errno << std::endl;
    }
}

void Server::delEpollInstance(int epfd, int fd){
    if (epoll_ctl(epfd, EPOLL_CTL_DEL, fd, nullptr) < 0){
        std::cout << "Failed to remove epoll event from interest list\n";
    }
}

void Server::modifyEpollInstance(int epfd, int fd, std::uint32_t event, void* data){
    epoll_event ev;
    ev.events = event;
    ev.data.ptr = data;
    if (epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev) < 0){
        std::cout << "Failed to modify epoll event to interest list\n";
    }
}
