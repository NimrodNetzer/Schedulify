#pragma once
#include "model_interfaces.h"
#include <string>
#include <map>

struct ColumnMapping {
    std::string universityName;

    // 0-based column indices (-1 = not mapped)
    int colCourseName     = -1;
    int colCourseId       = -1;
    int colTeacher        = -1;
    int colDay            = -1;
    int colStartTime      = -1;
    int colEndTime        = -1;
    int colBuilding       = -1;
    int colRoom           = -1;
    int colSessionType    = -1;
    int colSemester       = -1;   // optional — if -1, default semester is used
    int headerRow         = 1;    // 1-based row index of the header row (rows before are skipped)

    // default semester when colSemester == -1  (1=A, 2=B, 3=Summer, 4=Yearly)
    int defaultSemester   = 1;

    // Maps the university's session-type string (e.g. "Lecture", "שיעור") → SessionType
    std::map<std::string, SessionType> sessionTypeMap;

    // Maps the university's day strings (e.g. "Mon", "Monday", "ב'") → day_of_week (1=Sun..7=Sat)
    std::map<std::string, int> dayMap;
};

// Request struct for PARSE_WITH_MAPPING operation
struct UniversalParseRequest {
    std::string filePath;
    ColumnMapping mapping;
};
