#pragma once
#include "column_mapping.h"
#include "model_interfaces.h"
#include <vector>
#include <string>

// Parses an .xlsx or .txt file using a user-supplied ColumnMapping.
// Returns the parsed courses or an empty vector on failure.
std::vector<Course> parseWithMapping(const std::string& filePath, const ColumnMapping& mapping);

// Returns the first `maxRows` data rows (after the header) as raw string columns.
// Used by the /api/courses/upload/preview endpoint to show the user what the file looks like.
std::vector<std::vector<std::string>> previewFile(const std::string& filePath, int maxRows = 5);
