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
#ifndef STDINLINEPROVIDER_H
#define STDINLINEPROVIDER_H

#include "LineProvider.h"

#include <QThread>

#include <memory>

class StdinLineProviderThread : public QThread {
    Q_OBJECT
public:
    void run() override;

signals:
    void lineReceived(const QString& line);
};

class StdinLineProvider : public LineProvider {
public:
    StdinLineProvider(QObject* parent = nullptr);
    ~StdinLineProvider();

    QString lineAt(int row) const override;
    int lineCount() const override;

private:
    void onLineReceived(const QString& line);

    const std::unique_ptr<StdinLineProviderThread> mThread;
    QStringList mLines;
};

#endif // STDINLINEPROVIDER_H
