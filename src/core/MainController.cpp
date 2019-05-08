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
        : BaseMainController(parent)
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
    setLogPath(filePath);

    LogFormat* format = mLogFormat;
    if (!isStdin()) {
        addLogToRecentFiles();
        QString name = mConfig->logFormatForFile(filePath);
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

bool MainController::isStdin() const {
    return logPath() == "-";
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
    setCurrentHighlight(nullptr);
}

LogFormat* MainController::logFormat() const {
    return mLogFormat;
}

void MainController::startSearch(SearchDirection direction) {
    if (!currentHighlight()) {
        return;
    }
    auto* condition = currentHighlight()->condition();
    if (!condition) {
        return;
    }
    int row = currentRow().value_or(0) + (direction == SearchDirection::Down ? 1 : -1);
    mSearcher->start(mLogModel.get(), condition, direction, row);
}

void MainController::updateLogFormatForFile() {
    if (!logPath().isEmpty() && !isStdin() && !mLogFormat->name().isEmpty()) {
        mConfig->setLogFormatForFile(logPath(), mLogFormat->name());
    }
}

void MainController::createLineProvider() {
    if (isStdin()) {
        mLineProvider = std::make_unique<StdinLineProvider>();
    } else {
        mLineProvider = std::make_unique<FileLineProvider>(logPath());
    }
}

void MainController::addLogToRecentFiles() {
    QStringList files = mConfig->recentLogFiles();
    files.removeOne(logPath());
    files.insert(0, logPath());
    while (files.length() > MAX_RECENT_FILES) {
        files.takeLast();
    }
    mConfig->setRecentLogFiles(files);
}

void MainController::onSearchFinished(const SearchResponse& response) {
    if (response.matchType == SearchMatchType::None) {
        return;
    }
    Q_ASSERT(response.row.has_value());
    setCurrentRow(response.row);
}
