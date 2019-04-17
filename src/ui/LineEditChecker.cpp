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
#include "LineEditChecker.h"

#include <QLineEdit>
#include <QToolTip>

LineEditChecker::LineEditChecker(QLineEdit* lineEdit, const CheckFunction& validationFunction)
        : QObject(lineEdit), mLineEdit(lineEdit), mCheckFunction(validationFunction) {
    connect(mLineEdit, &QLineEdit::textChanged, this, &LineEditChecker::onChanged);
}

void LineEditChecker::onChanged(const QString& text) {
    QString error = mCheckFunction(text);
    if (error.isEmpty()) {
        mLineEdit->setPalette(QPalette());
        QToolTip::hideText();
    } else {
        QColor errorColor = QColor::fromRgbF(1, 0.7, 0.7);
        QPalette pal(mLineEdit->parentWidget()->palette());
        pal.setBrush(QPalette::Base, errorColor);
        mLineEdit->setPalette(pal);
        QPoint pos = mLineEdit->mapToGlobal({0, mLineEdit->height()});
        QToolTip::showText(pos, error);
    }
}
