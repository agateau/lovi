/*
 * Copyright 2019 Aurélien Gâteau <mail@agateau.com>
 *
 * This file is part of Lovi.
 *
 * Lovi is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "logformatio.h"

#include "conditionio.h"
#include "conditions.h"
#include "logformat.h"

#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QSaveFile>
#include <QStandardPaths>

using std::optional;
using std::shared_ptr;
using std::unique_ptr;

static optional<QByteArray> readFile(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qCritical() << "Could not open" << filePath << file.errorString();
        return {};
    }
    return file.readAll();
}

static unique_ptr<Condition> createCondition(const QString& conditionString,
                                             const QHash<QString, int>& columnByName) {
    QStringList tokens;
    {
        optional<QStringList> maybeTokens = ConditionIO::tokenize(conditionString);
        if (!maybeTokens.has_value()) {
            return {};
        }
        tokens = maybeTokens.value();
    }
    if (tokens.length() != 3) {
        qWarning() << "Wrong number of tokens in" << conditionString;
        return {};
    }

    auto columnName = tokens.at(0);
    auto op = tokens.at(1);
    auto value = tokens.at(2);

    auto it = columnByName.find(columnName);
    if (it == columnByName.end()) {
        qWarning() << "No column named" << columnName;
        return {};
    }
    int column = *it;

    if (op == "==") {
        return std::make_unique<ExactCondition>(column, value);
    } else if (op == "contains") {
        return std::make_unique<ContainsCondition>(column, value);
    } else if (op == "~") {
        QRegularExpression regex(value);
        if (!regex.isValid()) {
            qWarning() << value << "is not a valid regex:" << regex.errorString();
            return nullptr;
        }
        return std::make_unique<RegExCondition>(column, regex);
    } else {
        qWarning() << "Invalid operator:" << op;
        return nullptr;
    }
}

static std::optional<HighlightColor> initColor(const QString& text) {
    if (text.isEmpty()) {
        return {};
    }
    return HighlightColor(text);
}

static shared_ptr<LogFormat> loadLogFormat(const QJsonDocument& doc) {
    auto regex = doc.object().value("parser").toObject().value("regex").toString();
    if (regex.isEmpty()) {
        qWarning() << "No regex found";
        return {};
    }

    shared_ptr<LogFormat> logFormat = std::make_shared<LogFormat>();
    logFormat->parser.setPattern(regex);
    if (!logFormat->parser.isValid()) {
        qWarning() << "Invalid parser regex:" << logFormat->parser.errorString();
        return {};
    }
    logFormat->parser.optimize();

    QHash<QString, int> columnByName;
    {
        int role = 0;
        for (const auto& name : logFormat->parser.namedCaptureGroups()) {
            if (!name.isEmpty()) {
                columnByName[name] = role++;
            }
        }
    }

    for (QJsonValue jsonValue : doc.object().value("highlights").toArray()) {
        QJsonObject highlightObj = jsonValue.toObject();

        Highlight highlight;
        highlight.conditionDefinition = highlightObj.value("condition").toString();

        highlight.condition = createCondition(highlight.conditionDefinition, columnByName);

        auto rowBgColor = highlightObj.value("rowBgColor").toString();
        auto rowFgColor = highlightObj.value("rowFgColor").toString();
        auto bgColor = highlightObj.value("bgColor").toString();
        auto fgColor = highlightObj.value("fgColor").toString();
        highlight.rowBgColor = initColor(rowBgColor);
        highlight.rowFgColor = initColor(rowFgColor);
        highlight.bgColor = initColor(bgColor);
        highlight.fgColor = initColor(fgColor);
        logFormat->highlights.push_back(std::move(highlight));
    }

    return logFormat;
}

//- Save --------------------------------
static void
saveColor(QJsonObject* root, const QString& key, const optional<HighlightColor>& color) {
    if (!color.has_value()) {
        return;
    }
    root->insert(key, color.value().toString());
}

static QJsonObject saveHighlight(const Highlight& highlight) {
    QJsonObject root;
    root["condition"] = highlight.conditionDefinition;
    saveColor(&root, "rowBgColor", highlight.rowBgColor);
    saveColor(&root, "rowFgColor", highlight.rowFgColor);
    saveColor(&root, "bgColor", highlight.bgColor);
    saveColor(&root, "fgColor", highlight.fgColor);
    return root;
}

static QJsonDocument saveToJson(const shared_ptr<LogFormat>& logFormat) {
    QJsonObject root;
    {
        QJsonObject parser;
        parser["regex"] = logFormat->parser.pattern();
        root["parser"] = parser;
    }

    QJsonArray highlightsArray;
    for (const auto& highlight : logFormat->highlights) {
        QJsonObject highlightObj = saveHighlight(highlight);
        highlightsArray.append(highlightObj);
    }
    root["highlights"] = highlightsArray;

    QJsonDocument doc;
    doc.setObject(root);
    return doc;
}

namespace LogFormatIO {

shared_ptr<LogFormat> load(const QString& name) {
    QString filePath = LogFormatIO::pathForLogFormat(name);
    return loadFromPath(filePath);
}

shared_ptr<LogFormat> loadFromPath(const QString& filePath) {
    optional<QByteArray> json = readFile(filePath);
    if (!json.has_value()) {
        return LogFormat::createEmpty();
    }

    QJsonParseError error;
    auto doc = QJsonDocument::fromJson(json.value(), &error);
    if (error.error != QJsonParseError::NoError) {
        qCritical() << "Invalid Json in" << filePath << ":" << error.errorString();
        return LogFormat::createEmpty();
    }

    auto logFormat = loadLogFormat(doc);
    if (!logFormat) {
        return LogFormat::createEmpty();
    }
    logFormat->name = QFileInfo(filePath).baseName();
    return logFormat;
}

QString logFormatsDirPath() {
    return QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/logformats";
}

QString pathForLogFormat(const QString& name) {
    return QString("%1/%2.json").arg(logFormatsDirPath(), name);
}

bool save(std::shared_ptr<LogFormat>& logFormat) {
    QString filePath = LogFormatIO::pathForLogFormat(logFormat->name);
    return saveToPath(logFormat, filePath);
}

bool saveToPath(std::shared_ptr<LogFormat>& logFormat, const QString& filePath) {
    QSaveFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to open" << filePath << "for writing. Error:" << file.errorString();
        return false;
    }

    auto doc = saveToJson(logFormat);
    file.write(doc.toJson());

    if (!file.commit()) {
        qWarning() << "Failed to write changes to" << filePath << ". Error:" << file.errorString();
        return false;
    }
    return true;
}

} // namespace LogFormatIO
