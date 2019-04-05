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

#include "lineprovider.h"

class FileWatcher;

class QFile;

class FileLineProvider : public LineProvider {
    Q_OBJECT
public:
    explicit FileLineProvider(QObject* parent = nullptr);

    const QString& lineAt(int row) const override;
    int lineCount() const override;

    void setFilePath(const QString& filePath);

private:
    void readFile();
    void reset();

    FileWatcher* const mWatcher;
    QFile* const mFile;
    QString mFilePath;
    QStringList mLines;
    qint64 mFileSize = 0;
};

#endif // FILELINEPROVIDER_H
