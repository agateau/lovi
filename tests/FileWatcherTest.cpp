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
#include "FileWatcher.h"

#include <QSaveFile>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QTest>

#include <catch2/catch.hpp>

void createFile(const QString& path) {
    QFile file(path);
    REQUIRE(file.open(QIODevice::WriteOnly));
};

void appendToFile(const QString& path) {
    QFile file(path);
    REQUIRE(file.open(QIODevice::Append));
    file.write("hello\n");
};

TEST_CASE("filewatcher") {
    QTemporaryDir tempDir("filewatchertest-XXXXXX");
    FileWatcher watcher;
    QSignalSpy fileCreatedSpy(&watcher, &FileWatcher::fileCreated);
    QSignalSpy fileChangedSpy(&watcher, &FileWatcher::fileChanged);
    QSignalSpy fileDeletedSpy(&watcher, &FileWatcher::fileDeleted);

    SECTION("watch existing file") {
        // GIVEN an existing file
        QString path = tempDir.filePath("foo");
        createFile(path);
        // AND a watcher on it
        watcher.setFilePath(path);
        // WHEN I append to the file
        appendToFile(path);
        // THEN only fileChanged is emitted
        QTest::qWait(500);
        REQUIRE(fileCreatedSpy.count() == 0);
        REQUIRE(fileChangedSpy.count() == 1);
    }

    SECTION("watch new file") {
        // GIVEN a non-existing file
        QString path = tempDir.filePath("foo");
        // AND a watcher on it
        watcher.setFilePath(path);
        // WHEN I create the file
        createFile(path);
        // THEN only fileCreated is emitted
        QTest::qWait(500);
        REQUIRE(fileCreatedSpy.count() == 1);
        REQUIRE(fileChangedSpy.count() == 0);
    }

#if 0
    // Does not work. TODO: figure out why
    SECTION("watch recreated file") {
        // GIVEN an existing file
        QString path = tempDir.filePath("foo");
        createFile(path);
        // AND a watcher on it
        watcher.setFilePath(path);
        // WHEN I overwrite the file with a new one
        {
            QSaveFile saveFile(path);
            REQUIRE(saveFile.open(QIODevice::WriteOnly));
            saveFile.write("New content\n");
            REQUIRE(saveFile.commit());
        }
        // THEN fileDeleted and fileCreated is emitted
        QTest::qWait(500);
        REQUIRE(fileChangedSpy.count() == 0);
        REQUIRE(fileCreatedSpy.count() == 1);
        REQUIRE(fileDeletedSpy.count() == 1);
    }
#endif
}
