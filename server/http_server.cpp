#include "http_server.h"
#include "route_handlers.h"
#include <fstream>
#include <sstream>

void HttpServer::setupCors() {
    svr_.set_pre_routing_handler([](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        if (req.method == "OPTIONS") {
            res.status = 204;
            return httplib::Server::HandlerResponse::Handled;
        }
        return httplib::Server::HandlerResponse::Unhandled;
    });
}

void HttpServer::setupStaticFiles(const std::string& staticDir) {
    if (staticDir.empty()) return;
    svr_.set_mount_point("/", staticDir);
    // SPA fallback: serve index.html for non-API 404s
    svr_.set_error_handler([staticDir](const httplib::Request& req, httplib::Response& res) {
        if (res.status == 404 && req.path.find("/api/") == std::string::npos) {
            std::ifstream f(staticDir + "/index.html");
            if (f.good()) {
                std::ostringstream ss;
                ss << f.rdbuf();
                res.set_content(ss.str(), "text/html");
                res.status = 200;
            }
        }
    });
}

void HttpServer::registerRoutes() {
    svr_.Post("/api/courses/upload", Handlers::uploadCourses);
    svr_.Post("/api/courses/fetch", Handlers::fetchCoursesFromUrl);
    svr_.Post("/api/courses/upload/preview", Handlers::previewUpload);
    svr_.Post("/api/courses/upload/mapped", Handlers::uploadWithMapping);
    svr_.Get("/api/university-profiles", Handlers::listProfiles);
    svr_.Post("/api/university-profiles", Handlers::saveProfile);
    svr_.Delete("/api/university-profiles/:id", Handlers::deleteProfile);
    svr_.Post("/api/courses/load", Handlers::loadCoursesFromHistory);
    svr_.Post("/api/courses/validate", Handlers::validateCourses);
    svr_.Get("/api/files", Handlers::getFileHistory);
    svr_.Delete("/api/files/:id", Handlers::deleteFile);
    svr_.Post("/api/schedules/generate", Handlers::generateSchedules);
    svr_.Post("/api/schedules/clean", Handlers::cleanSchedules);
    svr_.Post("/api/schedules/export/csv", Handlers::exportCsv);
    svr_.Get("/api/schedules/filtered-ids", Handlers::getFilteredIds);
    svr_.Get("/api/schedules/filtered-unique-ids", Handlers::getFilteredUniqueIds);
    svr_.Post("/api/schedules/convert/to-indices", Handlers::convertToIndices);
    svr_.Post("/api/schedules/convert/to-unique-ids", Handlers::convertToUniqueIds);
    svr_.Post("/api/bot/query", Handlers::botQuery);
    svr_.Get("/api/settings/:key", Handlers::getSetting);
    svr_.Put("/api/settings/:key", Handlers::setSetting);
    svr_.Post("/api/schedules/export/ical", Handlers::exportIcal);
    svr_.Get("/api/health", Handlers::health);
}

void HttpServer::start(int port, const std::string& staticDir) {
    setupCors();
    setupStaticFiles(staticDir);
    registerRoutes();
    svr_.listen("0.0.0.0", port);
}
