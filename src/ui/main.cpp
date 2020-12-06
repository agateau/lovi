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
#include "MainWindow.h"

#include "BuildConfig.h"
#include "Config.h"
#include "LogFormatStore.h"
#include "Resources.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <QDir>
#include <QIcon>
#include <QLocale>
#include <QStandardPaths>
#include <QTranslator>

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

static QString configPath() {
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    if (!QDir(configDir).mkpath(".")) {
        qWarning() << "Could not create dir" << configDir;
    }
    return configDir + "/config.json";
}

static QString logFormatsDirPath() {
    return QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/logformats";
}

static void loadTranslations(QObject* parent) {
    // Search in current path first, to give translators an easy way to test
    // their translations
    QStringList searchDirs = {QDir::currentPath(), Resources::findDir("translations")};
    auto translator = new QTranslator(parent);
    QLocale locale;
    for (const auto& dir : searchDirs) {
        if (translator->load(locale, APP_NAME, "_", dir)) {
            QCoreApplication::installTranslator(translator);
            return;
        }
    }
}

/**
 * Initialize QIcon so that QIcon::fromTheme() finds our icons on Windows and macOS
 */
static void initFallbackIcons() {
#if defined(Q_OS_WINDOWS) || defined(Q_OS_MACOS)
    // A theme name must be defined othewise QIcon::fromTheme won't look in fallbackSearchPaths
    QIcon::setThemeName(APP_NAME);
    QIcon::setFallbackSearchPaths(QIcon::fallbackSearchPaths() << ":/icons");
#endif
}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    Q_INIT_RESOURCE(lovi);
    app.setApplicationName(APP_NAME);
    app.setApplicationVersion(APP_VERSION);
    auto iconName = QString(":/appicon/sc-apps-%1.svg").arg(APP_NAME);
    app.setWindowIcon(QIcon(iconName));
    app.setAttribute(Qt::AA_UseHighDpiPixmaps);

    initFallbackIcons();

    loadTranslations(&app);

    unique_ptr<QCommandLineParser> parser = createParser();
    parser->process(app);

    Config config(configPath());
    LogFormatStore store(logFormatsDirPath());
    MainWindow window(&config, &store);
    if (parser->isSet("format")) {
        QString formatName = parser->value("format");
        LogFormat* logFormat = store.byName(formatName);
        if (logFormat) {
            window.setLogFormat(logFormat);
        }
    }
    if (parser->positionalArguments().length() == 1) {
        window.loadLog(parser->positionalArguments().first());
    }
    window.show();

    return app.exec();
}
