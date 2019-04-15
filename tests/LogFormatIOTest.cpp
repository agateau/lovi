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
#include "LogFormatIO.h"

#include "LogFormat.h"

#include "testconfig.h"
#include "TestUtils.h"

#include <QTemporaryDir>

#include <catch2/catch.hpp>

using std::unique_ptr;

struct Fixture {
    QString input;
    QStringList expected;
};

void checkLogFormatEquality(const LogFormat& f1, const LogFormat& f2) {
    REQUIRE(f1.name() == f2.name());
    REQUIRE(f1.parserPattern() == f2.parserPattern());

    for (size_t idx = 0; idx < f1.highlights().size(); ++idx) {
        REQUIRE(idx < f2.highlights().size());
        const auto& hl1 = f1.highlights().at(idx);
        const auto& hl2 = f2.highlights().at(idx);
        REQUIRE(hl1->conditionDefinition() == hl2->conditionDefinition());
    }
}

TEST_CASE("logformatio") {
    QString testFile = QString(TEST_DATA_DIR) + "/logformatiotest-load.json";
    unique_ptr<LogFormat> format = LogFormatIO::loadFromPath(testFile);

    SECTION("load") {
        REQUIRE(format->parserPattern() == "^(?<level>[DEW])/(?<app>[^:]*): (?<message>.*)");
        auto it = format->highlights().begin();
        auto end = format->highlights().end();
        REQUIRE(it != end);
        Highlight* hl = it->get();
        REQUIRE(hl->conditionDefinition() == "level == E");
        REQUIRE(hl->scope() == Highlight::Row);
        REQUIRE(hl->bgColor()->toString() == "#ff0000");

        ++it;
        REQUIRE(it != end);
        hl = it->get();
        REQUIRE(hl->conditionDefinition() == "level == W");
        REQUIRE(hl->scope() == Highlight::Row);
        REQUIRE(hl->fgColor()->toString() == "#ff8800");

        ++it;
        REQUIRE(it != end);
        hl = it->get();
        REQUIRE(hl->conditionDefinition() == "message ~ start.*");
        REQUIRE(hl->scope() == Highlight::Cell);
        REQUIRE(hl->bgColor()->toString() == "auto");

        ++it;
        REQUIRE(it != end);
        hl = it->get();
        REQUIRE(hl->conditionDefinition() == "message contains bob");
        REQUIRE(hl->scope() == Highlight::Cell);
        REQUIRE(hl->fgColor()->toString() == "#00ff00");

        ++it;
        REQUIRE(it == end);
    }

    SECTION("save") {
        QTemporaryDir tempDir;
        QString tempPath = tempDir.path() + "/" + format->name() + ".json";
        REQUIRE(LogFormatIO::saveToPath(format.get(), tempPath));

        unique_ptr<LogFormat> format2 = LogFormatIO::loadFromPath(tempPath);
        checkLogFormatEquality(*format, *format2);
    }
}
