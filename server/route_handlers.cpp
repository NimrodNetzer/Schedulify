#include "route_handlers.h"
#include "json_serializers.h"
#include "main_model.h"
#include "model_interfaces.h"
#include "db_entities.h"
#include "column_mapping.h"
#include "universal_parser.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <random>
#include <unordered_map>
#include <ctime>
#include <curl/curl.h>

static std::string makeTempPath(const std::string& ext) {
    auto tmp = std::filesystem::temp_directory_path();
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(100000, 999999);
    return (tmp / ("sched_" + std::to_string(dis(gen)) + ext)).string();
}

static void sendJson(httplib::Response& res, const Json::Value& val, int status = 200) {
    res.status = status;
    res.set_content(jsonToString(val), "application/json");
}

static void sendError(httplib::Response& res, const std::string& msg, int status = 500) {
    Json::Value err;
    err["error"] = msg;
    sendJson(res, err, status);
}

namespace Handlers {

void uploadCourses(const httplib::Request& req, httplib::Response& res) {
    try {
        auto file = req.get_file_value("file");
        if (file.filename.empty()) {
            sendError(res, "No file uploaded", 400);
            return;
        }

        std::string ext = ".txt";
        auto pos = file.filename.rfind('.');
        if (pos != std::string::npos) {
            ext = file.filename.substr(pos);
        }

        std::string tmpPath = makeTempPath(ext);
        {
            std::ofstream out(tmpPath, std::ios::binary);
            out.write(file.content.data(), static_cast<std::streamsize>(file.content.size()));
        }

        IModel& model = Model::getInstance();
        void* result = model.executeOperation(ModelOperation::GENERATE_COURSES, nullptr, tmpPath);

        std::filesystem::remove(tmpPath);

        if (!result) {
            sendError(res, "Failed to parse courses", 500);
            return;
        }

        auto* courses = static_cast<std::vector<Course>*>(result);
        Json::Value arr(Json::arrayValue);
        for (const auto& c : *courses) arr.append(courseToJson(c));
        delete courses;

        sendJson(res, arr);
    } catch (const std::exception& e) {
        sendError(res, e.what());
    }
}

void loadCoursesFromHistory(const httplib::Request& req, httplib::Response& res) {
    try {
        Json::Value body = parseJson(req.body);
        FileLoadData loadData = fileLoadDataFromJson(body);

        IModel& model = Model::getInstance();
        void* result = model.executeOperation(ModelOperation::LOAD_FROM_HISTORY, &loadData, "");

        if (!result) {
            sendError(res, "Failed to load courses from history", 500);
            return;
        }

        auto* courses = static_cast<std::vector<Course>*>(result);
        Json::Value arr(Json::arrayValue);
        for (const auto& c : *courses) arr.append(courseToJson(c));
        delete courses;

        sendJson(res, arr);
    } catch (const std::exception& e) {
        sendError(res, e.what());
    }
}

void validateCourses(const httplib::Request& req, httplib::Response& res) {
    try {
        Json::Value body = parseJson(req.body);
        std::vector<Course> courses;
        for (const auto& j : body) {
            courses.push_back(courseFromJson(j));
        }

        IModel& model = Model::getInstance();
        void* result = model.executeOperation(ModelOperation::VALIDATE_COURSES, &courses, "");

        Json::Value resp;
        Json::Value warnings(Json::arrayValue);

        if (result) {
            auto* msgs = static_cast<std::vector<std::string>*>(result);
            for (const auto& w : *msgs) warnings.append(w);
            delete msgs;
        }

        resp["warnings"] = warnings;
        sendJson(res, resp);
    } catch (const std::exception& e) {
        sendError(res, e.what());
    }
}

void getFileHistory(const httplib::Request& /*req*/, httplib::Response& res) {
    try {
        IModel& model = Model::getInstance();
        void* result = model.executeOperation(ModelOperation::GET_FILE_HISTORY, nullptr, "");

        if (!result) {
            Json::Value arr(Json::arrayValue);
            sendJson(res, arr);
            return;
        }

        auto* files = static_cast<std::vector<FileEntity>*>(result);
        Json::Value arr(Json::arrayValue);
        for (const auto& f : *files) arr.append(fileEntityToJson(f));
        delete files;

        sendJson(res, arr);
    } catch (const std::exception& e) {
        sendError(res, e.what());
    }
}

void deleteFile(const httplib::Request& req, httplib::Response& res) {
    try {
        int id = std::stoi(req.path_params.at("id"));

        IModel& model = Model::getInstance();
        void* result = model.executeOperation(ModelOperation::DELETE_FILE_FROM_HISTORY, &id, "");

        Json::Value resp;
        if (result) {
            auto* success = static_cast<bool*>(result);
            resp["success"] = *success;
            delete success;
        } else {
            resp["success"] = false;
        }
        sendJson(res, resp);
    } catch (const std::exception& e) {
        sendError(res, e.what());
    }
}

void generateSchedules(const httplib::Request& req, httplib::Response& res) {
    try {
        Json::Value body = parseJson(req.body);
        std::string semester = body.get("semester", "A").asString();

        std::vector<Course> courses;
        for (const auto& j : body["courses"]) {
            courses.push_back(courseFromJson(j));
        }

        IModel& model = Model::getInstance();
        void* result = model.executeOperation(ModelOperation::GENERATE_SCHEDULES, &courses, semester);

        if (!result) {
            sendError(res, "Failed to generate schedules", 500);
            return;
        }

        auto* schedules = static_cast<std::vector<InformativeSchedule>*>(result);
        Json::Value arr(Json::arrayValue);
        for (const auto& s : *schedules) arr.append(scheduleToJson(s));
        delete schedules;

        sendJson(res, arr);
    } catch (const std::exception& e) {
        sendError(res, e.what());
    }
}

void cleanSchedules(const httplib::Request& /*req*/, httplib::Response& res) {
    try {
        IModel& model = Model::getInstance();
        model.executeOperation(ModelOperation::CLEAN_SCHEDULES, nullptr, "");

        Json::Value resp;
        resp["success"] = true;
        sendJson(res, resp);
    } catch (const std::exception& e) {
        sendError(res, e.what());
    }
}

void exportCsv(const httplib::Request& req, httplib::Response& res) {
    try {
        Json::Value body = parseJson(req.body);
        InformativeSchedule schedule = scheduleFromJson(body);

        std::string tmpPath = makeTempPath(".csv");

        IModel& model = Model::getInstance();
        model.executeOperation(ModelOperation::SAVE_SCHEDULE, &schedule, tmpPath);

        std::ifstream f(tmpPath, std::ios::binary);
        if (!f.good()) {
            sendError(res, "Failed to export CSV", 500);
            return;
        }

        std::ostringstream ss;
        ss << f.rdbuf();
        f.close();
        std::filesystem::remove(tmpPath);

        res.status = 200;
        res.set_header("Content-Disposition", "attachment; filename=\"schedule.csv\"");
        res.set_content(ss.str(), "text/csv");
    } catch (const std::exception& e) {
        sendError(res, e.what());
    }
}

void getFilteredIds(const httplib::Request& /*req*/, httplib::Response& res) {
    try {
        IModel& model = Model::getInstance();
        void* result = model.executeOperation(ModelOperation::GET_LAST_FILTERED_IDS, nullptr, "");

        Json::Value resp;
        Json::Value ids(Json::arrayValue);

        if (result) {
            auto* vec = static_cast<std::vector<int>*>(result);
            for (int id : *vec) ids.append(id);
            delete vec;
        }

        resp["ids"] = ids;
        sendJson(res, resp);
    } catch (const std::exception& e) {
        sendError(res, e.what());
    }
}

void getFilteredUniqueIds(const httplib::Request& /*req*/, httplib::Response& res) {
    try {
        IModel& model = Model::getInstance();
        void* result = model.executeOperation(ModelOperation::GET_LAST_FILTERED_UNIQUE_IDS, nullptr, "");

        Json::Value resp;
        Json::Value uids(Json::arrayValue);

        if (result) {
            auto* vec = static_cast<std::vector<std::string>*>(result);
            for (const auto& uid : *vec) uids.append(uid);
            delete vec;
        }

        resp["unique_ids"] = uids;
        sendJson(res, resp);
    } catch (const std::exception& e) {
        sendError(res, e.what());
    }
}

void convertToIndices(const httplib::Request& req, httplib::Response& res) {
    try {
        Json::Value body = parseJson(req.body);
        UniqueIdConversionRequest convReq;
        convReq.semester = body.get("semester", "A").asString();
        for (const auto& uid : body["unique_ids"]) {
            convReq.uniqueIds.push_back(uid.asString());
        }

        IModel& model = Model::getInstance();
        void* result = model.executeOperation(ModelOperation::CONVERT_UNIQUE_IDS_TO_INDICES, &convReq, "");

        Json::Value resp;
        Json::Value indices(Json::arrayValue);

        if (result) {
            auto* vec = static_cast<std::vector<int>*>(result);
            for (int idx : *vec) indices.append(idx);
            delete vec;
        }

        resp["indices"] = indices;
        sendJson(res, resp);
    } catch (const std::exception& e) {
        sendError(res, e.what());
    }
}

void convertToUniqueIds(const httplib::Request& req, httplib::Response& res) {
    try {
        Json::Value body = parseJson(req.body);
        IndexConversionRequest convReq;
        convReq.semester = body.get("semester", "A").asString();
        for (const auto& idx : body["indices"]) {
            convReq.indices.push_back(idx.asInt());
        }

        IModel& model = Model::getInstance();
        void* result = model.executeOperation(ModelOperation::CONVERT_INDICES_TO_UNIQUE_IDS, &convReq, "");

        Json::Value resp;
        Json::Value uids(Json::arrayValue);

        if (result) {
            auto* vec = static_cast<std::vector<std::string>*>(result);
            for (const auto& uid : *vec) uids.append(uid);
            delete vec;
        }

        resp["unique_ids"] = uids;
        sendJson(res, resp);
    } catch (const std::exception& e) {
        sendError(res, e.what());
    }
}

void botQuery(const httplib::Request& req, httplib::Response& res) {
    try {
        Json::Value body = parseJson(req.body);
        BotQueryRequest botReq = botQueryRequestFromJson(body);

        IModel& model = Model::getInstance();
        void* result = model.executeOperation(ModelOperation::BOT_QUERY_SCHEDULES, &botReq, "");

        if (!result) {
            sendError(res, "Bot query failed", 500);
            return;
        }

        auto* response = static_cast<BotQueryResponse*>(result);
        Json::Value resp = botQueryResponseToJson(*response);
        delete response;

        sendJson(res, resp);
    } catch (const std::exception& e) {
        sendError(res, e.what());
    }
}

void getSetting(const httplib::Request& req, httplib::Response& res) {
    try {
        std::string key = req.path_params.at("key");
        SettingRequest settingReq(key);

        IModel& model = Model::getInstance();
        void* result = model.executeOperation(ModelOperation::GET_SETTING, &settingReq, "");

        Json::Value resp;
        resp["key"] = key;

        if (result) {
            auto* val = static_cast<std::string*>(result);
            resp["value"] = *val;
            delete val;
        } else {
            resp["value"] = "";
        }

        sendJson(res, resp);
    } catch (const std::exception& e) {
        sendError(res, e.what());
    }
}

void setSetting(const httplib::Request& req, httplib::Response& res) {
    try {
        std::string key = req.path_params.at("key");
        Json::Value body = parseJson(req.body);
        std::string value = body.get("value", "").asString();

        SettingRequest settingReq(key, value);

        IModel& model = Model::getInstance();
        void* result = model.executeOperation(ModelOperation::SET_SETTING, &settingReq, "");

        Json::Value resp;
        if (result) {
            auto* success = static_cast<bool*>(result);
            resp["success"] = *success;
            delete success;
        } else {
            resp["success"] = false;
        }

        sendJson(res, resp);
    } catch (const std::exception& e) {
        sendError(res, e.what());
    }
}

void previewUpload(const httplib::Request& req, httplib::Response& res) {
    try {
        auto file = req.get_file_value("file");
        if (file.filename.empty()) { sendError(res, "No file uploaded", 400); return; }

        std::string ext = ".xlsx";
        auto pos = file.filename.rfind('.');
        if (pos != std::string::npos) ext = file.filename.substr(pos);

        std::string tmpPath = makeTempPath(ext);
        { std::ofstream out(tmpPath, std::ios::binary); out.write(file.content.data(), static_cast<std::streamsize>(file.content.size())); }

        auto rows = previewFile(tmpPath, 6);
        std::filesystem::remove(tmpPath);

        Json::Value result(Json::arrayValue);
        for (const auto& row : rows) {
            Json::Value jrow(Json::arrayValue);
            for (const auto& cell : row) jrow.append(cell);
            result.append(jrow);
        }
        sendJson(res, result);
    } catch (const std::exception& e) {
        sendError(res, e.what());
    }
}

void uploadWithMapping(const httplib::Request& req, httplib::Response& res) {
    try {
        auto file = req.get_file_value("file");
        auto mappingPart = req.get_file_value("mapping");

        if (file.filename.empty()) { sendError(res, "No file uploaded", 400); return; }
        if (mappingPart.content.empty()) { sendError(res, "No mapping provided", 400); return; }

        Json::Value mappingJson = parseJson(mappingPart.content);
        ColumnMapping mapping = columnMappingFromJson(mappingJson);

        std::string ext = ".xlsx";
        auto pos = file.filename.rfind('.');
        if (pos != std::string::npos) ext = file.filename.substr(pos);

        std::string tmpPath = makeTempPath(ext);
        { std::ofstream out(tmpPath, std::ios::binary); out.write(file.content.data(), static_cast<std::streamsize>(file.content.size())); }

        UniversalParseRequest parseReq;
        parseReq.filePath = tmpPath;
        parseReq.mapping  = mapping;

        IModel& model = Model::getInstance();
        void* result = model.executeOperation(ModelOperation::PARSE_WITH_MAPPING, &parseReq, "");
        std::filesystem::remove(tmpPath);

        if (!result) { sendError(res, "Failed to parse courses with provided mapping", 500); return; }

        auto* courses = static_cast<std::vector<Course>*>(result);
        Json::Value arr(Json::arrayValue);
        for (const auto& c : *courses) arr.append(courseToJson(c));
        delete courses;

        sendJson(res, arr);
    } catch (const std::exception& e) {
        sendError(res, e.what());
    }
}

void listProfiles(const httplib::Request& /*req*/, httplib::Response& res) {
    try {
        IModel& model = Model::getInstance();
        void* result = model.executeOperation(ModelOperation::LIST_UNIVERSITY_PROFILES, nullptr, "");

        Json::Value arr(Json::arrayValue);
        if (result) {
            auto* profiles = static_cast<std::vector<UniversityProfile>*>(result);
            for (const auto& p : *profiles) arr.append(universityProfileToJson(p));
            delete profiles;
        }
        sendJson(res, arr);
    } catch (const std::exception& e) {
        sendError(res, e.what());
    }
}

void saveProfile(const httplib::Request& req, httplib::Response& res) {
    try {
        Json::Value body = parseJson(req.body);
        std::string name = body.get("name", "").asString();
        if (name.empty()) { sendError(res, "Missing name", 400); return; }

        ColumnMapping mapping = columnMappingFromJson(body.get("mapping", Json::Value()));
        mapping.universityName = name;

        UniversityProfile profile;
        profile.name        = name;
        profile.mappingJson = jsonToString(columnMappingToJson(mapping));

        IModel& model = Model::getInstance();
        void* result = model.executeOperation(ModelOperation::SAVE_UNIVERSITY_PROFILE, &profile, "");

        Json::Value resp;
        if (result) {
            auto* ok = static_cast<bool*>(result);
            resp["success"] = *ok;
            delete ok;
        } else {
            resp["success"] = false;
        }
        sendJson(res, resp);
    } catch (const std::exception& e) {
        sendError(res, e.what());
    }
}

void deleteProfile(const httplib::Request& req, httplib::Response& res) {
    try {
        int id = std::stoi(req.path_params.at("id"));
        IModel& model = Model::getInstance();
        void* result = model.executeOperation(ModelOperation::DELETE_UNIVERSITY_PROFILE, &id, "");

        Json::Value resp;
        if (result) {
            auto* ok = static_cast<bool*>(result);
            resp["success"] = *ok;
            delete ok;
        } else {
            resp["success"] = false;
        }
        sendJson(res, resp);
    } catch (const std::exception& e) {
        sendError(res, e.what());
    }
}

void fetchCoursesFromUrl(const httplib::Request& req, httplib::Response& res) {
    try {
        Json::Value body = parseJson(req.body);
        std::string url = body.get("url", "").asString();
        if (url.empty()) {
            sendError(res, "Missing url", 400);
            return;
        }

        // Detect extension from URL, default to .xlsx
        std::string ext = ".xlsx";
        auto qpos = url.find('?');
        std::string urlPath = (qpos != std::string::npos) ? url.substr(0, qpos) : url;
        auto dotpos = urlPath.rfind('.');
        if (dotpos != std::string::npos) {
            std::string candidate = urlPath.substr(dotpos);
            if (candidate == ".txt" || candidate == ".xlsx") ext = candidate;
        }

        std::string tmpPath = makeTempPath(ext);

        // Download with libcurl
        CURL* curl = curl_easy_init();
        if (!curl) { sendError(res, "curl_easy_init failed"); return; }

        FILE* fp = fopen(tmpPath.c_str(), "wb");
        if (!fp) { curl_easy_cleanup(curl); sendError(res, "Cannot open temp file"); return; }

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
        CURLcode rc = curl_easy_perform(curl);
        fclose(fp);
        curl_easy_cleanup(curl);

        if (rc != CURLE_OK) {
            std::filesystem::remove(tmpPath);
            sendError(res, std::string("Download failed: ") + curl_easy_strerror(rc), 502);
            return;
        }

        IModel& model = Model::getInstance();
        void* result = model.executeOperation(ModelOperation::GENERATE_COURSES, nullptr, tmpPath);
        std::filesystem::remove(tmpPath);

        if (!result) {
            sendError(res, "Failed to parse courses from downloaded file", 500);
            return;
        }

        auto* courses = static_cast<std::vector<Course>*>(result);
        Json::Value arr(Json::arrayValue);
        for (const auto& c : *courses) arr.append(courseToJson(c));
        delete courses;

        sendJson(res, arr);
    } catch (const std::exception& e) {
        sendError(res, e.what());
    }
}

void exportIcal(const httplib::Request& req, httplib::Response& res) {
    try {
        Json::Value body = parseJson(req.body);
        InformativeSchedule schedule = scheduleFromJson(body);

        // Map day name → iCal BYDAY value
        static const std::unordered_map<std::string, std::string> dayToIcal = {
            {"Sunday", "SU"}, {"Monday", "MO"}, {"Tuesday", "TU"},
            {"Wednesday", "WE"}, {"Thursday", "TH"}, {"Friday", "FR"}, {"Saturday", "SA"}
        };

        // Semester start dates (use as DTSTART reference — adjust as needed)
        static const std::unordered_map<std::string, std::string> semesterStart = {
            {"A", "20241027"}, {"B", "20250302"}, {"SUMMER", "20250706"}
        };

        // Map day name → days offset from Sunday for DTSTART calculation
        static const std::unordered_map<std::string, int> dayOffset = {
            {"Sunday", 0}, {"Monday", 1}, {"Tuesday", 2},
            {"Wednesday", 3}, {"Thursday", 4}, {"Friday", 5}, {"Saturday", 6}
        };

        std::string startDate = semesterStart.count(schedule.semester)
            ? semesterStart.at(schedule.semester) : "20241027";

        std::ostringstream ics;
        ics << "BEGIN:VCALENDAR\r\n"
            << "VERSION:2.0\r\n"
            << "PRODID:-//Schedulify//Schedule Export//EN\r\n"
            << "CALSCALE:GREGORIAN\r\n"
            << "METHOD:PUBLISH\r\n";

        int uidCounter = 0;
        for (const auto& day : schedule.week) {
            auto icDay = dayToIcal.find(day.day);
            auto offset = dayOffset.find(day.day);
            if (icDay == dayToIcal.end() || offset == dayOffset.end()) continue;

            // Calculate actual date for first occurrence
            // startDate is a Sunday; add offset days
            int yr = std::stoi(startDate.substr(0,4));
            int mo = std::stoi(startDate.substr(4,2));
            int dy = std::stoi(startDate.substr(6,2));
            // Simple date addition (handles month overflow via tm)
            struct tm t = {};
            t.tm_year = yr - 1900; t.tm_mon = mo - 1; t.tm_mday = dy + offset->second;
            mktime(&t);
            char dateBuf[9];
            strftime(dateBuf, sizeof(dateBuf), "%Y%m%d", &t);

            for (const auto& item : day.day_items) {
                // Convert "HH:MM" to "HHMMSS"
                auto toTime = [](const std::string& hhmm) -> std::string {
                    if (hhmm.size() >= 5)
                        return hhmm.substr(0,2) + hhmm.substr(3,2) + "00";
                    return "080000";
                };
                std::string dtStart = std::string(dateBuf) + "T" + toTime(item.start);
                std::string dtEnd   = std::string(dateBuf) + "T" + toTime(item.end);

                ics << "BEGIN:VEVENT\r\n"
                    << "UID:schedulify-" << schedule.index << "-" << (++uidCounter) << "@schedulify\r\n"
                    << "DTSTART:" << dtStart << "\r\n"
                    << "DTEND:" << dtEnd << "\r\n"
                    << "RRULE:FREQ=WEEKLY;BYDAY=" << icDay->second << "\r\n"
                    << "SUMMARY:" << item.courseName << " (" << item.type << ")\r\n"
                    << "DESCRIPTION:" << item.raw_id << " - " << item.type << "\r\n"
                    << "LOCATION:Building " << item.building << " Room " << item.room << "\r\n"
                    << "END:VEVENT\r\n";
            }
        }

        ics << "END:VCALENDAR\r\n";

        res.status = 200;
        res.set_header("Content-Disposition",
            "attachment; filename=\"schedule_" + std::to_string(schedule.index) + ".ics\"");
        res.set_content(ics.str(), "text/calendar");
    } catch (const std::exception& e) {
        sendError(res, e.what());
    }
}

void health(const httplib::Request& /*req*/, httplib::Response& res) {
    try {
        Json::Value resp;
        resp["status"] = "ok";
        resp["version"] = "1.0";
        sendJson(res, resp);
    } catch (const std::exception& e) {
        sendError(res, e.what());
    }
}

} // namespace Handlers
