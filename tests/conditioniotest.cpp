#include "conditionio.h"

#include "testutils.h"

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
