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
#ifndef LINEEDITCHECKER_H
#define LINEEDITCHECKER_H

#include <QObject>

#include <functional>

class QLineEdit;

using CheckFunction = std::function<QString(QString)>;

class LineEditChecker : public QObject {
public:
    LineEditChecker(QLineEdit* lineEdit, const CheckFunction& validationFunction);

protected:
    bool eventFilter(QObject* target, QEvent* event) override;

private:
    void update();
    void hideError();
    void showError(const QString& error);
    QLineEdit* const mLineEdit;
    CheckFunction mCheckFunction;
};

#endif // LINEEDITCHECKER_H
