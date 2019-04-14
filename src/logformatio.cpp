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

static OptionalColor initColor(const QString& text) {
    if (text.isEmpty()) {
        return {};
    }
    return Color(text);
}

static unique_ptr<LogFormat> loadLogFormat(const QJsonDocument& doc) {
    auto regex = doc.object().value("parser").toObject().value("regex").toString();
    if (regex.isEmpty()) {
        qWarning() << "No regex found";
        return {};
    }

    unique_ptr<LogFormat> logFormat = std::make_unique<LogFormat>();
    logFormat->setParserPattern(regex);
    if (!logFormat->parser().isValid()) {
        qWarning() << "Invalid parser regex:" << logFormat->parser().errorString();
        return {};
    }

    for (QJsonValue jsonValue : doc.object().value("highlights").toArray()) {
        QJsonObject highlightObj = jsonValue.toObject();

        logFormat->highlights.push_back(Highlight(logFormat.get()));
        Highlight& highlight = *(logFormat->highlights.end() - 1);
        highlight.setConditionDefinition(highlightObj.value("condition").toString());

        QString scope = highlightObj.value("scope").toString();
        if (scope == "row") {
            highlight.setScope(Highlight::Row);
        } else if (scope == "cell") {
            highlight.setScope(Highlight::Cell);
        } else {
            qWarning() << "Invalid scope value:" << scope;
        }

        auto bgColor = highlightObj.value("bgColor").toString();
        auto fgColor = highlightObj.value("fgColor").toString();
        highlight.setBgColor(initColor(bgColor));
        highlight.setFgColor(initColor(fgColor));
    }

    return logFormat;
}

//- Save --------------------------------
static void saveColor(QJsonObject* root, const QString& key, const OptionalColor& color) {
    if (!color.has_value()) {
        return;
    }
    root->insert(key, color.value().toString());
}

static QJsonObject saveHighlight(const Highlight& highlight) {
    QJsonObject root;
    root["condition"] = highlight.conditionDefinition();
    root["scope"] = highlight.scope() == Highlight::Row ? "row" : "cell";
    saveColor(&root, "bgColor", highlight.bgColor());
    saveColor(&root, "fgColor", highlight.fgColor());
    return root;
}

static QJsonDocument saveToJson(LogFormat* logFormat) {
    QJsonObject root;
    {
        QJsonObject parser;
        parser["regex"] = logFormat->parserPattern();
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

unique_ptr<LogFormat> loadFromPath(const QString& filePath) {
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
    logFormat->setName(QFileInfo(filePath).baseName());
    return logFormat;
}

bool saveToPath(LogFormat* logFormat, const QString& filePath) {
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
