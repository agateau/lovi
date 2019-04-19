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
#ifndef FILELINEPROVIDER_H
#define FILELINEPROVIDER_H

#include "LineProvider.h"
#include "Vector.h"

#include <memory>

class FileWatcher;

class QFile;

class FileLineProvider : public LineProvider {
    Q_OBJECT
public:
    explicit FileLineProvider(const QString& filePath, QObject* parent = nullptr);
    ~FileLineProvider();

    QString lineAt(int row) const override;
    int lineCount() const override;

private:
    void readFile();
    void reset();
    void onFileDeleted();
    void onFileCreated();

    const QString mFilePath;
    const std::unique_ptr<FileWatcher> mWatcher;
    const std::unique_ptr<QFile> mFile;
    QString mContent;
    stdq::Vector<QStringRef> mLines;
    int mNextStart = 0;
    qint64 mFileSize = 0;
};

#endif // FILELINEPROVIDER_H
