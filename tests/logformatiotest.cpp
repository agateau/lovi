#include "logformatio.h"

#include "logformat.h"

#include "testconfig.h"
#include "testutils.h"

#include <QTemporaryDir>

#include <catch2/catch.hpp>

using std::shared_ptr;

struct Fixture {
    QString input;
    QStringList expected;
};

void checkLogFormatEquality(const LogFormat& f1, const LogFormat& f2) {
    REQUIRE(f1.name == f2.name);
    REQUIRE(f1.parser.pattern() == f2.parser.pattern());

    for (size_t idx = 0; idx < f1.highlights.size(); ++idx) {
        REQUIRE(idx < f2.highlights.size());
        const auto& hl1 = f1.highlights.at(idx);
        const auto& hl2 = f2.highlights.at(idx);
        REQUIRE(hl1.conditionDefinition == hl2.conditionDefinition);
    }
}

TEST_CASE("logformatio") {
    QString testFile = QString(TEST_DATA_DIR) + "/logformatiotest-load.json";
    shared_ptr<LogFormat> format = LogFormatIO::loadFromPath(testFile);

    SECTION("load") {
        REQUIRE(format->parser.pattern() == "^(?<level>[DEW])/(?<app>[^:]*): (?<message>.*)");
        REQUIRE(format->highlights[0].conditionDefinition == "level == E");
        REQUIRE(format->highlights[0].rowBgColor->toString() == "#ff0000");

        REQUIRE(format->highlights[1].conditionDefinition == "level == W");
        REQUIRE(format->highlights[1].rowFgColor->toString() == "#ff8800");

        REQUIRE(format->highlights[2].conditionDefinition == "message ~ start.*");
        REQUIRE(format->highlights[2].bgColor->toString() == "auto");

        REQUIRE(format->highlights[3].conditionDefinition == "message contains bob");
        REQUIRE(format->highlights[3].fgColor->toString() == "#00ff00");
    }

    SECTION("save") {
        QTemporaryDir tempDir;
        QString tempPath = tempDir.path() + "/" + format->name + ".json";
        REQUIRE(LogFormatIO::saveToPath(format, tempPath));

        shared_ptr<LogFormat> format2 = LogFormatIO::loadFromPath(tempPath);
        checkLogFormatEquality(*format, *format2);
    }
}
