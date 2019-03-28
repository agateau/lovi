#include "config.h"

#include "equalcondition.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

using std::optional;

optional<Config> Config::fromJsonDocument(const QJsonDocument &doc) {
    auto regex = doc.object().value("parser").toObject().value("regex").toString();
    if (regex.isEmpty()) {
        qWarning() << "No regex found";
        return {};
    }

    Config config;
    config.parser.setPattern(regex);
    if (!config.parser.isValid()) {
        qWarning() << "Invalid regex";
        return {};
    }
    config.parser.optimize();

    QHash<QString, int> columnByName;
    {
        int role = 0;
        for (const auto& name : config.parser.namedCaptureGroups()) {
            if (!name.isEmpty()) {
                columnByName[name] = role++;
            }
        }
    }

    for (QJsonValue jsonValue : doc.object().value("highlights").toArray()) {
        QJsonObject highlightObj = jsonValue.toObject();
        auto conditionObj = highlightObj.value("condition").toObject();
        auto columnName = conditionObj.value("column").toString();
        auto value = conditionObj.value("value").toString();
        auto bgColor = highlightObj.value("bgColor").toString();

        auto it = columnByName.find(columnName);
        if (it == columnByName.end()) {
            qWarning() << "No column named" << columnName;
            return {};
        }
        Highlight highlight;
        highlight.condition = std::make_unique<EqualCondition>(it.value(), value);
        highlight.bgColor = bgColor;
        config.highlights.push_back(std::move(highlight));
    }

    return std::move(config);
}
