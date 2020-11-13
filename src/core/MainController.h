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

#include <QObject>

// stl
#include <memory>
#include <optional>

enum class SearchDirection;
class Condition;
class Config;
class Highlight;
class LineProvider;
class LogFormat;
class LogFormatStore;
class LogModel;
class Searcher;
class SearchResponse;

class QAbstractItemModel;
class FilterProxyModel;

class MainController : public QObject {
    Q_OBJECT
public:
    explicit MainController(Config* config, LogFormatStore* store, QObject* parent = nullptr);
    ~MainController();

    void loadLog(const QString& logPath);

    Config* config() const;

    LogFormatStore* logFormatStore() const;

    Searcher* searcher() const;

    QAbstractItemModel* logModel() const;

    bool isStdin() const;

    void setLogFormat(LogFormat* logFormat);
    LogFormat* logFormat() const;

    std::optional<int> currentRow() const;
    void setCurrentRow(const std::optional<int>& value);

    QString logPath() const;

    Highlight* currentHighlight() const;
    void setCurrentHighlight(Highlight* value);

    void startSearch(SearchDirection direction);

    QStringRef lineAt(int row) const;

signals:
    void logFormatChanged(LogFormat* logFormat);
    void currentRowChanged(const std::optional<int>& currentRow);
    void currentHighlightChanged(Highlight* currentHighlight);

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
    std::unique_ptr<FilterProxyModel> mFilterProxyModel;

    LogFormat* mLogFormat = nullptr; // Never null
    QString mLogPath;
    std::optional<int> mCurrentRow;
    Highlight* mCurrentHighlight = nullptr;
};

#endif // MAINCONTROLLER_H
