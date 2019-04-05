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
#ifndef FILEWATCHER_H
#define FILEWATCHER_H

#include <QDateTime>
#include <QObject>

class QTimer;
class QFileSystemWatcher;

/**
 * A single file watcher, which also works when a file is deleted then recreated
 */
class FileWatcher : public QObject {
    Q_OBJECT
public:
    explicit FileWatcher(QObject* parent = nullptr);

    void setFilePath(const QString& path);
    QString filePath() const;

signals:
    void fileChanged();

private:
    void scheduleNotification();
    void onChangeDetected();

    QFileSystemWatcher* mWatcher = nullptr;
    QTimer* mTimer = nullptr;
    QString mPath;
    QDateTime mLastModified;
};

#endif // FILEWATCHER_H
