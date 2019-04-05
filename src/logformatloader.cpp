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
#include "logformatloader.h"

#include "filewatcher.h"
#include "logformat.h"

#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonParseError>

using std::optional;
using std::unique_ptr;

static optional<QByteArray> readFile(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qCritical() << "Could not open" << filePath << file.errorString();
        return {};
    }
    return file.readAll();
}

static unique_ptr<LogFormat> loadLogFormat(const QString& filePath) {
    optional<QByteArray> json = readFile(filePath);
    if (!json.has_value()) {
        return {};
    }

    QJsonParseError error;
    auto doc = QJsonDocument::fromJson(json.value(), &error);
    if (error.error != QJsonParseError::NoError) {
        qCritical() << "Invalid Json in" << filePath << ":" << error.errorString();
        return {};
    }

    return LogFormat::fromJsonDocument(doc);
}

LogFormatLoader::LogFormatLoader(QObject* parent)
        : QObject(parent), mWatcher(std::make_unique<FileWatcher>()) {
    connect(mWatcher.get(), &FileWatcher::fileChanged, this, &LogFormatLoader::reload);
}

LogFormatLoader::~LogFormatLoader() {
}

void LogFormatLoader::load(const QString& filePath) {
    unique_ptr<LogFormat> logFormat = ::loadLogFormat(filePath);
    if (!logFormat) {
        return;
    }
    mWatcher->setFilePath(filePath);
    logFormatChanged(logFormat.get());
    mLogFormat = std::move(logFormat);
}

LogFormat* LogFormatLoader::logFormat() const {
    return mLogFormat.get();
}

void LogFormatLoader::reload() {
    qInfo() << "Reloading log format";
    load(mWatcher->filePath());
}
