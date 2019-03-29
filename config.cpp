#include "config.h"

#include "conditions.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

using std::unique_ptr;

static unique_ptr<Condition> createCondition(int column, const QString& value, const QString& op) {
    if (op == "exact") {
        return std::make_unique<ExactCondition>(column, value);
    } else if (op.isEmpty() || op == "contains") {
        return std::make_unique<ContainsCondition>(column, value);
    } else if (op == "regex") {
        QRegularExpression regex(value);
        if (!regex.isValid()) {
            qWarning() << value << "is not a valid regex:" << regex.errorString();
            return nullptr;
        }
        return std::make_unique<RegExCondition>(column, regex);
    } else {
        qWarning() << "Invalid value for 'op':" << op;
        return nullptr;
    }
}

unique_ptr<Config> Config::fromJsonDocument(const QJsonDocument &doc) {
    auto regex = doc.object().value("parser").toObject().value("regex").toString();
    if (regex.isEmpty()) {
        qWarning() << "No regex found";
        return {};
    }

    unique_ptr<Config> config = std::make_unique<Config>();
    config->parser.setPattern(regex);
    if (!config->parser.isValid()) {
        qWarning() << "Invalid parser regex:" << config->parser.errorString();
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
        auto op = conditionObj.value("op").toString();
        auto bgColor = highlightObj.value("bgColor").toString();
        auto fgColor = highlightObj.value("fgColor").toString();

        auto it = columnByName.find(columnName);
        if (it == columnByName.end()) {
            qWarning() << "No column named" << columnName;
            return {};
        }
        Highlight highlight;
        highlight.condition = createCondition(it.value(), value, op);
        if (!highlight.condition) {
            return {};
        }
        highlight.bgColor = bgColor;
        highlight.fgColor = fgColor;
        config->highlights.push_back(std::move(highlight));
    }

    return config;
}
