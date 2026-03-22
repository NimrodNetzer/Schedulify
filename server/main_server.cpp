#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <QCoreApplication>
#include "model_db_integration.h"
#include "db_manager.h"
#include "logger.h"
#include "http_server.h"
#include "server_config.h"
#include <iostream>

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);
    app.setApplicationName("Schedulify");
    app.setOrganizationName("Schedulify");

    Logger::get().logInitiate();
    Logger::get().logInfo("Schedulify server starting...");

    try {
        auto& dbIntegration = ModelDatabaseIntegration::getInstance();
        if (!dbIntegration.initializeDatabase()) {
            Logger::get().logWarning("Database initialization failed - continuing without persistence");
        } else {
            Logger::get().logInfo("Database initialized successfully");
        }
    } catch (const std::exception& e) {
        Logger::get().logWarning("Database init exception: " + std::string(e.what()));
    }

    HttpServer server;
    int port = ServerConfig::port();
    std::string staticDir = ServerConfig::staticDir();

    Logger::get().logInfo("Starting HTTP server on port " + std::to_string(port));
    if (!staticDir.empty()) {
        Logger::get().logInfo("Serving static files from: " + staticDir);
    }

    server.start(port, staticDir);
    return 0;
}
