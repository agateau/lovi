#ifndef LOGFORMAT_H
#define LOGFORMAT_H

#include "highlight.h"

#include <QRegularExpression>

#include <memory>
#include <vector>

class QJsonDocument;

class LogFormat {
public:
    QRegularExpression parser;
    std::vector<Highlight> highlights;

    static std::unique_ptr<LogFormat> fromJsonDocument(const QJsonDocument& doc);
    static std::unique_ptr<LogFormat> createEmpty();
};

#endif // LOGFORMAT_H
