#include "settings_controller.h"

SettingsController::SettingsController(QObject *parent)
    : ControllerManager(parent) {
    modelConnection = ModelAccess::getModel();
}

SettingsController::~SettingsController() {
    modelConnection = nullptr;
}

QString SettingsController::getSetting(const QString& key, const QString& defaultValue) {
    SettingRequest req(key.toStdString(), defaultValue.toStdString());
    auto* result = static_cast<std::string*>(
        modelConnection->executeOperation(ModelOperation::GET_SETTING, &req, "")
    );
    if (result) {
        QString val = QString::fromStdString(*result);
        delete result;
        return val;
    }
    return defaultValue;
}

bool SettingsController::setSetting(const QString& key, const QString& value) {
    SettingRequest req(key.toStdString(), value.toStdString());
    auto* result = static_cast<bool*>(
        modelConnection->executeOperation(ModelOperation::SET_SETTING, &req, "")
    );
    if (result) {
        bool ok = *result;
        delete result;
        if (ok) emit settingSaved(key);
        return ok;
    }
    return false;
}

QString SettingsController::getClaudeApiKey() {
    return getSetting("gemini_api_key", "");
}
