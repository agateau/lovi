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

#include <memory>

class Config;
class LineProvider;
class LogFormat;
class LogFormatStore;
class LogModel;

class MainController : public QObject {
    Q_OBJECT
public:
    explicit MainController(Config* config, LogFormatStore* store, QObject* parent = nullptr);
    ~MainController();

    void loadLog(const QString& logPath);

    Config* config() const;

    LogFormatStore* logFormatStore() const;

    LineProvider* lineProvider() const;

    LogModel* logModel() const;

    QString logPath() const;

    bool isStdin() const;

    void setLogFormat(LogFormat* logFormat);
    LogFormat* logFormat() const;

private:
    void updateLogFormatForFile();
    void createLineProvider();
    void addLogToRecentFiles();

    Config* const mConfig;
    LogFormatStore* const mLogFormatStore;

    std::unique_ptr<LineProvider> mLineProvider;
    std::unique_ptr<LogModel> mLogModel;

    QString mLogPath;
    const std::unique_ptr<LogFormat> mEmptyLogFormat;
    LogFormat* mLogFormat = nullptr; // Never null
};

#endif // MAINCONTROLLER_H
