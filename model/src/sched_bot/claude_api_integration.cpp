#include "claude_api_integration.h"

struct APIResponse {
    string data;
    long response_code;
    bool success;

    APIResponse() : response_code(0), success(false) {}
};

BotQueryResponse ClaudeAPIClient::ActivateBot(const BotQueryRequest& request) {
    BotQueryResponse response;

    try {
        Logger::get().logInfo("ActivateBot: Processing bot query for semester: " + request.semester);

        auto& db = DatabaseManager::getInstance();
        if (!db.isConnected()) {
            Logger::get().logError("ActivateBot: Database not connected");
            response.hasError = true;
            response.errorMessage = "Database connection unavailable";
            return response;
        }

        // Create enhanced request with semester-specific metadata
        BotQueryRequest enhancedRequest = request;
        enhancedRequest.scheduleMetadata = db.schedules()->getSchedulesMetadataForBot();

        // Add semester info to metadata
        enhancedRequest.scheduleMetadata += "\n\nCURRENT SEMESTER FILTER: " + request.semester;
        enhancedRequest.scheduleMetadata += "\nNOTE: Only schedules from semester " + request.semester + " are available for filtering.";
        enhancedRequest.scheduleMetadata += "\nIMPORTANT: Always SELECT unique_id FROM schedule for filtering, not schedule_index.";

        // Try Claude API
        ClaudeAPIClient claudeClient;
        response = claudeClient.processScheduleQuery(enhancedRequest);

        // Handle rate limiting with fallback
        if (response.hasError &&
            (response.errorMessage.find("overloaded") != std::string::npos ||
             response.errorMessage.find("rate limit") != std::string::npos ||
             response.errorMessage.find("429") != std::string::npos ||
             response.errorMessage.find("529") != std::string::npos)) {

            Logger::get().logWarning("ActivateBot: Gemini API overloaded - using fallback");
            response = generateFallbackResponse(enhancedRequest);

            if (!response.hasError) {
                response.userMessage = "⚠️ Gemini API is currently busy, using simplified pattern matching.\n\n" + response.userMessage;
            }
        }

        if (response.hasError) {
            Logger::get().logError("ActivateBot: Gemini processing failed: " + response.errorMessage);
            return response;
        }

        // Execute SQL filter if needed with semester filtering
        if (response.isFilterQuery && !response.sqlQuery.empty()) {
            SQLValidator::ValidationResult validation = SQLValidator::validateScheduleQuery(response.sqlQuery);
            if (!validation.isValid) {
                Logger::get().logError("ActivateBot: Generated query failed validation: " + validation.errorMessage);
                response.hasError = true;
                response.errorMessage = "Generated query failed security validation: " + validation.errorMessage;
                return response;
            }

            // Ensure query returns unique_id and add semester filtering
            string semesterFilteredQuery = response.sqlQuery;

            // Replace schedule_index with unique_id if needed
            size_t pos = semesterFilteredQuery.find("schedule_index");
            if (pos != string::npos) {
                semesterFilteredQuery.replace(pos, 14, "unique_id");
            }

            // Convert to lowercase for parsing
            string lowerQuery = semesterFilteredQuery;
            std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), ::tolower);

            // Add semester filter to WHERE clause
            if (lowerQuery.find("where") != string::npos) {
                semesterFilteredQuery += " AND semester = ?";
            } else {
                semesterFilteredQuery += " WHERE semester = ?";
            }

            // Add semester to parameters
            vector<string> enhancedParameters = response.queryParameters;
            enhancedParameters.push_back(request.semester);

            Logger::get().logInfo("Executing semester-filtered query: " + semesterFilteredQuery);
            Logger::get().logInfo("Parameters: " + std::to_string(enhancedParameters.size()) + " total, last one is semester: " + request.semester);

            // Execute query to get unique_ids directly
            vector<string> matchingUniqueIds = db.schedules()->executeCustomQueryForUniqueIds(semesterFilteredQuery, enhancedParameters);

            // Convert available schedule IDs to unique IDs for filtering
            vector<string> availableUniqueIds;
            for (int scheduleIndex : request.availableScheduleIds) {
                string uniqueId = db.schedules()->getUniqueIdByScheduleIndex(scheduleIndex, request.semester);
                if (!uniqueId.empty()) {
                    availableUniqueIds.push_back(uniqueId);
                }
            }

            // Filter to only include available schedules
            vector<string> filteredUniqueIds;
            std::set<string> availableSet(availableUniqueIds.begin(), availableUniqueIds.end());

            for (const string& uniqueId : matchingUniqueIds) {
                if (availableSet.find(uniqueId) != availableSet.end()) {
                    filteredUniqueIds.push_back(uniqueId);
                }
            }

            // NEW: Store unique IDs as primary result
            response.filteredUniqueIds = filteredUniqueIds;

            // Convert back to schedule indices for backward compatibility
            vector<int> filteredIds = db.schedules()->getScheduleIndicesByUniqueIds(filteredUniqueIds);
            response.filteredScheduleIds = filteredIds;

            // Update response message
            if (filteredUniqueIds.empty()) {
                response.userMessage += "\n\n❌ No schedules match your criteria in semester " + request.semester + ".";
            } else {
                response.userMessage += "\n\n✅ Found " + std::to_string(filteredUniqueIds.size()) +
                                        " matching schedules in semester " + request.semester + ".";
            }
        }

        Logger::get().logInfo("ActivateBot: Successfully processed query for semester " + request.semester);

    } catch (const std::exception& e) {
        Logger::get().logError("ActivateBot: Exception processing query: " + std::string(e.what()));
        response.hasError = true;
        response.errorMessage = "An error occurred while processing your query: " + std::string(e.what());
        response.isFilterQuery = false;
    }

    return response;
}

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, APIResponse* response) {
    size_t totalSize = size * nmemb;
    response->data.append((char*)contents, totalSize);
    return totalSize;
}

ClaudeAPIClient::ClaudeAPIClient() {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    Logger::get().logInfo("Gemini API client initialized");
}

ClaudeAPIClient::~ClaudeAPIClient() {
    curl_global_cleanup();
}

BotQueryResponse ClaudeAPIClient::processScheduleQuery(const BotQueryRequest& request) {
    BotQueryResponse response;

    // Prefer key stored via the in-app Settings UI; fall back to env variable
    string cleanApiKey;
    auto& db = DatabaseManager::getInstance();
    if (db.isConnected()) {
        string dbKey = db.getMetadata("gemini_api_key", "");
        for (char c : dbKey) {
            if (c >= 33 && c <= 126) cleanApiKey += c;
        }
    }
    if (cleanApiKey.empty()) {
        const char* envKey = getenv("GEMINI_API_KEY");
        if (envKey && strlen(envKey) > 0) {
            for (char c : string(envKey)) {
                if (c >= 33 && c <= 126) cleanApiKey += c;
            }
        }
    }
    if (cleanApiKey.empty()) {
        Logger::get().logError("No Gemini API key configured. Set it in Settings or GEMINI_API_KEY env var.");
        response.hasError = true;
        response.errorMessage = "SchedBot requires a free Gemini API key. Please configure it in Settings (⚙️).";
        return response;
    }

    Logger::get().logInfo("Starting Gemini API request");

    try {
        // Build Gemini request body
        Json::Value requestJson = createRequestPayload(request);
        Json::StreamWriterBuilder builder;
        string jsonString = Json::writeString(builder, requestJson);

        string apiUrl = GEMINI_API_BASE + cleanApiKey;

        CURL* curl = curl_easy_init();
        if (!curl) {
            response.hasError = true;
            response.errorMessage = "Failed to initialize CURL";
            return response;
        }

        APIResponse apiResponse;

        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_URL, apiUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonString.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &apiResponse);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 15L);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);

        CURLcode res = curl_easy_perform(curl);
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &apiResponse.response_code);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK) {
            response.hasError = true;
            response.errorMessage = "Network error: " + string(curl_easy_strerror(res));
            return response;
        }

        if (apiResponse.response_code == 200) {
            response = parseGeminiResponse(apiResponse.data);
            return response;
        } else if (apiResponse.response_code == 429) {
            response.hasError = true;
            response.errorMessage = "Gemini API rate limit reached. Please wait a moment and try again.";
            return response;
        } else if (apiResponse.response_code == 400 || apiResponse.response_code == 403) {
            Logger::get().logError("Gemini API auth/request error (HTTP " + to_string(apiResponse.response_code) + "): " + apiResponse.data.substr(0, 300));
            response.hasError = true;
            response.errorMessage = "Invalid API key. Please check your Gemini API key in Settings (⚙️).";
            return response;
        } else {
            Logger::get().logError("Gemini API error HTTP " + to_string(apiResponse.response_code) + ": " + apiResponse.data.substr(0, 300));
            response.hasError = true;
            response.errorMessage = "Gemini API request failed (HTTP " + to_string(apiResponse.response_code) + ").";
            return response;
        }

    } catch (const exception& e) {
        Logger::get().logError("Exception in Gemini API request: " + string(e.what()));
        response.hasError = true;
        response.errorMessage = "Request processing error: " + string(e.what());
    }

    return response;
}

BotQueryResponse ClaudeAPIClient::generateFallbackResponse(const BotQueryRequest& request) {
    BotQueryResponse response;

    Logger::get().logInfo("Generating fallback response for Gemini API failure");

    string userMsg = request.userMessage;
    transform(userMsg.begin(), userMsg.end(), userMsg.begin(), ::tolower);

    // Simple pattern matching for common queries - UPDATED to use unique_id
    if (userMsg.find("start after") != string::npos || userMsg.find("begin after") != string::npos) {
        if (userMsg.find("10") != string::npos) {
            response.userMessage = "I understand you want schedules that start after 10:00 AM. I'll look for schedules where the earliest class begins after 10:00 AM.";
            response.sqlQuery = "SELECT unique_id FROM schedule WHERE earliest_start > ?";
            response.queryParameters = {"600"}; // 10:00 AM = 600 minutes
            response.isFilterQuery = true;
        } else if (userMsg.find("9") != string::npos) {
            response.userMessage = "I'll find schedules that start after 9:00 AM for you.";
            response.sqlQuery = "SELECT unique_id FROM schedule WHERE earliest_start > ?";
            response.queryParameters = {"540"}; // 9:00 AM = 540 minutes
            response.isFilterQuery = true;
        }
    } else if (userMsg.find("no early") != string::npos || userMsg.find("not early") != string::npos) {
        response.userMessage = "I'll find schedules with no early morning classes (before 8:30 AM).";
        response.sqlQuery = "SELECT unique_id FROM schedule WHERE has_early_morning = ?";
        response.queryParameters = {"0"};
        response.isFilterQuery = true;
    } else if (userMsg.find("no morning") != string::npos) {
        response.userMessage = "I'll find schedules with no morning classes (before 10:00 AM).";
        response.sqlQuery = "SELECT unique_id FROM schedule WHERE has_morning_classes = ?";
        response.queryParameters = {"0"};
        response.isFilterQuery = true;
    } else {
        // Generic fallback
        response.userMessage = "I'm currently experiencing high demand and cannot process complex queries. Please try a simpler request like 'no early morning classes' or 'start after 10 AM'.";
        response.isFilterQuery = false;
    }

    response.hasError = false;
    return response;
}

Json::Value ClaudeAPIClient::createRequestPayload(const BotQueryRequest& request) {
    Json::Value payload;

    // System instruction
    string systemPrompt = createSystemPrompt(request.scheduleMetadata);
    Json::Value sysPart;
    sysPart["text"] = systemPrompt;
    Json::Value sysParts(Json::arrayValue);
    sysParts.append(sysPart);
    payload["system_instruction"]["parts"] = sysParts;

    // User message
    Json::Value userPart;
    userPart["text"] = request.userMessage;
    Json::Value userParts(Json::arrayValue);
    userParts.append(userPart);
    Json::Value userContent;
    userContent["role"] = "user";
    userContent["parts"] = userParts;
    Json::Value contents(Json::arrayValue);
    contents.append(userContent);
    payload["contents"] = contents;

    payload["generationConfig"]["maxOutputTokens"] = 1024;

    return payload;
}

string ClaudeAPIClient::createSystemPrompt(const string& scheduleMetadata) {
    string prompt = R"(
You are SchedBot, an expert schedule filtering assistant. Your job is to analyze user requests and generate SQL queries to filter class schedules.

<schedule_data>
)" + scheduleMetadata + R"(
</schedule_data>

<comprehensive_column_reference>
FILTERABLE COLUMNS WITH DESCRIPTIONS:

CRITICAL: Always use unique_id for filtering, NOT schedule_index!

BASIC METRICS:
- unique_id: TEXT (unique identifier for each schedule - USE THIS FOR FILTERING)
- schedule_index: INTEGER (display number only - DO NOT USE for filtering)
- semester: TEXT (A, B, or SUMMER)
- amount_days: INTEGER (number of study days, 1-7)
- amount_gaps: INTEGER (total number of gaps between classes)
- gaps_time: INTEGER (total gap time in minutes)
- avg_start: INTEGER (average daily start time in minutes from midnight)
- avg_end: INTEGER (average daily end time in minutes from midnight)

TIME RANGE METRICS:
- earliest_start: INTEGER (earliest class start across all days, minutes from midnight)
- latest_end: INTEGER (latest class end across all days, minutes from midnight)
- longest_gap: INTEGER (longest single gap between classes in minutes)
- total_class_time: INTEGER (total minutes spent in actual classes)
- schedule_span: INTEGER (time from first to last class: latest_end - earliest_start)

DAY PATTERN METRICS:
- consecutive_days: INTEGER (longest streak of consecutive class days)
- weekend_classes: BOOLEAN (1 if has Saturday/Sunday classes, 0 if not)
- weekday_only: BOOLEAN (1 if only Monday-Friday, 0 if has weekends)

TIME PREFERENCE FLAGS (BOOLEAN: 1=true, 0=false):
- has_early_morning: BOOLEAN (classes before 8:30 AM / 510 minutes)
- has_morning_classes: BOOLEAN (classes before 10:00 AM / 600 minutes)
- has_evening_classes: BOOLEAN (classes after 6:00 PM / 1080 minutes)
- has_late_evening: BOOLEAN (classes after 8:00 PM / 1200 minutes)

DAILY INTENSITY METRICS:
- max_daily_hours: INTEGER (most hours of classes in any single day)
- min_daily_hours: INTEGER (fewest hours on days that have classes)
- avg_daily_hours: INTEGER (average hours per study day)

GAP AND BREAK PATTERNS:
- has_lunch_break: BOOLEAN (has gap between 12:00-14:00 PM / 720-840 minutes)
- max_daily_gaps: INTEGER (maximum number of gaps in any single day)
- avg_gap_length: INTEGER (average gap length when gaps exist)

EFFICIENCY METRICS:
- compactness_ratio: REAL (total_class_time / schedule_span, higher = more efficient)

SPECIFIC WEEKDAY FLAGS (BOOLEAN: 1=true, 0=false):
- has_monday, has_tuesday, has_wednesday, has_thursday, has_friday, has_saturday, has_sunday
</comprehensive_column_reference>

<time_conversion_quick_reference>
Common time conversions (minutes from midnight):
- 7:00 AM = 420    - 8:00 AM = 480    - 8:30 AM = 510    - 9:00 AM = 540
- 10:00 AM = 600   - 12:00 PM = 720   - 2:00 PM = 840    - 5:00 PM = 1020
- 6:00 PM = 1080   - 8:00 PM = 1200   - 9:00 PM = 1260   - 10:00 PM = 1320
</time_conversion_quick_reference>

<user_query_examples>
EXAMPLE QUERIES AND THEIR SQL (ALWAYS USE unique_id!):

"Find schedules with no early morning classes"
→ SELECT unique_id FROM schedule WHERE has_early_morning = 0

"Show me schedules that start after 9 AM"
→ SELECT unique_id FROM schedule WHERE earliest_start > 540

"I want schedules with maximum 4 study days and no gaps"
→ SELECT unique_id FROM schedule WHERE amount_days <= 4 AND amount_gaps = 0

"Find schedules ending before 5 PM"
→ SELECT unique_id FROM schedule WHERE latest_end <= 1020

"Show schedules with classes only on weekdays"
→ SELECT unique_id FROM schedule WHERE weekday_only = 1

"I want compact schedules with good efficiency"
→ SELECT unique_id FROM schedule WHERE compactness_ratio > 0.6

"Find schedules with a lunch break"
→ SELECT unique_id FROM schedule WHERE has_lunch_break = 1

"Show me schedules with no Friday classes"
→ SELECT unique_id FROM schedule WHERE has_friday = 0

"I want schedules with consecutive days but not too many"
→ SELECT unique_id FROM schedule WHERE consecutive_days >= 2 AND consecutive_days <= 4

"Find schedules with light daily workload"
→ SELECT unique_id FROM schedule WHERE max_daily_hours <= 6 AND avg_daily_hours <= 4
</user_query_examples>

<instructions>
When a user asks to filter schedules, you MUST respond in this EXACT format:

RESPONSE: [Your helpful explanation of what you're filtering for]
SQL: [The SQL query to execute]
PARAMETERS: [Comma-separated parameter values, or NONE]

For non-filtering questions, respond normally and set SQL to NONE.

CRITICAL RULES:
- ALWAYS SELECT unique_id FROM schedule WHERE [conditions]
- NEVER use schedule_index in SELECT statements
- Use ? for parameters, never hardcode values
- Use boolean columns efficiently (=1 for true, =0 for false)
- Combine multiple conditions with AND/OR as needed
- Consider user intent - "early" usually means has_early_morning or has_morning_classes
- For time ranges, use earliest_start/latest_end for global times, avg_start/avg_end for averages
</instructions>

<common_user_intents>
"early morning" → has_early_morning = 0 OR earliest_start > 540
"late evening" → has_evening_classes = 0 OR latest_end < 1080
"compact schedule" → compactness_ratio > 0.5 OR schedule_span < 480
"spread out" → consecutive_days <= 2 OR amount_days <= 3
"intensive days" → max_daily_hours >= 6
"light days" → max_daily_hours <= 4
"no gaps" → amount_gaps = 0
"minimal gaps" → amount_gaps <= 2
"weekdays only" → weekday_only = 1
"free weekends" → weekend_classes = 0
</common_user_intents>

Remember: You MUST follow the exact response format with RESPONSE:, SQL:, and PARAMETERS: labels.
CRITICAL: Always use unique_id in SELECT statements, never schedule_index!
)";

    return prompt;
}

BotQueryResponse ClaudeAPIClient::parseGeminiResponse(const string& responseData) {
    BotQueryResponse botResponse;

    if (responseData.empty()) {
        Logger::get().logError("Empty response from Gemini API");
        botResponse.hasError = true;
        botResponse.errorMessage = "Empty response from Gemini API";
        return botResponse;
    }

    try {
        Json::Reader reader;
        Json::Value root;

        if (!reader.parse(responseData, root)) {
            Logger::get().logError("Failed to parse Gemini JSON: " + reader.getFormattedErrorMessages());
            botResponse.hasError = true;
            botResponse.errorMessage = "Invalid JSON response from Gemini API";
            return botResponse;
        }

        if (root.isMember("error")) {
            Json::Value error = root["error"];
            string errorMessage = error.isMember("message") ? error["message"].asString() : "Unknown error";
            Logger::get().logError("Gemini API error: " + errorMessage);
            botResponse.hasError = true;
            botResponse.errorMessage = errorMessage;
            return botResponse;
        }

        // Gemini response: candidates[0].content.parts[0].text
        if (!root.isMember("candidates") || !root["candidates"].isArray() || root["candidates"].empty()) {
            Logger::get().logError("Invalid candidates structure in Gemini response");
            botResponse.hasError = true;
            botResponse.errorMessage = "Invalid response format from Gemini API";
            return botResponse;
        }

        Json::Value candidate = root["candidates"][0];
        if (!candidate.isMember("content") || !candidate["content"].isMember("parts") ||
            !candidate["content"]["parts"].isArray() || candidate["content"]["parts"].empty()) {
            Logger::get().logError("No content parts in Gemini response");
            botResponse.hasError = true;
            botResponse.errorMessage = "No text content in Gemini API response";
            return botResponse;
        }

        string contentText = candidate["content"]["parts"][0]["text"].asString();
        if (contentText.empty()) {
            Logger::get().logError("Empty text content from Gemini");
            botResponse.hasError = true;
            botResponse.errorMessage = "Empty text content from Gemini API";
            return botResponse;
        }

        // Parse SQL query (minimal logging)
        string sqlQuery;
        vector<string> parameters;
        bool hasSql = extractSQLQuery(contentText, sqlQuery, parameters);

        if (hasSql) {
            botResponse.isFilterQuery = true;
            botResponse.sqlQuery = sqlQuery;
            botResponse.queryParameters = parameters;
            Logger::get().logInfo("sqlQuery: " + sqlQuery);

            if (parameters.empty()) {
                Logger::get().logInfo("Query Parameters: None");
            } else {
                Logger::get().logInfo("Query Parameters (" + std::to_string(parameters.size()) + " total):");
                for (size_t i = 0; i < parameters.size(); ++i) {
                    Logger::get().logInfo("  [" + std::to_string(i) + "]: " + parameters[i]);
                }
            }
        } else {
            botResponse.isFilterQuery = false;
        }

        // Extract user message
        auto trim = [](string str) {
            str.erase(0, str.find_first_not_of(" \t\n\r"));
            str.erase(str.find_last_not_of(" \t\n\r") + 1);
            return str;
        };

        string lowerContent = contentText;
        transform(lowerContent.begin(), lowerContent.end(), lowerContent.begin(), ::tolower);
        size_t responsePos = lowerContent.find("response:");

        if (responsePos != string::npos) {
            size_t responseStartPos = responsePos + 9;
            size_t responseEndPos = lowerContent.find("sql:", responseStartPos);
            if (responseEndPos == string::npos) {
                responseEndPos = contentText.length();
            }
            string responseContent = contentText.substr(responseStartPos, responseEndPos - responseStartPos);
            botResponse.userMessage = trim(responseContent);
        } else {
            botResponse.userMessage = contentText;
        }

        if (botResponse.userMessage.empty()) {
            Logger::get().logError("Empty message extracted from Gemini response");
            botResponse.hasError = true;
            botResponse.errorMessage = "Empty message extracted from Gemini response";
            return botResponse;
        }

    } catch (const exception& e) {
        Logger::get().logError("Exception parsing Gemini response: " + string(e.what()));
        botResponse.hasError = true;
        botResponse.errorMessage = "Failed to parse Gemini response: " + string(e.what());
    }

    return botResponse;
}

bool ClaudeAPIClient::extractSQLQuery(const string& content, string& sqlQuery, vector<string>& parameters) {
    auto trim = [](string str) {
        str.erase(0, str.find_first_not_of(" \t\n\r"));
        str.erase(str.find_last_not_of(" \t\n\r") + 1);
        return str;
    };

    string lowerContent = content;
    transform(lowerContent.begin(), lowerContent.end(), lowerContent.begin(), ::tolower);

    size_t sqlPos = lowerContent.find("sql:");
    if (sqlPos == string::npos) {
        return false;
    }

    size_t sqlStartPos = sqlPos + 4;
    size_t sqlEndPos = lowerContent.find("parameters:", sqlStartPos);
    if (sqlEndPos == string::npos) {
        sqlEndPos = content.length();
    }

    string rawSql = content.substr(sqlStartPos, sqlEndPos - sqlStartPos);
    rawSql = trim(rawSql);

    string lowerSql = rawSql;
    transform(lowerSql.begin(), lowerSql.end(), lowerSql.begin(), ::tolower);
    if (lowerSql == "none" || rawSql.empty()) {
        return false;
    }

    sqlQuery = rawSql;

    // Extract parameters
    parameters.clear();
    size_t paramPos = lowerContent.find("parameters:");
    if (paramPos != string::npos) {
        size_t paramStartPos = paramPos + 11;
        size_t paramEndPos = content.find('\n', paramStartPos);
        if (paramEndPos == string::npos) {
            paramEndPos = content.length();
        }

        string rawParams = content.substr(paramStartPos, paramEndPos - paramStartPos);
        rawParams = trim(rawParams);

        string lowerParams = rawParams;
        transform(lowerParams.begin(), lowerParams.end(), lowerParams.begin(), ::tolower);
        if (lowerParams != "none" && !rawParams.empty()) {
            stringstream ss(rawParams);
            string param;
            while (getline(ss, param, ',')) {
                param = trim(param);
                if (!param.empty()) {
                    parameters.push_back(param);
                }
            }
        }
    }

    return true;
}