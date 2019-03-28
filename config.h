#ifndef CONFIG_H
#define CONFIG_H

#include "highlight.h"

#include <QRegularExpression>

#include <vector>

class QJsonDocument;

class Config {
public:
    QRegularExpression parser;
    std::vector<Highlight> highlights;

    static std::optional<Config> fromJsonDocument(const QJsonDocument& doc);
};


#endif // CONFIG_H
