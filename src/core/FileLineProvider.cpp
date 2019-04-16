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
#include "FileLineProvider.h"

#include "FileWatcher.h"

#include <QDebug>
#include <QFile>
#include <QTimer>

FileLineProvider::FileLineProvider(const QString& filePath, QObject* parent)
        : LineProvider(parent)
        , mFilePath(filePath)
        , mWatcher(std::make_unique<FileWatcher>(this))
        , mFile(std::make_unique<QFile>(this)) {
    mWatcher->setFilePath(mFilePath);
    mFile->setFileName(mFilePath);

    connect(mWatcher.get(), &FileWatcher::fileChanged, this, &FileLineProvider::readFile);
    connect(mWatcher.get(), &FileWatcher::fileCreated, this, &FileLineProvider::onFileCreated);
    connect(mWatcher.get(), &FileWatcher::fileDeleted, this, &FileLineProvider::onFileDeleted);

    if (mFile->exists()) {
        QTimer::singleShot(0, this, &FileLineProvider::onFileCreated);
    }
}

FileLineProvider::~FileLineProvider() {
}

const QString& FileLineProvider::lineAt(int row) const {
    return mLines.at(row);
}

int FileLineProvider::lineCount() const {
    return mLines.length();
}

void FileLineProvider::readFile() {
    int oldLineCount = lineCount();

    qint64 fileSize = mFile->size();
    if (mFileSize > 0 && fileSize <= mFileSize) {
        qInfo() << "File truncated, rereading";
        reset();
    }

    mFileSize = fileSize;
    QString data = QString::fromUtf8(mFile->readAll());
    mLines.append(data.split('\n'));
    if (mLines.last().isEmpty()) {
        mLines.removeLast();
    }
    lineCountChanged(lineCount(), oldLineCount);
}

void FileLineProvider::reset() {
    if (mFile->isOpen()) {
        mFile->seek(0);
    }
    mFileSize = 0;
    mLines.clear();
}

void FileLineProvider::onFileCreated() {
    int oldLineCount = lineCount();
    if (!mFile->open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open" << mFilePath << ":" << mFile->errorString();
        reset();
        lineCountChanged(lineCount(), oldLineCount);
        return;
    }
    readFile();
}

void FileLineProvider::onFileDeleted() {
    int oldLineCount = lineCount();
    mFile->close();
    reset();
    lineCountChanged(lineCount(), oldLineCount);
}
