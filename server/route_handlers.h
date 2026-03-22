#pragma once
#include <httplib.h>

namespace Handlers {
    void uploadCourses(const httplib::Request&, httplib::Response&);
    void loadCoursesFromHistory(const httplib::Request&, httplib::Response&);
    void validateCourses(const httplib::Request&, httplib::Response&);
    void getFileHistory(const httplib::Request&, httplib::Response&);
    void deleteFile(const httplib::Request&, httplib::Response&);
    void generateSchedules(const httplib::Request&, httplib::Response&);
    void cleanSchedules(const httplib::Request&, httplib::Response&);
    void exportCsv(const httplib::Request&, httplib::Response&);
    void getFilteredIds(const httplib::Request&, httplib::Response&);
    void getFilteredUniqueIds(const httplib::Request&, httplib::Response&);
    void convertToIndices(const httplib::Request&, httplib::Response&);
    void convertToUniqueIds(const httplib::Request&, httplib::Response&);
    void botQuery(const httplib::Request&, httplib::Response&);
    void getSetting(const httplib::Request&, httplib::Response&);
    void setSetting(const httplib::Request&, httplib::Response&);
    void health(const httplib::Request&, httplib::Response&);
    void exportIcal(const httplib::Request&, httplib::Response&);
    void fetchCoursesFromUrl(const httplib::Request&, httplib::Response&);
    // Universal parser / university profiles
    void previewUpload(const httplib::Request&, httplib::Response&);
    void uploadWithMapping(const httplib::Request&, httplib::Response&);
    void listProfiles(const httplib::Request&, httplib::Response&);
    void saveProfile(const httplib::Request&, httplib::Response&);
    void deleteProfile(const httplib::Request&, httplib::Response&);
}
