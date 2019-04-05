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
#include "config.h"

#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

Config::Config(const QString& configPath, QObject* parent)
        : QObject(parent), mConfigPath(configPath) {
    load();
}

QStringList Config::recentLogFiles() const {
    return mRecentLogFiles;
}

void Config::setRecentLogFiles(const QStringList& files) {
    mRecentLogFiles = files;
    save();
}

void Config::load() {
    QFile file(mConfigPath);
    if (!file.exists()) {
        return;
    }
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Cannot read" << mConfigPath << ":" << file.errorString();
        return;
    }
    QByteArray json = file.readAll();

    QJsonParseError error;
    auto doc = QJsonDocument::fromJson(json, &error);
    if (error.error != QJsonParseError::NoError) {
        qWarning() << "Cannot parse" << mConfigPath << ":" << error.errorString();
        return;
    }

    auto root = doc.object();
    {
        auto array = root.value("recentLogFiles").toArray();
        mRecentLogFiles.clear();
        for (const QJsonValue value : array) {
            mRecentLogFiles << value.toString();
        }
    }
}

void Config::save() const {
    QFile file(mConfigPath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Cannot write to" << mConfigPath << ":" << file.errorString();
        return;
    }

    QJsonObject root;
    {
        QJsonArray array;
        for (const auto& value : mRecentLogFiles) {
            array << QJsonValue(value);
        }
        root["recentLogFiles"] = array;
    }
    file.write(QJsonDocument(root).toJson());
}
