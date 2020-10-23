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
#include "LogFormat.h"

#include "Catch2QtUtils.h"

#include <catch2/catch.hpp>

TEST_CASE("LogFormat") {
    LogFormat format;
    SECTION("patternUpdate") {
        // GIVEN a log format with a single column and a highlight matching this column
        format.setParserPattern("(?<message>.*)");
        Highlight* highlight = format.addHighlight();
        highlight->setConditionDefinition("message == foo");
        REQUIRE(highlight->condition()->column() == 0);

        // WHEN a new column is inserted before the current one
        format.setParserPattern("(?<level>[A-Z]) (?<message>.*)");

        // THEN the column of the highlight condition is updated
        REQUIRE(highlight->condition()->column() == 1);
    }
}
