#ifndef PRINT_SCHEDULE_H
#define PRINT_SCHEDULE_H

#include "model_interfaces.h"
#include "logger.h"

#include <QString>
#ifndef SCHEDULIFY_SERVER_BUILD
#include <QFileDialog>
#include <QPrinter>
#include <QPainter>
#include <QTextDocument>
#include <QPrintDialog>
#include <QPageSize>
#include <QPageLayout>
#include <QDesktopServices>
#endif
#include <QDateTime>

// Function to print a schedule directly to a printer
bool printSelectedSchedule(const InformativeSchedule& schedule);

#endif // PRINT_SCHEDULE_H