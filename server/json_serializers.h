#pragma once
#include <json/json.h>
#include <string>
#include <vector>
#include <sstream>
#include "model_interfaces.h"
#include "db_entities.h"
#include "column_mapping.h"
#include <QString>
#include <QDateTime>

inline std::string sessionTypeToString(SessionType t) {
    switch(t) {
        case SessionType::LECTURE: return "LECTURE";
        case SessionType::TUTORIAL: return "TUTORIAL";
        case SessionType::LAB: return "LAB";
        case SessionType::BLOCK: return "BLOCK";
        case SessionType::DEPARTMENTAL_SESSION: return "DEPARTMENTAL_SESSION";
        case SessionType::REINFORCEMENT: return "REINFORCEMENT";
        case SessionType::GUIDANCE: return "GUIDANCE";
        case SessionType::OPTIONAL_COLLOQUIUM: return "OPTIONAL_COLLOQUIUM";
        case SessionType::REGISTRATION: return "REGISTRATION";
        case SessionType::THESIS: return "THESIS";
        case SessionType::PROJECT: return "PROJECT";
        default: return "UNSUPPORTED";
    }
}

inline SessionType sessionTypeFromString(const std::string& s) {
    if (s == "LECTURE") return SessionType::LECTURE;
    if (s == "TUTORIAL") return SessionType::TUTORIAL;
    if (s == "LAB") return SessionType::LAB;
    if (s == "BLOCK") return SessionType::BLOCK;
    if (s == "DEPARTMENTAL_SESSION") return SessionType::DEPARTMENTAL_SESSION;
    if (s == "REINFORCEMENT") return SessionType::REINFORCEMENT;
    if (s == "GUIDANCE") return SessionType::GUIDANCE;
    if (s == "OPTIONAL_COLLOQUIUM") return SessionType::OPTIONAL_COLLOQUIUM;
    if (s == "REGISTRATION") return SessionType::REGISTRATION;
    if (s == "THESIS") return SessionType::THESIS;
    if (s == "PROJECT") return SessionType::PROJECT;
    return SessionType::UNSUPPORTED;
}

inline Json::Value sessionToJson(const Session& s) {
    Json::Value j;
    j["day_of_week"] = s.day_of_week;
    j["start_time"] = s.start_time;
    j["end_time"] = s.end_time;
    j["building_number"] = s.building_number;
    j["room_number"] = s.room_number;
    return j;
}

inline Session sessionFromJson(const Json::Value& j) {
    Session s;
    s.day_of_week = j["day_of_week"].asInt();
    s.start_time = j["start_time"].asString();
    s.end_time = j["end_time"].asString();
    s.building_number = j["building_number"].asString();
    s.room_number = j["room_number"].asString();
    return s;
}

inline Json::Value groupToJson(const Group& g) {
    Json::Value j;
    j["type"] = sessionTypeToString(g.type);
    Json::Value sessions(Json::arrayValue);
    for (const auto& s : g.sessions) sessions.append(sessionToJson(s));
    j["sessions"] = sessions;
    return j;
}

inline Group groupFromJson(const Json::Value& j) {
    Group g;
    g.type = sessionTypeFromString(j["type"].asString());
    for (const auto& s : j["sessions"]) g.sessions.push_back(sessionFromJson(s));
    return g;
}

inline Json::Value groupVecToJson(const std::vector<Group>& groups) {
    Json::Value arr(Json::arrayValue);
    for (const auto& g : groups) arr.append(groupToJson(g));
    return arr;
}

inline std::vector<Group> groupVecFromJson(const Json::Value& arr) {
    std::vector<Group> groups;
    for (const auto& g : arr) groups.push_back(groupFromJson(g));
    return groups;
}

inline Json::Value courseToJson(const Course& c) {
    Json::Value j;
    j["id"] = c.id;
    j["semester"] = c.semester;
    j["raw_id"] = c.raw_id;
    j["name"] = c.name;
    j["teacher"] = c.teacher;
    j["lectures"] = groupVecToJson(c.Lectures);
    j["tutorials"] = groupVecToJson(c.Tirgulim);
    j["labs"] = groupVecToJson(c.labs);
    j["blocks"] = groupVecToJson(c.blocks);
    j["departmental_sessions"] = groupVecToJson(c.DepartmentalSessions);
    j["reinforcements"] = groupVecToJson(c.Reinforcements);
    j["guidance"] = groupVecToJson(c.Guidance);
    j["optional_colloquium"] = groupVecToJson(c.OptionalColloquium);
    j["registration"] = groupVecToJson(c.Registration);
    j["thesis"] = groupVecToJson(c.Thesis);
    j["project"] = groupVecToJson(c.Project);
    return j;
}

inline Course courseFromJson(const Json::Value& j) {
    Course c;
    c.id = j["id"].asInt();
    c.semester = j["semester"].asInt();
    c.raw_id = j["raw_id"].asString();
    c.name = j["name"].asString();
    c.teacher = j["teacher"].asString();
    c.Lectures = groupVecFromJson(j["lectures"]);
    c.Tirgulim = groupVecFromJson(j["tutorials"]);
    c.labs = groupVecFromJson(j["labs"]);
    c.blocks = groupVecFromJson(j["blocks"]);
    c.DepartmentalSessions = groupVecFromJson(j["departmental_sessions"]);
    c.Reinforcements = groupVecFromJson(j["reinforcements"]);
    c.Guidance = groupVecFromJson(j["guidance"]);
    c.OptionalColloquium = groupVecFromJson(j["optional_colloquium"]);
    c.Registration = groupVecFromJson(j["registration"]);
    c.Thesis = groupVecFromJson(j["thesis"]);
    c.Project = groupVecFromJson(j["project"]);
    return c;
}

inline Json::Value scheduleItemToJson(const ScheduleItem& item) {
    Json::Value j;
    j["courseName"] = item.courseName;
    j["raw_id"] = item.raw_id;
    j["type"] = item.type;
    j["start"] = item.start;
    j["end"] = item.end;
    j["building"] = item.building;
    j["room"] = item.room;
    return j;
}

inline Json::Value scheduleDayToJson(const ScheduleDay& day) {
    Json::Value j;
    j["day"] = day.day;
    Json::Value items(Json::arrayValue);
    for (const auto& item : day.day_items) items.append(scheduleItemToJson(item));
    j["day_items"] = items;
    return j;
}

inline Json::Value scheduleToJson(const InformativeSchedule& s) {
    Json::Value j;
    j["index"] = s.index;
    j["unique_id"] = s.unique_id;
    j["semester"] = s.semester;
    j["amount_days"] = s.amount_days;
    j["amount_gaps"] = s.amount_gaps;
    j["gaps_time"] = s.gaps_time;
    j["avg_start"] = s.avg_start;
    j["avg_end"] = s.avg_end;
    j["earliest_start"] = s.earliest_start;
    j["latest_end"] = s.latest_end;
    j["longest_gap"] = s.longest_gap;
    j["total_class_time"] = s.total_class_time;
    j["consecutive_days"] = s.consecutive_days;
    j["days_json"] = s.days_json;
    j["weekend_classes"] = s.weekend_classes;
    j["has_morning_classes"] = s.has_morning_classes;
    j["has_early_morning"] = s.has_early_morning;
    j["has_evening_classes"] = s.has_evening_classes;
    j["has_late_evening"] = s.has_late_evening;
    j["max_daily_hours"] = s.max_daily_hours;
    j["min_daily_hours"] = s.min_daily_hours;
    j["avg_daily_hours"] = s.avg_daily_hours;
    j["has_lunch_break"] = s.has_lunch_break;
    j["max_daily_gaps"] = s.max_daily_gaps;
    j["avg_gap_length"] = s.avg_gap_length;
    j["schedule_span"] = s.schedule_span;
    j["compactness_ratio"] = s.compactness_ratio;
    j["weekday_only"] = s.weekday_only;
    j["has_monday"] = s.has_monday;
    j["has_tuesday"] = s.has_tuesday;
    j["has_wednesday"] = s.has_wednesday;
    j["has_thursday"] = s.has_thursday;
    j["has_friday"] = s.has_friday;
    j["has_saturday"] = s.has_saturday;
    j["has_sunday"] = s.has_sunday;
    Json::Value week(Json::arrayValue);
    for (const auto& day : s.week) week.append(scheduleDayToJson(day));
    j["week"] = week;
    return j;
}

inline InformativeSchedule scheduleFromJson(const Json::Value& j) {
    InformativeSchedule s;
    s.index = j["index"].asInt();
    s.unique_id = j["unique_id"].asString();
    s.semester = j["semester"].asString();
    s.amount_days = j["amount_days"].asInt();
    s.amount_gaps = j["amount_gaps"].asInt();
    s.gaps_time = j["gaps_time"].asInt();
    s.avg_start = j["avg_start"].asInt();
    s.avg_end = j["avg_end"].asInt();
    s.earliest_start = j["earliest_start"].asInt();
    s.latest_end = j["latest_end"].asInt();
    s.longest_gap = j["longest_gap"].asInt();
    s.total_class_time = j["total_class_time"].asInt();
    s.consecutive_days = j["consecutive_days"].asInt();
    s.days_json = j["days_json"].asString();
    s.weekend_classes = j["weekend_classes"].asBool();
    s.has_morning_classes = j["has_morning_classes"].asBool();
    s.has_early_morning = j["has_early_morning"].asBool();
    s.has_evening_classes = j["has_evening_classes"].asBool();
    s.has_late_evening = j["has_late_evening"].asBool();
    s.max_daily_hours = j["max_daily_hours"].asInt();
    s.min_daily_hours = j["min_daily_hours"].asInt();
    s.avg_daily_hours = j["avg_daily_hours"].asInt();
    s.has_lunch_break = j["has_lunch_break"].asBool();
    s.max_daily_gaps = j["max_daily_gaps"].asInt();
    s.avg_gap_length = j["avg_gap_length"].asInt();
    s.schedule_span = j["schedule_span"].asInt();
    s.compactness_ratio = j["compactness_ratio"].asDouble();
    s.weekday_only = j["weekday_only"].asBool();
    s.has_monday = j["has_monday"].asBool();
    s.has_tuesday = j["has_tuesday"].asBool();
    s.has_wednesday = j["has_wednesday"].asBool();
    s.has_thursday = j["has_thursday"].asBool();
    s.has_friday = j["has_friday"].asBool();
    s.has_saturday = j["has_saturday"].asBool();
    s.has_sunday = j["has_sunday"].asBool();
    for (const auto& d : j["week"]) {
        ScheduleDay day;
        day.day = d["day"].asString();
        for (const auto& item : d["day_items"]) {
            ScheduleItem si;
            si.courseName = item["courseName"].asString();
            si.raw_id = item["raw_id"].asString();
            si.type = item["type"].asString();
            si.start = item["start"].asString();
            si.end = item["end"].asString();
            si.building = item["building"].asString();
            si.room = item["room"].asString();
            day.day_items.push_back(si);
        }
        s.week.push_back(day);
    }
    return s;
}

inline Json::Value fileEntityToJson(const FileEntity& f) {
    Json::Value j;
    j["id"] = f.id;
    j["file_name"] = f.file_name;
    j["file_type"] = f.file_type;
    j["upload_time"] = f.upload_time.toString(Qt::ISODate).toStdString();
    j["updated_at"] = f.updated_at.toString(Qt::ISODate).toStdString();
    return j;
}

inline FileLoadData fileLoadDataFromJson(const Json::Value& j) {
    FileLoadData d;
    for (const auto& id : j["file_ids"]) d.fileIds.push_back(id.asInt());
    d.operation_type = j.get("operation_type", "load").asString();
    return d;
}

inline BotQueryRequest botQueryRequestFromJson(const Json::Value& j) {
    BotQueryRequest req;
    req.userMessage = j["message"].asString();
    req.scheduleMetadata = j.get("schedule_metadata", "").asString();
    req.semester = j.get("semester", "A").asString();
    for (const auto& id : j["available_schedule_ids"]) req.availableScheduleIds.push_back(id.asInt());
    for (const auto& uid : j["available_unique_ids"]) req.availableUniqueIds.push_back(uid.asString());
    return req;
}

inline Json::Value botQueryResponseToJson(const BotQueryResponse& r) {
    Json::Value j;
    j["message"] = r.userMessage;
    j["sql_query"] = r.sqlQuery;
    j["is_filter_query"] = r.isFilterQuery;
    j["has_error"] = r.hasError;
    j["error_message"] = r.errorMessage;
    Json::Value ids(Json::arrayValue);
    for (int id : r.filteredScheduleIds) ids.append(id);
    j["filtered_ids"] = ids;
    Json::Value uids(Json::arrayValue);
    for (const auto& uid : r.filteredUniqueIds) uids.append(uid);
    j["filtered_unique_ids"] = uids;
    return j;
}

// ── ColumnMapping ↔ JSON ─────────────────────────────────────────────────────

inline Json::Value columnMappingToJson(const ColumnMapping& m) {
    Json::Value j;
    j["universityName"]  = m.universityName;
    j["colCourseName"]   = m.colCourseName;
    j["colCourseId"]     = m.colCourseId;
    j["colTeacher"]      = m.colTeacher;
    j["colDay"]          = m.colDay;
    j["colStartTime"]    = m.colStartTime;
    j["colEndTime"]      = m.colEndTime;
    j["colBuilding"]     = m.colBuilding;
    j["colRoom"]         = m.colRoom;
    j["colSessionType"]  = m.colSessionType;
    j["colSemester"]     = m.colSemester;
    j["headerRow"]       = m.headerRow;
    j["defaultSemester"] = m.defaultSemester;

    Json::Value stMap(Json::objectValue);
    for (const auto& [k, v] : m.sessionTypeMap)
        stMap[k] = sessionTypeToString(v);
    j["sessionTypeMap"] = stMap;

    Json::Value dayMap(Json::objectValue);
    for (const auto& [k, v] : m.dayMap)
        dayMap[k] = v;
    j["dayMap"] = dayMap;

    return j;
}

inline ColumnMapping columnMappingFromJson(const Json::Value& j) {
    ColumnMapping m;
    m.universityName  = j.get("universityName",  "").asString();
    m.colCourseName   = j.get("colCourseName",   -1).asInt();
    m.colCourseId     = j.get("colCourseId",     -1).asInt();
    m.colTeacher      = j.get("colTeacher",      -1).asInt();
    m.colDay          = j.get("colDay",          -1).asInt();
    m.colStartTime    = j.get("colStartTime",    -1).asInt();
    m.colEndTime      = j.get("colEndTime",      -1).asInt();
    m.colBuilding     = j.get("colBuilding",     -1).asInt();
    m.colRoom         = j.get("colRoom",         -1).asInt();
    m.colSessionType  = j.get("colSessionType",  -1).asInt();
    m.colSemester     = j.get("colSemester",     -1).asInt();
    m.headerRow       = j.get("headerRow",        1).asInt();
    m.defaultSemester = j.get("defaultSemester",  1).asInt();

    if (j.isMember("sessionTypeMap"))
        for (const auto& key : j["sessionTypeMap"].getMemberNames())
            m.sessionTypeMap[key] = sessionTypeFromString(j["sessionTypeMap"][key].asString());

    if (j.isMember("dayMap"))
        for (const auto& key : j["dayMap"].getMemberNames())
            m.dayMap[key] = j["dayMap"][key].asInt();

    return m;
}

inline std::string jsonToString(const Json::Value& val) {
    Json::StreamWriterBuilder builder;
    builder["indentation"] = "";
    return Json::writeString(builder, val);
}

inline Json::Value parseJson(const std::string& body) {
    Json::Value root;
    Json::CharReaderBuilder builder;
    std::string errs;
    std::istringstream ss(body);
    Json::parseFromStream(builder, ss, &root, &errs);
    return root;
}

inline Json::Value universityProfileToJson(const UniversityProfile& p) {
    Json::Value j;
    j["id"]         = p.id;
    j["name"]       = p.name;
    j["mapping"]    = parseJson(p.mappingJson);
    j["created_at"] = p.createdAt;
    j["updated_at"] = p.updatedAt;
    return j;
}
