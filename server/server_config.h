#pragma once
#include <string>
#include <cstdlib>

struct ServerConfig {
    static int port() {
        const char* p = std::getenv("SCHEDULIFY_PORT");
        return (p && *p) ? std::stoi(std::string(p)) : 8080;
    }
    static std::string staticDir() {
        const char* d = std::getenv("SCHEDULIFY_STATIC_DIR");
        return (d && *d) ? std::string(d) : "";
    }
};
