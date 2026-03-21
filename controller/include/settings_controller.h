#ifndef SETTINGS_CONTROLLER_H
#define SETTINGS_CONTROLLER_H

#include "controller_manager.h"
#include "model_access.h"
#include "model_interfaces.h"
#include <QString>

class SettingsController : public ControllerManager {
    Q_OBJECT

public:
    explicit SettingsController(QObject *parent = nullptr);
    ~SettingsController() override;

    Q_INVOKABLE QString getSetting(const QString& key, const QString& defaultValue = "");
    Q_INVOKABLE bool setSetting(const QString& key, const QString& value);

    // Convenience accessors used by other controllers
    QString getClaudeApiKey();

signals:
    void settingSaved(const QString& key);

private:
    IModel* modelConnection;
};

#endif // SETTINGS_CONTROLLER_H
