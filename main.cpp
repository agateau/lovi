#include "config.h"
#include "filelineprovider.h"
#include "filewatcher.h"
#include "logmodel.h"
#include "mainwindow.h"

#include <QAbstractListModel>
#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <QFile>
#include <QJsonDocument>

#include <iostream>
#include <memory>

using std::optional;
using std::cout;
using std::unique_ptr;

void dumpModel(LogModel* model) {
    int rowCount = model->rowCount();
    int columnCount = model->columns().count();
    cout << "<html><body>\n";
    cout << "<table>\n";
    for (int row = 0; row < rowCount; ++row) {
        auto idx = model->index(row, 0);
        QVariant bgColorVariant = idx.data(Qt::BackgroundColorRole);
        cout << "<tr";
        if (bgColorVariant.isValid()) {
            QColor color = bgColorVariant.value<QColor>();
            cout << " style='background-color:" << color.name().toStdString() << "'";
        }
        cout << ">";

        for (int column = 0; column < columnCount; ++column) {
            QString value = model->index(row, column).data().toString();
            cout << "<td>" << value.toStdString() << "</td>";
        }
        cout << "</tr>\n";
    }
    cout << "</table>\n";
    cout << "</body></html>\n";
}

optional<QByteArray> readFile(const QString& fileName) {
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        qCritical() << "Could not open" << fileName << file.errorString();
        return {};
    }
    return file.readAll();
}

unique_ptr<Config> loadConfig(const QString& fileName) {
    optional<QByteArray> json = readFile(fileName);
    if (!json.has_value()) {
        return {};
    }

    QJsonParseError error;
    auto doc = QJsonDocument::fromJson(json.value(), &error);
    if (error.error != QJsonParseError::NoError) {
        qCritical() << "Invalid Json in" << fileName << ":" << error.errorString();
        return {};
    }

    return Config::fromJsonDocument(doc);
}

unique_ptr<QCommandLineParser> createParser() {
    unique_ptr<QCommandLineParser> parser = std::make_unique<QCommandLineParser>();
    parser->setApplicationDescription(QCoreApplication::translate("main", "Log viewer"));
    parser->addHelpOption();
    parser->addVersionOption();
    parser->addPositionalArgument("log_file", QCoreApplication::translate("main", "Log file."));
    parser->addOption({{"f", "format"},
                     QCoreApplication::translate("main", "Log format definition."),
                     QCoreApplication::translate("main", "log_format.json"),
                     });
    return parser;
}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    unique_ptr<QCommandLineParser> parser = createParser();
    parser->process(app);

    QString logFileName = parser->positionalArguments().first();
    Q_ASSERT(parser->isSet("format"));
    QString configFileName = parser->value("format");

    FileLineProvider lineProvider;
    lineProvider.setFilePath(logFileName);
    LogModel model(&lineProvider);

    unique_ptr<Config> config = loadConfig(configFileName);
    if (!config) {
        return 1;
    }
    model.setConfig(config.get());

    auto reloadConfig = [&configFileName, &model, &config] {
        qDebug() << "Reloading config";
        unique_ptr<Config> newConfig = loadConfig(configFileName);
        if (newConfig) {
            model.setConfig(newConfig.get());
            config = std::move(newConfig);
        }
    };
    FileWatcher watcher;
    watcher.setFilePath(configFileName);
    QObject::connect(&watcher, &FileWatcher::fileChanged, reloadConfig);

    //dumpModel(&model);
    MainWindow window(&model);
    window.show();
    return app.exec();
}
