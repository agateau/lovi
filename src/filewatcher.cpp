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
#include "filewatcher.h"

#include <QFileInfo>
#include <QFileSystemWatcher>
#include <QTimer>

#include <chrono>

using namespace std::chrono_literals;

static const std::chrono::duration DELAY_INTERVAL = 100ms;

FileWatcher::FileWatcher(QObject* parent)
        : QObject(parent), mWatcher(new QFileSystemWatcher(this)), mTimer(new QTimer(this)) {
    connect(mWatcher, &QFileSystemWatcher::directoryChanged, this, &FileWatcher::onChangeDetected);
    connect(mWatcher, &QFileSystemWatcher::fileChanged, this, &FileWatcher::onChangeDetected);

    mTimer->setInterval(DELAY_INTERVAL);
    mTimer->setSingleShot(true);
    connect(mTimer, &QTimer::timeout, this, &FileWatcher::fileChanged);
}

void FileWatcher::setFilePath(const QString& path) {
    if (mPath == path) {
        return;
    }
    if (!mPath.isEmpty()) {
        mWatcher->removePath(mPath);
        mWatcher->removePath(QFileInfo(mPath).absolutePath());
    }
    mPath = path;
    QFileInfo info(mPath);
    mLastModified = info.lastModified();
    mWatcher->addPath(mPath);
    mWatcher->addPath(info.absolutePath());
}

QString FileWatcher::filePath() const {
    return mPath;
}

void FileWatcher::onChangeDetected() {
    QFileInfo info(mPath);
    info.refresh();
    QDateTime lastModified = info.lastModified();
    if (mLastModified == lastModified) {
        // Another file in the dir changed
        return;
    }
    mLastModified = lastModified;
    mTimer->start();
}
