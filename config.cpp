#include "config.h"

#include "conditions.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

using std::unique_ptr;

unique_ptr<Config> Config::fromJsonDocument(const QJsonDocument &doc) {
    auto regex = doc.object().value("parser").toObject().value("regex").toString();
    if (regex.isEmpty()) {
        qWarning() << "No regex found";
        return {};
    }

    unique_ptr<Config> config = std::make_unique<Config>();
    config->parser.setPattern(regex);
    if (!config->parser.isValid()) {
        qWarning() << "Invalid regex";
        return {};
    }
    config->parser.optimize();

    QHash<QString, int> columnByName;
    {
        int role = 0;
        for (const auto& name : config->parser.namedCaptureGroups()) {
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
        auto fgColor = highlightObj.value("fgColor").toString();

        auto it = columnByName.find(columnName);
        if (it == columnByName.end()) {
            qWarning() << "No column named" << columnName;
            return {};
        }
        Highlight highlight;
        highlight.condition = std::make_unique<EqualCondition>(it.value(), value);
        highlight.bgColor = bgColor;
        highlight.fgColor = fgColor;
        config->highlights.push_back(std::move(highlight));
    }

    return config;
}
