#pragma once
#include <string>
#include <httplib.h>

class HttpServer {
public:
    void start(int port, const std::string& staticDir);
private:
    httplib::Server svr_;
    void setupCors();
    void setupStaticFiles(const std::string& staticDir);
    void registerRoutes();
};
