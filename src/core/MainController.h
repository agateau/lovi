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
#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include "BaseMainController.h"

#include <QObject>

#include <memory>

enum class SearchDirection;
class Condition;
class Config;
class LineProvider;
class LogFormat;
class LogFormatStore;
class LogModel;
class Searcher;
class SearchResponse;

class MainController : public BaseMainController {
    Q_OBJECT
public:
    explicit MainController(Config* config, LogFormatStore* store, QObject* parent = nullptr);
    ~MainController();

    void loadLog(const QString& logPath);

    Config* config() const;

    LogFormatStore* logFormatStore() const;

    Searcher* searcher() const;

    LineProvider* lineProvider() const;

    LogModel* logModel() const;

    QString logPath() const;

    bool isStdin() const;

    void setLogFormat(LogFormat* logFormat) override;
    LogFormat* logFormat() const override;

    void startSearch(std::unique_ptr<Condition> condition, SearchDirection direction);

private:
    void updateLogFormatForFile();
    void createLineProvider();
    void addLogToRecentFiles();
    void onSearchFinished(const SearchResponse& response);

    Config* const mConfig;
    LogFormatStore* const mLogFormatStore;

    const std::unique_ptr<LogFormat> mEmptyLogFormat;
    const std::unique_ptr<Searcher> mSearcher;

    // Mutable state
    std::unique_ptr<LineProvider> mLineProvider;
    std::unique_ptr<LogModel> mLogModel;

    QString mLogPath;
    LogFormat* mLogFormat = nullptr; // Never null
};

#endif // MAINCONTROLLER_H
