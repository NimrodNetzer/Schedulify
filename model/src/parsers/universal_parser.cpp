#include "universal_parser.h"
#include "logger.h"
#include <OpenXLSX.hpp>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <stdexcept>

// ── helpers ──────────────────────────────────────────────────────────────────

static std::string trim(const std::string& s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == std::string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

// Read a single cell from an OpenXLSX worksheet as a string
static std::string cellStr(OpenXLSX::XLWorksheet& ws, int row, int col) {
    try {
        auto cell = ws.cell(row, col + 1); // OpenXLSX is 1-based; col is 0-based
        return trim(cell.value().get<std::string>());
    } catch (...) {
        return "";
    }
}

// Read a row from a CSV/txt line (comma-separated)
static std::vector<std::string> splitCsv(const std::string& line) {
    std::vector<std::string> cols;
    std::string cur;
    bool inQuote = false;
    for (char c : line) {
        if (c == '"') { inQuote = !inQuote; }
        else if (c == ',' && !inQuote) { cols.push_back(trim(cur)); cur.clear(); }
        else { cur += c; }
    }
    cols.push_back(trim(cur));
    return cols;
}

static SessionType resolveSessionType(const std::string& raw,
                                      const std::map<std::string, SessionType>& sessionTypeMap) {
    if (sessionTypeMap.empty()) return SessionType::LECTURE;
    auto it = sessionTypeMap.find(raw);
    if (it != sessionTypeMap.end()) return it->second;
    // Case-insensitive fallback
    std::string lower = raw;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    for (const auto& [key, val] : sessionTypeMap) {
        std::string kl = key;
        std::transform(kl.begin(), kl.end(), kl.begin(), ::tolower);
        if (kl == lower) return val;
    }
    return SessionType::LECTURE;
}

static int resolveDay(const std::string& raw, const std::map<std::string, int>& dayMap) {
    if (dayMap.empty()) return 1;
    auto it = dayMap.find(raw);
    if (it != dayMap.end()) return it->second;
    std::string lower = raw;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    for (const auto& [key, val] : dayMap) {
        std::string kl = key;
        std::transform(kl.begin(), kl.end(), kl.begin(), ::tolower);
        if (kl == lower) return val;
    }
    return 1;
}

static int resolveSemester(const std::string& raw) {
    std::string lower = raw;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    if (lower.find('b') != std::string::npos || lower.find("2") != std::string::npos) return 2;
    if (lower.find("sum") != std::string::npos || lower.find("3") != std::string::npos) return 3;
    if (lower.find("year") != std::string::npos || lower.find("4") != std::string::npos) return 4;
    return 1;
}

// ── core builder ─────────────────────────────────────────────────────────────

static std::vector<Course> buildCourses(
    const std::vector<std::vector<std::string>>& rows,
    const ColumnMapping& mapping)
{
    std::map<std::string, Course> courseMap;
    int idCounter = 1;

    auto getCol = [](const std::vector<std::string>& row, int col) -> std::string {
        if (col < 0 || col >= static_cast<int>(row.size())) return "";
        return row[col];
    };

    for (const auto& row : rows) {
        std::string courseId  = getCol(row, mapping.colCourseId);
        std::string name      = getCol(row, mapping.colCourseName);
        std::string teacher   = getCol(row, mapping.colTeacher);
        std::string dayStr    = getCol(row, mapping.colDay);
        std::string startStr  = getCol(row, mapping.colStartTime);
        std::string endStr    = getCol(row, mapping.colEndTime);
        std::string building  = getCol(row, mapping.colBuilding);
        std::string room      = getCol(row, mapping.colRoom);
        std::string typeStr   = getCol(row, mapping.colSessionType);
        std::string semStr    = (mapping.colSemester >= 0) ? getCol(row, mapping.colSemester) : "";

        if (courseId.empty() && name.empty()) continue;

        int semester = semStr.empty()
            ? mapping.defaultSemester
            : resolveSemester(semStr);

        std::string key = courseId + "_sem" + std::to_string(semester);

        if (courseMap.find(key) == courseMap.end()) {
            Course c;
            c.id        = idCounter++;
            c.raw_id    = courseId;
            c.name      = name;
            c.teacher   = teacher;
            c.semester  = semester;
            courseMap[key] = c;
        }

        Course& course = courseMap[key];
        // Update teacher/name if blank
        if (course.teacher.empty()) course.teacher = teacher;
        if (course.name.empty())    course.name    = name;

        // Build session
        Session session;
        session.day_of_week     = resolveDay(dayStr, mapping.dayMap);
        session.start_time      = startStr;
        session.end_time        = endStr;
        session.building_number = building;
        session.room_number     = room;

        SessionType stype = resolveSessionType(typeStr, mapping.sessionTypeMap);

        // Find or create a group of this type
        auto addToGroup = [&](std::vector<Group>& groups) {
            if (groups.empty()) {
                Group g;
                g.type = stype;
                g.sessions.push_back(session);
                groups.push_back(g);
            } else {
                groups[0].sessions.push_back(session);
            }
        };

        switch (stype) {
            case SessionType::LECTURE:              addToGroup(course.Lectures); break;
            case SessionType::TUTORIAL:             addToGroup(course.Tirgulim); break;
            case SessionType::LAB:                  addToGroup(course.labs); break;
            case SessionType::BLOCK:                addToGroup(course.blocks); break;
            case SessionType::DEPARTMENTAL_SESSION: addToGroup(course.DepartmentalSessions); break;
            case SessionType::REINFORCEMENT:        addToGroup(course.Reinforcements); break;
            case SessionType::GUIDANCE:             addToGroup(course.Guidance); break;
            case SessionType::OPTIONAL_COLLOQUIUM:  addToGroup(course.OptionalColloquium); break;
            case SessionType::REGISTRATION:         addToGroup(course.Registration); break;
            case SessionType::THESIS:               addToGroup(course.Thesis); break;
            case SessionType::PROJECT:              addToGroup(course.Project); break;
            default:                                addToGroup(course.Lectures); break;
        }
    }

    std::vector<Course> result;
    result.reserve(courseMap.size());
    for (auto& [k, c] : courseMap) result.push_back(std::move(c));
    return result;
}

// ── xlsx reader ──────────────────────────────────────────────────────────────

static std::vector<std::vector<std::string>> readXlsx(const std::string& path, int startRow, int maxRows = 0) {
    std::vector<std::vector<std::string>> rows;
    try {
        OpenXLSX::XLDocument doc;
        doc.open(path);
        auto ws = doc.workbook().worksheet(doc.workbook().worksheetNames()[0]);

        int lastRow = ws.rowCount();
        int lastCol = ws.columnCount();

        for (int r = startRow; r <= lastRow; ++r) {
            if (maxRows > 0 && static_cast<int>(rows.size()) >= maxRows) break;
            std::vector<std::string> row;
            for (int c = 0; c < lastCol; ++c) {
                row.push_back(cellStr(ws, r, c));
            }
            // Skip completely empty rows
            bool empty = true;
            for (const auto& cell : row) if (!cell.empty()) { empty = false; break; }
            if (!empty) rows.push_back(row);
        }
        doc.close();
    } catch (const std::exception& e) {
        Logger::get().logError("universal_parser xlsx error: " + std::string(e.what()));
    }
    return rows;
}

// ── txt/csv reader ───────────────────────────────────────────────────────────

static std::vector<std::vector<std::string>> readTxt(const std::string& path, int startRow, int maxRows = 0) {
    std::vector<std::vector<std::string>> rows;
    std::ifstream f(path);
    if (!f.is_open()) return rows;

    std::string line;
    int lineNo = 0;
    while (std::getline(f, line)) {
        ++lineNo;
        if (lineNo <= startRow) continue; // skip header rows
        if (maxRows > 0 && static_cast<int>(rows.size()) >= maxRows) break;
        if (trim(line).empty()) continue;
        rows.push_back(splitCsv(line));
    }
    return rows;
}

// ── public API ───────────────────────────────────────────────────────────────

std::vector<Course> parseWithMapping(const std::string& filePath, const ColumnMapping& mapping) {
    bool isXlsx = filePath.size() >= 5 &&
                  filePath.substr(filePath.size() - 5) == ".xlsx";

    int dataStartRow = mapping.headerRow + 1; // rows before and including header are skipped

    std::vector<std::vector<std::string>> rows = isXlsx
        ? readXlsx(filePath, dataStartRow)
        : readTxt(filePath, mapping.headerRow);

    if (rows.empty()) {
        Logger::get().logWarning("universal_parser: no data rows found in " + filePath);
        return {};
    }

    Logger::get().logInfo("universal_parser: " + std::to_string(rows.size()) +
                          " rows read from " + filePath);

    return buildCourses(rows, mapping);
}

std::vector<std::vector<std::string>> previewFile(const std::string& filePath, int maxRows) {
    bool isXlsx = filePath.size() >= 5 &&
                  filePath.substr(filePath.size() - 5) == ".xlsx";

    // Return the first maxRows rows starting from row 1 (including header)
    return isXlsx
        ? readXlsx(filePath, 1, maxRows)
        : readTxt(filePath, 0, maxRows);
}
