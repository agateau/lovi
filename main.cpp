#include "config.h"
#include "filewatcher.h"
#include "lineprovider.h"
#include "logmodel.h"
#include "mainwindow.h"

#include <QAbstractListModel>
#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
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

class MemoryLineProvider : public LineProvider {
public:
    MemoryLineProvider(const QStringList& lines)
        : mLines(lines) {}

    const QString& lineAt(int row) const override {
        return mLines.at(row);
    }

    int lineCount() const override {
        return mLines.length();
    }

private:
    QStringList mLines;
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    QString configFileName = QFileInfo(argv[1]).absoluteFilePath();
    QString logFileName = argv[2];

    unique_ptr<Config> config = loadConfig(configFileName);
    if (!config) {
        return 1;
    }

    QStringList lines;
    {
        auto logContent = readFile(logFileName);
        if (!logContent.has_value()) {
            return 1;
        }
        lines = QString::fromUtf8(logContent.value()).split('\n');
    }

    MemoryLineProvider lineProvider(lines);
    LogModel model(config.get(), &lineProvider);

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
