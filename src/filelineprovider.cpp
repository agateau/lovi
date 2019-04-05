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
#include "filelineprovider.h"

#include "filewatcher.h"

#include <QDebug>
#include <QFile>

FileLineProvider::FileLineProvider(QObject* parent)
        : LineProvider(parent), mWatcher(new FileWatcher(this)) {
    connect(mWatcher, &FileWatcher::fileChanged, this, &FileLineProvider::readFile);
}

const QString& FileLineProvider::lineAt(int row) const {
    return mLines.at(row);
}

int FileLineProvider::lineCount() const {
    return mLines.length();
}

void FileLineProvider::setFilePath(const QString& filePath) {
    if (mFilePath == filePath) {
        return;
    }
    mFilePath = filePath;
    mWatcher->setFilePath(mFilePath);
    readFile();
}

void FileLineProvider::readFile() {
    QFile file(mFilePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open" << mFilePath << ":" << file.errorString();
        return;
    }
    int oldLineCount = lineCount();
    qint64 fileSize = file.size();
    if (fileSize > mFileSize) {
        // Assume append
        file.seek(mFileSize);
    } else {
        // Reread all
        mLines.clear();
    }
    mFileSize = fileSize;
    QString data = QString::fromUtf8(file.readAll());
    mLines.append(data.split('\n'));
    if (mLines.last().isEmpty()) {
        mLines.removeLast();
    }
    lineCountChanged(lineCount(), oldLineCount);
}
