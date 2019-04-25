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
#include "Searcher.h"
#include "StdinLineProvider.h"

#include <QDebug>

static const int MAX_RECENT_FILES = 10;

MainController::MainController(Config* config, LogFormatStore* store, QObject* parent)
        : QObject(parent)
        , mConfig(config)
        , mLogFormatStore(store)
        , mEmptyLogFormat(LogFormat::createEmpty())
        , mSearcher(std::make_unique<Searcher>()) {
    mLogFormat = mEmptyLogFormat.get();
    connect(mSearcher.get(), &Searcher::finished, this, &MainController::onSearchFinished);
}

MainController::~MainController() {
}

void MainController::loadLog(const QString& filePath) {
    mLogPath = filePath;

    LogFormat* format = mLogFormat;
    if (!isStdin()) {
        addLogToRecentFiles();
        QString name = mConfig->logFormatForFile().value(filePath);
        if (!name.isEmpty()) {
            format = mLogFormatStore->byName(name);
        }
    }

    createLineProvider();
    mLogModel = std::make_unique<LogModel>(mLineProvider.get(), format);
    if (mLogFormat == format) {
        updateLogFormatForFile();
    } else {
        setLogFormat(format);
    }
}

Config* MainController::config() const {
    return mConfig;
}

LogFormatStore* MainController::logFormatStore() const {
    return mLogFormatStore;
}

Searcher* MainController::searcher() const {
    return mSearcher.get();
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
    if (mLogFormat == format) {
        return;
    }
    mLogFormat = format;
    if (mLogModel) {
        updateLogFormatForFile();
        mLogModel->setLogFormat(format);
    }
    logFormatChanged(mLogFormat);
}

LogFormat* MainController::logFormat() const {
    return mLogFormat;
}

void MainController::setCurrentRow(int row) {
    if (mCurrentRow == row) {
        return;
    }
    mCurrentRow = row;
    currentRowChanged(mCurrentRow);
}

int MainController::currentRow() const {
    return mCurrentRow;
}

void MainController::startSearch(std::unique_ptr<Condition> condition, SearchDirection direction) {
    int row = mCurrentRow + (direction == SearchDirection::Down ? 1 : -1);
    mSearcher->start(mLogModel.get(), std::move(condition), direction, row);
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

void MainController::onSearchFinished(const SearchResponse& response) {
    if (response.result == SearchResponse::NoHit) {
        return;
    }
    Q_ASSERT(response.row >= 0);
    setCurrentRow(response.row);
}
