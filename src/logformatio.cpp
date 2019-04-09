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

#include "conditions.h"
#include "logformat.h"

#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
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
        auto conditionObj = highlightObj.value("condition").toObject();
        auto columnName = conditionObj.value("column").toString();
        auto value = conditionObj.value("value").toString();
        auto op = conditionObj.value("op").toString();

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

namespace LogFormatIO {

shared_ptr<LogFormat> load(const QString& name) {
    QString filePath = LogFormatIO::pathForLogFormat(name);
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
    logFormat->name = name;
    return logFormat;
}

QString logFormatsDirPath() {
    return QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/logformats";
}

QString pathForLogFormat(const QString& name) {
    return QString("%1/%2.json").arg(logFormatsDirPath(), name);
}

} // namespace LogFormatIO
