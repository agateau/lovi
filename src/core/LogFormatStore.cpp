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
#include "LogFormatStore.h"

#include "LogFormat.h"
#include "LogFormatIO.h"

#include <QDebug>
#include <QDir>
#include <QFileInfo>

using std::optional;
using std::unique_ptr;

LogFormatStore::LogFormatStore(const QString& dirPath, QObject* parent)
        : QObject(parent), mDirPath(dirPath) {
    load();
}

LogFormatStore::~LogFormatStore() {
}

LogFormat* LogFormatStore::byName(const QString& name) const {
    auto it = std::find(mLogFormatNames.begin(), mLogFormatNames.end(), name);
    if (it == mLogFormatNames.end()) {
        qWarning() << "No log format named" << name;
        return {};
    }
    auto idx = it - mLogFormatNames.begin();
    return at(int(idx));
}

QString LogFormatStore::nameAt(int idx) const {
    return mLogFormatNames.at(idx);
}

LogFormat* LogFormatStore::at(int idx) const {
    LogFormat* logFormat = mLogFormats.at(idx).get();
    if (!logFormat) {
        auto path = pathForName(mLogFormatNames.at(idx));
        unique_ptr<LogFormat> newLogFormat = LogFormatIO::loadFromPath(path);
        logFormat = newLogFormat.get();
        connect(logFormat, &LogFormat::changed, logFormat, [logFormat, path] {
            LogFormatIO::saveToPath(logFormat, path);
        });
        mLogFormats[idx] = std::move(newLogFormat);
    }
    return logFormat;
}

int LogFormatStore::count() const {
    return mLogFormatNames.size();
}

optional<QString> LogFormatStore::addLogFormat(const QString& name) {
    auto it = std::find(mLogFormatNames.begin(), mLogFormatNames.end(), name);
    if (it != mLogFormatNames.end()) {
        return tr("There is already a log format named '%1'.").arg(name);
    }

    if (!QDir(mDirPath).mkpath(".")) {
        return tr("Could not create directory '%1' to store log format.").arg(mDirPath);
    }

    // Store an empty file, to ensure it can be created
    unique_ptr<LogFormat> logFormat = LogFormat::createEmpty();
    QString path = pathForName(name);
    if (!LogFormatIO::saveToPath(logFormat.get(), path)) {
        return tr("Could not create file '%1'.").arg(path);
    }

    mLogFormatNames.push_back(name);
    mLogFormats.push_back({});

    logFormatAdded();
    return {};
}

void LogFormatStore::load() {
    QDir dir(mDirPath);
    mLogFormatNames.clear();
    for (const QFileInfo& info : dir.entryInfoList({"*.json"}, QDir::Files, QDir::Name)) {
        mLogFormatNames.push_back(info.baseName());
    }
    mLogFormats.clear();
    mLogFormats.resize(mLogFormatNames.size());
}

QString LogFormatStore::pathForName(const QString& name) const {
    return QString("%1/%2.json").arg(mDirPath, name);
}
