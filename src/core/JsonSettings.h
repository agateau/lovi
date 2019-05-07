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
#ifndef JSONSETTINGS_H
#define JSONSETTINGS_H

#include <QDateTime>
#include <QJsonObject>
#include <QVariant>

/**
 * Provides key/value access to a json file.
 * Automatically reloads the json file when it is changed by another process.
 */
class JsonSettings {
public:
    explicit JsonSettings(const QString& path);

    QString readString(const QString& key);
    void writeString(const QString& key, const QString& value);

    QStringList readStringList(const QString& key);
    void writeStringList(const QString& key, const QStringList& value);

    QVariantHash readVariantHash(const QString& key);
    void writeVariantHash(const QString& key, const QVariantHash& hash);

private:
    void load();
    void save();

    const QString mConfigPath;
    QJsonObject mRoot;
    QDateTime mLastRead;
};

#endif // JSONSETTINGS_H
