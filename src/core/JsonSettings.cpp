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
#include "JsonSettings.h"

#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSaveFile>

JsonSettings::JsonSettings(const QString& path) : mConfigPath(path) {
}

QString JsonSettings::readString(const QString& key) {
    load();
    return mRoot.value(key).toString();
}

void JsonSettings::writeString(const QString& key, const QString& value) {
    load();
    mRoot.insert(key, value);
    save();
}

QStringList JsonSettings::readStringList(const QString& key) {
    load();
    QJsonArray array = mRoot.value(key).toArray();
    QStringList list;
    for (const auto value : array) {
        list << value.toString();
    }
    return list;
}

void JsonSettings::writeStringList(const QString& key, const QStringList& list) {
    load();
    QJsonArray array = QJsonArray::fromStringList(list);
    mRoot.insert(key, array);
    save();
}

QVariantHash JsonSettings::readVariantHash(const QString& key) {
    load();
    QJsonObject object = mRoot.value(key).toObject();
    return object.toVariantHash();
}

void JsonSettings::writeVariantHash(const QString& key, const QVariantHash& hash) {
    load();
    auto hashValue = QJsonObject::fromVariantHash(hash);
    mRoot.insert(key, hashValue);
    save();
}

void JsonSettings::load() {
    QFileInfo info(mConfigPath);
    info.refresh();
    if (!info.exists()) {
        return;
    }
    if (info.lastModified() <= mLastRead) {
        return;
    }
    QFile file(mConfigPath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Can't read" << mConfigPath << file.errorString();
        return;
    }
    QByteArray json = file.readAll();
    QJsonParseError error;
    auto doc = QJsonDocument::fromJson(json, &error);
    if (error.error != QJsonParseError::NoError) {
        qWarning() << "Error parsing" << mConfigPath << error.errorString();
        return;
    }
    if (!doc.isObject()) {
        qWarning() << mConfigPath << "should be an object";
    }
    mRoot = doc.object();
    mLastRead = QDateTime::currentDateTime();
}

void JsonSettings::save() {
    QSaveFile file(mConfigPath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Can't create" << mConfigPath << file.errorString();
        return;
    }
    auto json = QJsonDocument(mRoot).toJson();
    file.write(json);
    if (!file.commit()) {
        qWarning() << "Failed to commit" << mConfigPath << file.errorString();
    }
    mLastRead = QDateTime::currentDateTime();
}
