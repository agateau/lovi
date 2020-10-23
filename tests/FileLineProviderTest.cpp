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
#include "FileLineProvider.h"

#include "Catch2QtUtils.h"

#include <QSignalSpy>
#include <QTemporaryDir>
#include <QTest>

#include <catch2/catch.hpp>

using std::unique_ptr;

static void appendLines(const QString& path, const QStringList& lines) {
    QFile file(path);
    REQUIRE(file.open(QIODevice::Append));
    for (const QString& line : lines) {
        file.write(line.toUtf8() + "\n");
    }
}

static void appendString(const QString& path, const QString& str) {
    QFile file(path);
    REQUIRE(file.open(QIODevice::Append));
    file.write(str.toUtf8());
}

static void microWait() {
    QTest::qWait(1);
}

static void checkSignal(QSignalSpy* spy, const QVariantList& args) {
    REQUIRE(spy->count() == 1);
    REQUIRE(spy->at(0) == args);
    spy->clear();
}

TEST_CASE("FileLineProvider") {
    QTemporaryDir tempDir;
    QString path = tempDir.filePath("test.txt");

    SECTION("existingFile") {
        QStringList lines = {"liné1", "linè2"};
        appendLines(path, lines);

        FileLineProvider provider(path);
        QSignalSpy spy(&provider, &LineProvider::lineCountChanged);
        microWait();

        REQUIRE(provider.lineCount() == 2);
        REQUIRE(provider.lineAt(0) == lines.at(0));
        REQUIRE(provider.lineAt(1) == lines.at(1));
        checkSignal(&spy, {2, 0});

        SECTION("addNewLines") {
            QStringList newLines = {"line3"};
            appendLines(path, newLines);
            microWait();

            REQUIRE(provider.lineCount() == 3);
            REQUIRE(provider.lineAt(2) == newLines.at(0));
            checkSignal(&spy, {3, 2});
        }

        SECTION("appendTruncatedLine") {
            // GIVEN a file provider monitoring a file

            // WHEN I write a string which does not end with \n to the file
            QString part1 = "foo";
            appendString(path, part1);

            // THEN the file provider does not report a new line
            microWait();
            REQUIRE(provider.lineCount() == 2);
            REQUIRE(spy.count() == 0);

            SECTION("finishLine") {
                // GIVEN a file provider with a truncated line
                // WHEN I finish the line
                QString part2 = "bar";
                appendString(path, part2 + "\n");
                // THEN  the file provider reports a new line
                microWait();
                REQUIRE(provider.lineCount() == 3);
                REQUIRE(provider.lineAt(2) == part1 + part2);
                checkSignal(&spy, {3, 2});
            }
        }
    }
}
