#ifndef CONFIG_H
#define CONFIG_H

#include "highlight.h"

#include <QRegularExpression>

#include <memory>
#include <vector>

class QJsonDocument;

class Config {
public:
    QRegularExpression parser;
    std::vector<Highlight> highlights;

    static std::unique_ptr<Config> fromJsonDocument(const QJsonDocument& doc);
    static std::unique_ptr<Config> createEmptyConfig();
};


#endif // CONFIG_H
