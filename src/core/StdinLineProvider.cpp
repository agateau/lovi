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
#include "StdinLineProvider.h"

#include <QDebug>
#include <QFile>

static const int STDIN_FD = 0;
static const int POLL_INTERVAL = 500;

void StdinLineProviderThread::run() {
    QFile file;
    if (!file.open(STDIN_FD, QIODevice::ReadOnly)) {
        qWarning() << "Could not open stdin";
        return;
    }
    while (!isInterruptionRequested()) {
        QByteArray array = file.readLine();
        if (array.isEmpty()) {
            QThread::msleep(POLL_INTERVAL);
            continue;
        }
        QString line = QString::fromUtf8(array);
        lineReceived(line);
    }
}

StdinLineProvider::StdinLineProvider(QObject* parent)
        : LineProvider(parent), mThread(std::make_unique<StdinLineProviderThread>()) {
    mThread->start();
    connect(mThread.get(),
            &StdinLineProviderThread::lineReceived,
            this,
            &StdinLineProvider::onLineReceived,
            Qt::QueuedConnection);
}

StdinLineProvider::~StdinLineProvider() {
    mThread->requestInterruption();
    mThread->wait();
}

QString StdinLineProvider::lineAt(int row) const {
    return mLines.at(row);
}

int StdinLineProvider::lineCount() const {
    return mLines.count();
}

void StdinLineProvider::onLineReceived(const QString& line) {
    int oldLineCount = lineCount();
    mLines.append(line);
    lineCountChanged(lineCount(), oldLineCount);
}
