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
#ifndef CATCH2QTUTILS_H
#define CATCH2QTUTILS_H

// Qt
#include <QApplication>
#include <QElapsedTimer>
#include <QString>
#include <QUrl>
#include <QtTest>

// std
#include <fstream>

/**
 * Wait until the active window can be qobject_cast'ed to class T.
 * @return a pointer to the active window on success, nullptr on failure
 */
template <class T> static T waitForActiveWindow(int timeout = 5000) {
    QElapsedTimer timer;
    timer.start();
    while (!timer.hasExpired(timeout)) {
        T window = qobject_cast<T>(QApplication::activeWindow());
        if (window) {
            return window;
        }
        QTest::qWait(200);
    }
    return nullptr;
}

// Let Catch know how to print some Qt types
std::ostream& operator<<(std::ostream& ostr, const QString& str);
std::ostream& operator<<(std::ostream& ostr, const QStringList& lst);
std::ostream& operator<<(std::ostream& ostr, const QUrl& url);

#endif // CATCH2QTUTILS_H
