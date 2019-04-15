#include "logformatio.h"

#include "logformat.h"

#include "testconfig.h"
#include "testutils.h"

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
