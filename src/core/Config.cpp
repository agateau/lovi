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
#include "Config.h"

#include "JsonSettings.h"

static constexpr char RECENT_LOG_FILES_KEY[] = "recentLogFiles";
static constexpr char LOG_FORMAT_FOR_FILE_KEY[] = "logFormatForFile";

Config::Config(const QString& configPath, QObject* parent)
        : QObject(parent), mSettings(std::make_unique<JsonSettings>(configPath)) {
}

Config::~Config() {
}

QStringList Config::recentLogFiles() const {
    return mSettings->readStringList(RECENT_LOG_FILES_KEY);
}

void Config::setRecentLogFiles(const QStringList& files) {
    mSettings->writeStringList(RECENT_LOG_FILES_KEY, files);
}

QString Config::logFormatForFile(const QString& file) const {
    QVariantHash hash = mSettings->readVariantHash(LOG_FORMAT_FOR_FILE_KEY);
    return hash.value(file).toString();
}

void Config::setLogFormatForFile(const QString& file, const QString& format) {
    QVariantHash hash = mSettings->readVariantHash(LOG_FORMAT_FOR_FILE_KEY);
    hash.insert(file, format);
    mSettings->writeVariantHash(LOG_FORMAT_FOR_FILE_KEY, hash);
}
