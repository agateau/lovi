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

#include "Catch2QtUtils.h"

#include <QTemporaryDir>

#include <catch2/catch.hpp>

using std::optional;

TEST_CASE("LogFormatStore") {
    SECTION("save") {
        QTemporaryDir tempDir;
        QString storePath = tempDir.filePath("foo/bar");
        LogFormatStore store(storePath);
        optional<QString> error = store.addLogFormat("test");
        REQUIRE(!error.has_value());
    }
}
