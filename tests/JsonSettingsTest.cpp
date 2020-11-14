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

#include "Catch2QtUtils.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QTemporaryDir>

#include <catch2/catch.hpp>

// stl
#include <optional>

using std::optional;

static void writeContent(const QString& path, const QByteArray& content) {
    QFile file(path);
    REQUIRE(file.open(QIODevice::WriteOnly));
    file.write(content);
}

static void writeJson(const QString& path, const QVariantHash& hash) {
    QJsonDocument doc;
    doc.setObject(QJsonObject::fromVariantHash(hash));
    writeContent(path, doc.toJson());
}

static QVariantHash readJson(const QString& path) {
    QFile file(path);
    REQUIRE(file.open(QIODevice::ReadOnly));
    QByteArray json = file.readAll();
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json, &error);
    REQUIRE(error.error == QJsonParseError::NoError);
    return doc.object().toVariantHash();
}

TEST_CASE("JsonSettings") {
    QTemporaryDir tempDir;
    QString path = tempDir.filePath("conf.json");
    JsonSettings settings(path);
    QVariantHash testHash = {{"k1", "v1"}, {"k2", 12}};
    SECTION("load") {
        writeJson(path,
                  {
                      {"key", "value"},
                      {"lst", QVariantList{"a", "b"}},
                      {"hash", testHash},
                  });
        REQUIRE(settings.readString("key") == "value");
        REQUIRE(settings.readStringList("lst") == QStringList{"a", "b"});
        REQUIRE(settings.readVariantHash("hash") == testHash);
    }

    SECTION("save") {
        settings.writeString("key", "value");
        settings.writeStringList("lst", {"a", "b"});
        settings.writeVariantHash("hash", testHash);
        QVariantHash hash = readJson(path);
        QVariantHash expected = {
            {"key", "value"},
            {"lst", QVariantList{"a", "b"}},
            {"hash", testHash},
        };
        REQUIRE(hash == expected);
    }

    SECTION("saveOverGarbage") {
        writeContent(path, "garbage");
        settings.writeString("key", "value");
        QVariantHash hash = readJson(path);
        QVariantHash expected = {{"key", "value"}};
        REQUIRE(hash == expected);
    }
}
