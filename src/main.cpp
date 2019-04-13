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
#include "mainwindow.h"

#include "config.h"
#include "logformatstore.h"

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
        QCoreApplication::translate("main", "log_format_name"),
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

static QString logFormatsDirPath() {
    return QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/logformats";
}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    unique_ptr<QCommandLineParser> parser = createParser();
    parser->process(app);

    Config config(getConfigPath());
    LogFormatStore store(logFormatsDirPath());
    MainWindow window(&config, &store);
    if (parser->isSet("format")) {
        window.loadLogFormat(parser->value("format"));
    }
    if (parser->positionalArguments().length() == 1) {
        window.loadLog(parser->positionalArguments().first());
    }
    window.show();
    return app.exec();
}
