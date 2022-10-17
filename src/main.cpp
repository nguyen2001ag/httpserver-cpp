#include <server.hpp>
#include <signal.h>
#include <iostream>


auto hello_world = [](const HttpRequest& request) -> HttpResponse {
    HttpResponse response(HttpStatus::Ok);
    response.addHeader("Content-Type", "text/html");
    std::string content;
    content += "<!doctype html>\n";
    content += "<html>\n<body>\n\n";
    content += "<h1>Hello World</h1>\n";
    content += "</body>\n</html>\n";
    response.setContent(content);
    return response;
};

auto random_html = [](const HttpRequest& request) -> HttpResponse {
    HttpResponse response(HttpStatus::Ok);
    std::string content;
    content += "<!doctype html>\n";
    content += "<html>\n<body>\n\n";
    content += "<h1>Another page</h1>\n";
    content += "<p>Random html page for demonstration</p>\n\n";
    content += "</body>\n</html>\n";

    response.addHeader("Content-Type", "text/html");
    response.setContent(content);
    return response;
};

void sigintHandler(int sig_num)
{
    printf("\nStopping SERVER\n");
    exit(sig_num);
}

int main(){
    signal(SIGINT, sigintHandler);
    std::string host = "0.0.0.0";
    int port = 8080;
    Server server(host, port);

    server.registerRequestHandler("/", HttpMethod::GET, hello_world);
    server.registerRequestHandler("/demonstration", HttpMethod::GET, random_html);

    server.start();
    std::cout << "Starting HTTP Server\n";
    std::cout << "Listening on " << host << ":" << port << std::endl;
    std::cout << "Press Ctrl-C to kill this server" << std::endl;
    for(;;);
    return 0;
}