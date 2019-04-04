#include "mainwindow.h"

#include "config.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <QDir>
#include <QStandardPaths>

#include <memory>

using std::unique_ptr;

unique_ptr<QCommandLineParser> createParser() {
    unique_ptr<QCommandLineParser> parser = std::make_unique<QCommandLineParser>();
    parser->setApplicationDescription(QCoreApplication::translate("main", "Log viewer"));
    parser->addHelpOption();
    parser->addVersionOption();
    parser->addPositionalArgument("log_file", QCoreApplication::translate("main", "Log file."));
    parser->addOption({
        {"f", "format"},
        QCoreApplication::translate("main", "Log format definition."),
        QCoreApplication::translate("main", "log_format.json"),
    });
    return parser;
}

static QString getConfigPath() {
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    if (!QDir(configDir).mkpath(".")) {
        qWarning() << "Could not create dir" << configDir;
    }
    return configDir + "/config.json";
}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    unique_ptr<QCommandLineParser> parser = createParser();
    parser->process(app);

    Config config(getConfigPath());
    MainWindow window(&config);
    if (parser->isSet("format")) {
        window.loadLogFormat(parser->value("format"));
    }
    if (parser->positionalArguments().length() == 1) {
        window.loadLog(parser->positionalArguments().first());
    }
    window.show();
    return app.exec();
}
