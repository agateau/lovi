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
#include "ConditionIO.h"

#include "TestUtils.h"

#include <catch2/catch.hpp>

struct Fixture {
    QString input;
    QStringList expected;
};

TEST_CASE("conditionio") {
    QList<Fixture> fixtures = {
        {"foo = bar", {"foo", "=", "bar"}},
        {"\"foo bar\"", {"foo bar"}},
    };
    for (const auto& fixture : fixtures) {
        SECTION(fixture.input.toStdString()) {
            auto result = ConditionIO::tokenize(fixture.input);
            REQUIRE(result.has_value());
            REQUIRE(result.value() == fixture.expected);
        }
    }
}
