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
#ifndef LOGFORMATLOADER_H
#define LOGFORMATLOADER_H

#include <QObject>

#include <memory>

class FileWatcher;
class LogFormat;

class LogFormatLoader : public QObject {
    Q_OBJECT
public:
    explicit LogFormatLoader(QObject* parent = nullptr);
    ~LogFormatLoader();

    void load(const QString& name);

    LogFormat* logFormat() const;

    static QString logFormatsDirPath();

    static QString pathForLogFormat(const QString& name);

signals:
    void logFormatChanged(LogFormat* logFormat);

private:
    void reload();

    const std::unique_ptr<FileWatcher> mWatcher;
    QString mLogFormatName;
    std::unique_ptr<LogFormat> mLogFormat;
};

#endif // LOGFORMATLOADER_H
