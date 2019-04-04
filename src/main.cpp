#include "mainwindow.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>

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

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    unique_ptr<QCommandLineParser> parser = createParser();
    parser->process(app);

    MainWindow window;
    if (parser->isSet("format")) {
        window.loadLogFormat(parser->value("format"));
    }
    if (parser->positionalArguments().length() == 1) {
        window.loadLog(parser->positionalArguments().first());
    }
    window.show();
    return app.exec();
}
