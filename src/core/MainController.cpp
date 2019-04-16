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
#include "MainController.h"

#include "Config.h"
#include "FileLineProvider.h"
#include "LogFormat.h"
#include "LogFormatStore.h"
#include "LogModel.h"
#include "StdinLineProvider.h"

#include <QDebug>

static const int MAX_RECENT_FILES = 10;

MainController::MainController(Config* config, LogFormatStore* store, QObject* parent)
        : QObject(parent)
        , mConfig(config)
        , mLogFormatStore(store)
        , mEmptyLogFormat(LogFormat::createEmpty()) {
    mLogFormat = mEmptyLogFormat.get();
}

MainController::~MainController() {
}

void MainController::loadLog(const QString& filePath) {
    mLogPath = filePath;

    LogFormat* format = nullptr;
    if (!isStdin()) {
        addLogToRecentFiles();
        QString name = mConfig->logFormatForFile().value(filePath);
        if (!name.isEmpty()) {
            format = mLogFormatStore->byName(name);
        }
    }

    createLineProvider();
    mLogModel = std::make_unique<LogModel>(mLineProvider.get(), format ? format : mLogFormat);
    updateLogFormatForFile();
}

Config* MainController::config() const {
    return mConfig;
}

LogFormatStore* MainController::logFormatStore() const {
    return mLogFormatStore;
}

LineProvider* MainController::lineProvider() const {
    return mLineProvider.get();
}

LogModel* MainController::logModel() const {
    return mLogModel.get();
}

QString MainController::logPath() const {
    return mLogPath;
}

bool MainController::isStdin() const {
    return mLogPath == "-";
}

void MainController::setLogFormat(LogFormat* format) {
    mLogFormat = format;
    if (mLogModel) {
        mLogModel->setLogFormat(mLogFormat);
        updateLogFormatForFile();
    }
}

LogFormat* MainController::logFormat() const {
    return mLogFormat;
}

void MainController::updateLogFormatForFile() {
    if (!mLogPath.isEmpty() && !isStdin() && !mLogFormat->name().isEmpty()) {
        mConfig->setLogFormatForFile(mLogPath, mLogFormat->name());
    }
}

void MainController::createLineProvider() {
    if (isStdin()) {
        mLineProvider = std::make_unique<StdinLineProvider>();
    } else {
        mLineProvider = std::make_unique<FileLineProvider>(mLogPath);
    }
}

void MainController::addLogToRecentFiles() {
    QStringList files = mConfig->recentLogFiles();
    files.removeOne(mLogPath);
    files.insert(0, mLogPath);
    while (files.length() > MAX_RECENT_FILES) {
        files.takeLast();
    }
    mConfig->setRecentLogFiles(files);
}
